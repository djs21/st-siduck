/*
 * patches/scrollback_reflow.c - bodies for the always-on scrollback +
 * reflow patch (ported from suckless
 * st-scrollback-reflow-standalone-0.9.3.diff).
 *
 * Textually included at the END of st.c (via st_include.c), so it can
 * use static symbols defined earlier in st.c (term, sel, macros, ...).
 *
 * NOTE (task .1 scaffold): bodies added as the port progresses.
 */
