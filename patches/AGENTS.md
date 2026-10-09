# patches/

## Purpose

Always-on, flexipatch-style patch layer for `st-siduck`. It ports the
upstream suckless **scrollback + reflow (standalone)** patch
(`st-scrollback-reflow-standalone-0.9.3.diff`) into the tree without
rewriting `st.c`/`x.c` in place.

The layer replaces the legacy scrollback model:
- history lives in a ring buffer (`Scrollback sb`) instead of
  `term.hist`/`term.histi`/`term.scr`;
- rendering and selection read lines through `renderline()`;
- resize rewraps history (`sb_resize`) when the width changes;
- `term.maxcol` is disabled so columns may shrink.

On the alt-screen (a running TUI such as tmux/herdr) the mouse wheel and
Shift+PageUp/Down are forwarded to the application; otherwise they scroll
`st`'s own buffer.

## Ownership

Patch headers, patch bodies, and the include routers that wire them into
`st.c` and `x.c`. The parent `../AGENTS.md` owns the build, toolchain, and
configuration contract; this file owns how the patch layer is structured
and included.

## Local Contracts

- **Always-on**: no `#if *_PATCH` guards and no generated `patches.h`.
  Everything in this folder is compiled unconditionally.
- **Flexipatch-style routers**: patch bodies are textually `#include`d,
  not built as separate translation units, so `static` helpers and macros
  stay visible to `st.c`/`x.c`:
  - `st.c` includes `patches/st_include.h` after its globals and
    `patches/st_include.c` at end of file.
  - `x.c` includes `patches/x_include.h` after `config.h` and
    `patches/x_include.c` at end of file.
  - `st_include.h` -> `scrollback_reflow.h`; `st_include.c` ->
    `scrollback_reflow.c`; `x_include.h`/`x_include.c` are the x.c
    router (body file `scrollback_reflow_x.c` when needed).
- **Type/instance placement**: `Scrollback` and `static Scrollback sb`
  live in `scrollback_reflow.h` (not the `.c`) because `st.c` functions
  defined before the EOF include (e.g. `tscrollup`) reference `sb`.
- **Forward declarations**: any helper `st.c` calls before the EOF
  include must be declared in `scrollback_reflow.h`.
- **Symbols**: patch bodies may define public entry points used from
  `config.h`/`x.c` (`kscrollup`, `kscrolldown`, `ttysend`); declare them
  in `../st.h`.
- **Makefile**: object rules depend on the patch headers, and `$(OBJ)`
  depends on all `patches/*.h`/`patches/*.c`, so edits rebuild.
- **Not toggleable**: to add/remove behavior, edit the patch body
  directly; do not introduce feature macros.

## Work Guidance

- Port upstream hunks faithfully; when a hunk clashes with `st-siduck`
  specifics (e.g. `xdrawcursor` takes 8 args, `bpress` uses
  `forceselmod`), adapt the call site, not the upstream logic.
- When moving a function out of `st.c` into the patch layer, delete the
  `st.c` copy to avoid duplicate symbols and leave a one-line comment
  marking where it moved.
- After editing `config.def.h`, regenerate `config.h`
  (`cp config.def.h config.h`) before building.
- `scrollback_lines` (config) sets ring-buffer capacity.

## Verification

- `make clean && make` completes with no errors and no new warnings
  (run with `-Wall`; only the pre-existing `x.c` dangling-else,
  `selclear_`, and `su_timeout` warnings may remain).
- `./st -v` reports `./st 0.9.3`.
- Manual: wheel and Shift+PageUp/Down act inside a running TUI
  (tmux/herdr) and do not jump to `st`'s scrollback; resizing rewraps
  history without loss.

## Child DOX Index

No subdirectories. Files:
- `st_include.h`, `st_include.c`: routers that pull the patch body into
  `st.c`.
- `x_include.h`, `x_include.c`: routers that pull x.c patch bodies into
  `x.c` (`x_include.c` -> `scrollback_reflow_x.c`).
- `scrollback_reflow.h`: `Scrollback` type, `sb` instance, and forward
  declarations for the ring buffer, reflow, and scroll helpers.
- `scrollback_reflow.c`: ring buffer (`sb_*`), reflow engine
  (`sb_resize`), `renderline`/`tlinelen`/`tlinelen_render`,
  `kscrollup`/`kscrolldown`, `ttysend`, `tisaltscreen`, and the
  selection/scroll helpers.
- `scrollback_reflow_x.c`: x.c-side bodies (currently empty).
