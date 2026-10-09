/*
 * patches/st_include.h - flexipatch-style router (ALWAYS ON).
 *
 * This header is textually included inside st.c (after the static
 * prototypes / globals, before function bodies). It pulls in the
 * declarations for every always-on patch. There is intentionally NO
 * #if *_PATCH guard and NO patches.h: the scrollback-reflow patch is
 * permanently enabled in this fork.
 */

#include "scrollback_reflow.h"
