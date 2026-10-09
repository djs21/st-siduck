/*
 * patches/scrollback_reflow.h - declarations for the always-on
 * scrollback + reflow patch (ported from suckless
 * st-scrollback-reflow-standalone-0.9.3.diff).
 *
 * This header is included from st.c (via st_include.h) BEFORE the
 * function bodies, so any helper that st.c calls from earlier code must
 * be declared here. The Scrollback type and the `sb` instance live here
 * (not in the .c) because functions defined earlier in st.c
 * (e.g. tscrollup) reference sb and its helpers before the .c body is
 * textually included at EOF.
 */

#ifndef ST_PATCHES_SCROLLBACK_REFLOW_H
#define ST_PATCHES_SCROLLBACK_REFLOW_H

typedef struct
{
	Line *buf;       /* ring of Line pointers */
	int cap;         /* max number of lines */
	int len;         /* current number of valid lines (<= cap) */
	int head;        /* physical index of logical oldest (valid when len>0) */
	uint64_t base;   /* Can overflow in the extreme */
	/*
	 * max_width tracks the widest line ever pushed to scrollback.
	 * It may be conservative (stale) if that line has since been
	 * evicted from the ring buffer, which is acceptable - it just
	 * means we might reflow when not strictly necessary, which is
	 * better than skipping a needed reflow.
	 */
	int max_width;
	int view_offset; /* 0 means live screen */
} Scrollback;

static Scrollback sb;

/* Helpers referenced from st.c code that appears before the EOF include. */
static Line renderline(int y);
static void sb_push(Line line);
static uint64_t sb_view_start(void);
static void selscrollback(int delta);

/* Public entry points. */
static int tlinelen_render(int y);
int tisaltscreen(void);

#endif /* ST_PATCHES_SCROLLBACK_REFLOW_H */
