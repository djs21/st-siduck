/*
 * patches/scrollback_reflow.c - bodies for the always-on scrollback +
 * reflow patch (ported from suckless
 * st-scrollback-reflow-standalone-0.9.3.diff).
 *
 * Textually included at the END of st.c (via st_include.c), so it can
 * use static symbols defined earlier in st.c (term, sel, macros, ...).
 */

/* ---- Line length (ported: diff st.c hunk @@ -404,20 +778,23) ---- */
static int
tlinelen(Line line)
{
	int i = term.col;

	if (line[i - 1].mode & ATTR_WRAP)
		return i;

	while (i > 0 && line[i - 1].u == ' ')
		--i;

	return i;
}

/* ---- Ring buffer core (ported: diff st.c hunk @@ -232,6 +233,379) ---- */
/* typedef Scrollback + `static Scrollback sb;` live in
 * patches/scrollback_reflow.h so earlier st.c code can reference them. */
static int
sb_phys_index(int logical_idx)
{
	/* logical_idx: 0..sb.len-1 (0 = oldest) */
	return (sb.head + logical_idx) % sb.cap;
}

static Line
lineclone(Line src)
{
	Line dst;

	if (!src)
		return NULL;

	dst = xmalloc(term.col * sizeof(Glyph));
	memcpy(dst, src, term.col * sizeof(Glyph));
	return dst;
}

static void
sb_init(int lines)
{
	int i;

	sb.buf  = xmalloc(sizeof(Line) * lines);
	sb.cap  = lines;
	sb.len  = 0;
	sb.head = 0;
	sb.base = 0;
	for (i = 0; i < sb.cap; i++)
		sb.buf[i] = NULL;

	sb.view_offset = 0;
	sb.max_width = 0;
}

/* Push one screen line into scrollback.
 * Overwrites oldest when full (ring buffer).
 */
static void
sb_push(Line line)
{
	Line copy;
	int tail;
	int width;

	if (sb.cap <= 0)
		return;

	copy = lineclone(line);

	if (sb.len < sb.cap) {
		tail = sb_phys_index(sb.len);
		sb.buf[tail] = copy;
		sb.len++;
	} else {
		/* We might've just evicted the widest line... */
		free(sb.buf[sb.head]);
		sb.buf[sb.head] = copy;
		sb.head = (sb.head + 1) % sb.cap;
		sb.base++;
	}
	width = tlinelen(copy);
	/* ...so max_width might be stale. */
	if (width > sb.max_width)
		sb.max_width = width;
}

static Line
sb_get(int idx)
{
	/* idx is logical: 0..sb.len-1 */
	if (idx < 0 || idx >= sb.len)
		return NULL;
	return sb.buf[sb_phys_index(idx)];
}

static void
sb_clear(void)
{
	int i;
	int p;

	if (!sb.buf)
		return;

	for (i = 0; i < sb.len; i++) {
		p = sb_phys_index(i);
		if (sb.buf[p]) {
			free(sb.buf[p]);
			sb.buf[p] = NULL;
		}
	}

	sb.len = 0;
	sb.head = 0;
	sb.base = 0;
	sb.view_offset = 0;
	sb.max_width = 0;
}

/*
 * Reflows the scrollback buffer to fit a new terminal width.
 *
 * The algorithm works in three steps:
 * 1) Unwrap: It iterates through the existing history, joining physical lines
 * marked with ATTR_WRAP into a single continuous 'logical' line.
 * 2) Reflow: It slices this logical line into new chunks of size 'col'.
 * - New wrap flags are applied where the text exceeds the new width.
 * - Trailing spaces are trimmed to prevent ghost padding.
 * 3) Rebuild: The new lines are pushed into a fresh ring buffer.
 * - Uses O(1) ring insertion (updating head/tail) to avoid expensive
 * memmoves during resize, but it is still O(N) where N is the existing
 * history.
 *
 * Note: During reflow we reset sb to match the rebuilt buffer
 * (head, base and len might change).
 */
