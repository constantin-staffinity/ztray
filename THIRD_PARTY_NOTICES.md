# Third-party notices

## Vendored source

### stb_image v2.30

`vendor/stb_image.h` is authored by Sean Barrett and contributors. It is distributed under the public domain or the MIT License, at the recipient's option. Its full license text is retained in the vendored header.

## Build dependencies

The following dependencies are pinned by commit and hash in `build.zig.zon`:

- dvui — used lazily by the DVUI example and optional in-app menu module.
- wio — used lazily by native-window examples.
- zigwin32 — used lazily by Windows frame-chrome support.

Each release must review the dependency revisions and their upstream licensing before publication. Dependency source is not redistributed by this repository unless included by a package manager or consumer build.
