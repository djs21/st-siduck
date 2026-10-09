/*
 * patches/st_include.c - flexipatch-style router (ALWAYS ON).
 *
 * Textually included at the END of st.c. Provides the bodies for all
 * always-on patches. No #if *_PATCH guard, no patches.h.
 */

#include "scrollback_reflow.c"
