# ztray

Cross-platform **menu bar** and **system tray** for Zig.

| Platform | Menu bar | Tray |
|----------|----------|------|
| macOS    | AppKit `NSMenu` | `NSStatusItem` |
| Windows  | Win32 `HMENU` + `comctl32` | `Shell_NotifyIcon` |
| Linux    | DBusMenu over D-Bus | StatusNotifierItem |

An optional **in-app** DVUI menu bar is available via `ztray.dvui_menu` when you use `createZtrayDvuiModule`. The core module never imports DVUI. Tray is always native.

---

## Requirements

- Zig **0.16.0** or newer

---

## Setup

Add a path (or fetched) dependency. Import its build API with `@import("ztray")`, then use matching `target` / `optimize` values.

### Native menus / tray only

```zig
const ztray = @import("ztray");

const zmenu_mod = ztray.createZmenuModule(b, target, optimize);
const ztray_mod = ztray.createZtrayModule(b, target, optimize, zmenu_mod);
exe.addImport("zmenu", zmenu_mod);
exe.addImport("ztray", ztray_mod);
```

### Native + DVUI in-app menu bar

```zig
const ztray = @import("ztray");

const zmenu_mod = ztray.createZmenuModule(b, target, optimize);
const ztray_mod = ztray.createZtrayModule(b, target, optimize, zmenu_mod);
const zmenu_dvui = ztray.createZmenuDvuiModule(
    b,
    target,
    optimize,
    zmenu_mod,
    dvui_mod,
    sdl_mod,
);
exe.addImport("zmenu", zmenu_dvui);
exe.addImport("ztray", ztray_mod);
```

Use `zmenu.dvui_menu` for in-app calls. Use `zmenu.installMainMenuForSdlDvuiWindow` to get the native `HWND` from SDL automatically on Windows.

### Optional window frame chrome (`zwindow`)

```zig
const zwindow_mod = createZwindowModule(…);
exe.addImport("zwindow", zwindow_mod);
```

| OS | Call | Notes |
|----|------|-------|
| macOS | `zwindow.setFrameChrome(ns_window, policy)` | `.tray_compatible` / `.full_vibrancy` |
| Windows | `zwindow.setFrameChrome(hwnd, policy)` | |
| Linux | `zwindow.setFrameChrome(linux_frame_target, policy)` | X11: theme hint + opacity + KDE blur region; Wayland: clears opaque region, attaches KWin blur on `.full_vibrancy` |

Pass `-Dzwindow_unix_backends=x11,wayland` (default: both). Wayland support `dlopen`s `libwayland-client.so.0` at runtime — no static link needed.

`zwindow` is **not** re-exported from the core `ztray` module.

---

## Build

```sh
zig fetch        # resolve lazy deps (dvui, wio) for examples
zig build
zig build test   # unit tests
zig build ci     # cross-build examples (Linux: x11 / wayland / both × arches)
```

### Build flags

| Flag | Meaning |
|------|---------|
| `-Dforce_dvui_menu=true` | Force DVUI in-app menu bar in the `ztray-dvui` sample |
| `-Dzwindow_unix_backends` | Comma list: `x11`, `wayland` (default: `x11,wayland`) |

### Examples

| Step | Binary | Notes |
|------|--------|-------|
| `zig build run-tray` | `ztray-minimal` | Tray icon + context menu only. |
| `zig build run-dvui` | `zmenu-dvui` | DVUI window + ztray. Native menu by default; `-Dforce_dvui_menu=true` for in-app. |
| `zig build run-wio` | `zmenu-wio` | [wio](https://github.com/ypsvlq/wio) window + native menus only. |
| `zig build run-wio-window` | `zwindow-wio` | wio + `zwindow.setFrameChrome(.full_vibrancy)`. |
| `zig build run-wio-tray` | `ztray-wio-tray` | wio + native menu bar + tray + `zwindow.setFrameChrome`. |

On Linux, wio and tray examples link dynamically where applicable.
