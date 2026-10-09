# DOX framework

- DOX is highly performant AGENTS.md hierarchy installed here
- Agent must follow DOX instructions across any edits

## Core Contract

- AGENTS.md files are binding work contracts for their subtrees
- Work products, source materials, instructions, records, assets, and durable docs must stay understandable from the nearest applicable AGENTS.md plus every parent AGENTS.md above it

## Read Before Editing

1. Read the root AGENTS.md
2. Identify every file or folder you expect to touch
3. Walk from the repository root to each target path
4. Read every AGENTS.md found along each route
5. If a parent AGENTS.md lists a child AGENTS.md whose scope contains the path, read that child and continue from there
6. Use the nearest AGENTS.md as the local contract and parent docs for repo-wide rules
7. If docs conflict, the closer doc controls local work details, but no child doc may weaken DOX

Do not rely on memory. Re-read the applicable DOX chain in the current session before editing.

## Update After Editing

Every meaningful change requires a DOX pass before the task is done.

Update the closest owning AGENTS.md when a change affects:

- purpose, scope, ownership, or responsibilities
- durable structure, contracts, workflows, or operating rules
- required inputs, outputs, permissions, constraints, side effects, or artifacts
- user preferences about behavior, communication, process, organization, or quality
- AGENTS.md creation, deletion, move, rename, or index contents

Update parent docs when parent-level structure, ownership, workflow, or child index changes. Update child docs when parent changes alter local rules. Remove stale or contradictory text immediately. Small edits that do not change behavior or contracts may leave docs unchanged, but the DOX pass still must happen.

## Hierarchy

- Root AGENTS.md is the DOX rail: project-wide instructions, global preferences, durable workflow rules, and the top-level Child DOX Index
- Child AGENTS.md files own domain-specific instructions and their own Child DOX Index
- Each parent explains what its direct children cover and what stays owned by the parent
- The closer a doc is to the work, the more specific and practical it must be

## Child Doc Shape

- Create a child AGENTS.md when a folder becomes a durable boundary with its own purpose, rules, responsibilities, workflow, materials, or quality standards
- Work Guidance must reflect the current standards of the project or user instructions; if there are no specific standards or instructions yet, leave it empty
- Verification must reflect an existing check; if no verification framework exists yet, leave it empty and update it when one exists

Default section order:

- Purpose
- Ownership
- Local Contracts
- Work Guidance
- Verification
- Child DOX Index

## Style

- Keep docs concise, current, and operational
- Document stable contracts, not diary entries
- Put broad rules in parent docs and concrete details in child docs
- Prefer direct bullets with explicit names
- Do not duplicate rules across many files unless each scope needs a local version
- Delete stale notes instead of explaining history
- Trim obvious statements, repeated rules, misplaced detail, and warnings for risks that no longer exist

## Closeout

1. Re-check changed paths against the DOX chain
2. Update nearest owning docs and any affected parents or children
3. Refresh every affected Child DOX Index
4. Remove stale or contradictory text
5. Run existing verification when relevant
6. Report any docs intentionally left unchanged and why

## User Preferences

When the user requests a durable behavior change, record it here or in the relevant child AGENTS.md

## Purpose

Customized suckless simple terminal (`st`) fork maintained with modern features, upgraded to Upstream Suckless `st 0.9.3` base with security, compatibility, and stability fixes.

## Ownership

Root configuration, source code, build scripts, and durable operational documentation for `st-siduck`.

## Local Contracts

- **Toolchain**: C99 compliant build via standard POSIX `make`.
- **Upstream Base**: Suckless `st 0.9.3` + master stability fixes (async-safe `sigchld`, `tsetdirt` zero-size guard, bracketed paste reset, IME buffer guard, colon-separated SGR truecolor, CSI 58 undercurl bypass, OSC 110-112 color reset).
- **Integrated Patches**:
  - Harfbuzz font shaping & ligatures (`hb.c`, `hb.h`, `x.c`).
  - Custom box drawing characters (`boxdraw.c`, `boxdraw_data.h`).
  - Vim modal buffer navigation (`normalMode.c`, `normalMode.h`).
  - 32-bit visual alpha transparency (`x.c`).
  - External pipe scripts (`st-urlhandler`, `st-copyout`).
  - Scrollback with reflow-on-resize + ring buffer (`patches/`, always-on): replaces the legacy `term.hist`/`term.scr` model, forwards the mouse wheel and Shift+PageUp/Down to the running application on the alt-screen, and rewraps history on resize.
- **Scrollback Model**: history lives in a ring buffer (`Scrollback sb` in `patches/scrollback_reflow.c`), read through `renderline()`. `scrollback_lines` (config) sets capacity. `term.maxcol` is disabled so columns may shrink and reflow. On the alt-screen, wheel and Shift+PageUp/Down are forwarded to the application; otherwise they scroll `st`'s own buffer.
- **Screen-Aware Bindings**: `struct Shortcut`/`MouseShortcut`/`MouseKey` carry an `int screen` (`S_PRI`/`S_ALL`/`S_ALT`); `kpress()`/`bpress()` only fire a binding whose screen matches the current one.
- **Configuration Contract**: `config.def.h` is the source of truth for configuration; `config.h` is generated upon build.

## Work Guidance

- Never perform wholesale git merges from upstream; port upstream commits surgically to protect Harfbuzz, Boxdraw, and NormalMode hooks in `st.c` and `x.c`.
- Keep shortcut bindings free from conflict with external multiplexers (e.g. tmux).
- Keep the scrollback/reflow patch always-on and organized under `patches/` (see `patches/AGENTS.md`); do not add `#if *_PATCH` toggles or a `patches.h`.
- After editing `config.def.h`, regenerate `config.h` (`cp config.def.h config.h`) before building.
- Verify compilation after every source change with `make clean && make`.

## Verification

- **Compilation**: `make clean && make` completes without errors or warnings.
- **Version Banner**: `./st -v` reports `./st 0.9.3`.
- **Alt-screen scroll**: with a TUI (tmux/herdr) running, the wheel and Shift+PageUp/Down act inside the application and do not jump to `st`'s own scrollback.
- **Reflow**: resizing the window rewraps history without losing content.

## Child DOX Index

The repository root is otherwise flat. Module breakdown:
- `st.c`: Terminal emulation core, escape sequence parser (CSI, OSC, DCS), tty management. Scrollback and reflow live in `patches/` (see below).
- `x.c`: X11 windowing, XRender 32-bit visual, font rendering (Xft), XIM input handling, clipboard, and event dispatch.
- `hb.c`, `hb.h`: Harfbuzz shaping engine and glyph caching for font ligatures.
- `boxdraw.c`, `boxdraw_data.h`: Native pixel-perfect box drawing and block element renderer.
- `normalMode.c`, `normalMode.h`: Modal terminal buffer navigation using Vim-style motion commands.
- `config.def.h`: Source template for terminal geometry, colors, fonts, mouse actions, and shortcuts.
- `config.mk`: Make variables, library dependency paths (`pkg-config`), and package version.
- `patches/`: Always-on, flexipatch-style scrollback + reflow patch layer; owned by `patches/AGENTS.md`.
