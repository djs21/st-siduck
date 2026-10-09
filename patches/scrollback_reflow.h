/*
 * patches/scrollback_reflow.h - declarations for the always-on
 * scrollback + reflow patch (ported from suckless
 * st-scrollback-reflow-standalone-0.9.3.diff).
 *
 * This header is included from st.c (via st_include.h) BEFORE the
 * function bodies, so any helper that st.c calls from earlier code must
 * be declared here.
 */

#ifndef ST_PATCHES_SCROLLBACK_REFLOW_H
#define ST_PATCHES_SCROLLBACK_REFLOW_H

/* Ring buffer + rendered-line helpers exposed to st.c. */
static int tlinelen_render(int y);
int tisaltscreen(void);

#endif /* ST_PATCHES_SCROLLBACK_REFLOW_H */
