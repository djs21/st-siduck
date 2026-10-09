/*
 * patches/x_include.c - flexipatch-style router for x.c (ALWAYS ON).
 *
 * Textually included at the END of x.c. Bodies for always-on x.c
 * patches. No #if *_PATCH guard, no patches.h.
 */

#include "scrollback_reflow_x.c"
