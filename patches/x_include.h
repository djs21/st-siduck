/*
 * patches/x_include.h - flexipatch-style router for x.c (ALWAYS ON).
 *
 * Textually included inside x.c after "config.h". Declarations for
 * always-on patches that hook into x.c. No #if *_PATCH guard, no
 * patches.h.
 *
 * NOTE (task .1 scaffold): the mouse-wheel hook declaration is added
 * in task .6, when bpress() is actually wired to call it.
 */

#ifndef ST_PATCHES_X_INCLUDE_H
#define ST_PATCHES_X_INCLUDE_H

#endif /* ST_PATCHES_X_INCLUDE_H */