static void
sb_resize(int col)
{
	Line *new_buf;
	int i, j;
	int new_len, logical_cap, logical_len, is_wrapped, cursor;
	int copy_width, tail, current_width;
	Line logical, line, nl;
	uint64_t new_base = 0;
	int new_head = 0;
	int new_max_width = 0;
	Glyph *g;

	new_len = 0;

	if (sb.len == 0)
		return;

	new_buf = xmalloc(sizeof(Line) * sb.cap);
	for (i = 0; i < sb.cap; i++)
		new_buf[i] = NULL;

	logical_cap = term.col * 2;
	logical = xmalloc(logical_cap * sizeof(Glyph));
	logical_len = 0;

	for (i = 0; i < sb.len; i++) {
		/* Unwrap: Accumulate physical lines into one logical line. */
		line = sb_get(i);
		is_wrapped = (line[term.col - 1].mode & ATTR_WRAP);
		if (logical_len + term.col > logical_cap) {
			logical_cap *= 2;
			logical = xrealloc(logical, logical_cap * sizeof(Glyph));
		}

		memcpy(logical + logical_len, line, term.col * sizeof(Glyph));
		for (j = 0; j < term.col; j++) {
			logical[logical_len + j].mode &= ~ATTR_WRAP;
		}
		logical_len += term.col;
		/* If the line was wrapped, continue accumulating before reflowing. */
		if (is_wrapped) {
			continue;
		}
		/* Trim trailing spaces from the fully unwrapped line. */
		while (logical_len > 0) {
			g = &logical[logical_len - 1];
			if (g->u == ' ' && g->bg == defaultbg
					&& (g->mode & ATTR_BOLD) == 0) {
				logical_len--;
			} else {
				break;
			}
		}
		if (logical_len == 0)
			logical_len = 1;

		/* Reflow: Split the logical line into new chunks. */
		cursor = 0;
		while (cursor < logical_len) {
			nl = xmalloc(col * sizeof(Glyph));
			for (j = 0; j < col; j++) {
				nl[j].fg = defaultfg;
				nl[j].bg = defaultbg;
				nl[j].mode = 0;
				nl[j].u = ' ';
			}

			copy_width = logical_len - cursor;
			if (copy_width > col)
				copy_width = col;

			memcpy(nl, logical + cursor, copy_width * sizeof(Glyph));

			for (j = 0; j < copy_width; j++) {
				nl[j].mode &= ~ATTR_WRAP;
			}

			if (cursor + copy_width < logical_len) {
				nl[col - 1].mode |= ATTR_WRAP;
			} else {
				nl[col - 1].mode &= ~ATTR_WRAP;
			}

			/* Rebuild: Push new lines into the ring buffer. */
			if (new_len < sb.cap) {
				tail = (new_head + new_len) % sb.cap;
				new_buf[tail] = nl;
				new_len++;
			} else {
				free(new_buf[new_head]);
				new_buf[new_head] = nl;
				new_head = (new_head + 1) % sb.cap;
				new_base++;
			}
			current_width = (cursor + copy_width < logical_len) ? col : copy_width;
			if (current_width > new_max_width)
				new_max_width = current_width;
			cursor += copy_width;
		}
		logical_len = 0;
	}
	free(logical);
	sb_clear();
	free(sb.buf);
	sb.buf = new_buf;
	sb.len = new_len;
	sb.head = new_head;
	sb.base = new_base;
	sb.view_offset = 0;
	sb.max_width = new_max_width;
}

static void
sb_pop_screen(int loaded, int new_cols)
{
	int i, p;
	int start_logical;
	Line line;

	loaded = MIN(loaded, sb.len);
	start_logical = sb.len - loaded;
	new_cols = MIN(new_cols, term.col);
	for (i = 0; i < loaded; i++) {
		p = sb_phys_index(start_logical + i);
		line = sb.buf[p];

		memcpy(term.line[i], line, new_cols * sizeof(Glyph));

		free(line);
		sb.buf[p] = NULL;
	}

	sb.len -= loaded;
}

static uint64_t
sb_view_start(void)
{
	return sb.base + sb.len - sb.view_offset;
}

static void
sb_view_changed(void)
{
	if (!term.dirty || term.row <= 0)
		return;
	tfulldirt();
}

static void
selscrollback(int delta)
{
	if (delta == 0)
		return;

	if (sel.ob.x == -1 || sel.mode == SEL_EMPTY)
		return;

	if (sel.alt != IS_SET(MODE_ALTSCREEN))
		return;

	sel.nb.y += delta;
	sel.ne.y += delta;
	sel.ob.y += delta;
	sel.oe.y += delta;

	if (sel.ne.y < 0 || sel.nb.y >= term.row)
		selclear();

	sb_view_changed();
}

static Line
emptyline(void)
{
	static Line empty;
	static int empty_cols;
	int i = 0;

	if (empty_cols != term.col) {
		free(empty);
		empty = xmalloc(term.col * sizeof(Glyph));
		empty_cols = term.col;
	}

	for (i = 0; i < term.col; i++) {
		empty[i] = term.c.attr;
		empty[i].u = ' ';
		empty[i].mode = 0;
	}
	return empty;
}

static Line
renderline(int y)
{
	int start, v;

	if (sb.view_offset <= 0)
		return term.line[y];

	start = sb.len - sb.view_offset; /* can be negative */
	v = start + y;

	if (v < 0)
		return emptyline();

	if (v < sb.len)
		return sb_get(v);

	/* past scrollback -> into current screen */
	v -= sb.len;
	if (v >= 0 && v < term.row)
		return term.line[v];

	return emptyline();
}

static void
sb_reset_on_clear(void)
{
	sb_clear();
	sb_view_changed();
	if (sel.ob.x != -1 && term.row > 0)
		selclear();
}

int
tisaltscreen(void)
{
	return IS_SET(MODE_ALTSCREEN);
}


/* ---- Rendered-line length (ported: diff st.c hunk @@ -404,20 +778,23) ---- */
static int
tlinelen_render(int y)
{
	return tlinelen(renderline(y));
}

/* ---- Scroll commands (ported: diff st.c hunk @@ -2163,6 +2601,46) ---- */
static void
kscroll(const Arg *arg)
{
	uint64_t oldstart;
	uint64_t newstart;

	oldstart = sb_view_start();
	sb.view_offset += arg->i;
	LIMIT(sb.view_offset, 0, sb.len);
	newstart = sb_view_start();
	selscrollback(oldstart - newstart);
	redraw();
}

void
kscrolldown(const Arg *arg)
{
	Arg a;

	if (arg->i < 0)
		a.i = -term.row;
	else
		a.i = -arg->i;

	kscroll(&a);
}

void
kscrollup(const Arg *arg)
{
	Arg a;

	if (arg->i < 0)
		a.i = term.row;
	else
		a.i = arg->i;

	kscroll(&a);
}
