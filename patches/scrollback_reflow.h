/*
 * patches/scrollback_reflow.h - declarations for the always-on
 * scrollback + reflow patch (ported from suckless
 * st-scrollback-reflow-standalone-0.9.3.diff).
 *
 * This header is included from st.c (via st_include.h) BEFORE the
 * function bodies, so any helper that st.c calls from earlier code must
 * be declared here.
 *
 * NOTE (task .1 scaffold): declarations added as the port progresses.
 */

#ifndef ST_PATCHES_SCROLLBACK_REFLOW_H
#define ST_PATCHES_SCROLLBACK_REFLOW_H

/* Added in later tasks:
 *  - Scrollback ring buffer + sb_* helpers + renderline  (task .2)
 *  - kscrollup / kscrolldown, tlinelen(Line)             (task .3)
 *  - tisaltscreen()                                      (task .2)
 */

#endif /* ST_PATCHES_SCROLLBACK_REFLOW_H */
