# ztray task backlog

Audit date: 2026-09-19  
Scope: tracked first-party source, package/build configuration, examples, documentation, and repository metadata. Items marked **Verified** are directly evidenced in the current codebase. Items marked **Suggested** are improvements rather than confirmed defects.

## Release blockers

- [ ] **Fix the published package allowlist.** **Verified.** Add `vendor` and `LICENSE` to `.paths` in `build.zig.zon`. `src/windows_tray_png.c` includes `../vendor/stb_image.h`, but `vendor` is not package-listed, so a fetched dependency can fail to build on Windows. Validate a clean consumer project on Windows, macOS, and Linux. (`build.zig.zon:23-30`, `src/windows_tray_png.c:7-9`)
- [ ] **Make README setup snippets compile.** **Verified.** The documented `createZtrayModule` call omits its required `zmenu_mod` argument; `createZtrayDvuiModule` does not exist—the factory is `createZmenuDvuiModule`. Include a complete dependency-import example. (`README.md:27-38`, `build.zig:92-107`, `build.zig:146-164`)
- [ ] **Correct README example executable names.** **Verified.** Documented binary names disagree with declared executable names. Prefer stable `zig build run-*` commands if binary names are not part of the API. (`README.md:77-85`, `build.zig:299`, `build.zig:313-315`, `build.zig:335-336`, `build.zig:346-348`)
- [ ] **Establish real CI.** **Verified.** A local `zig build ci` step exists, but no repository-hosted workflow is tracked. Add native Linux, Windows, and macOS workflows, formatting/static-analysis checks, and clean package-consumer validation. (`build.zig:366-446`)

## P1 — correctness, lifecycle, and reliability

### Linux D-Bus and native shell integration

- [ ] **Validate all incoming D-Bus method signatures.** **Verified.** Check each argument type before `dbus_message_iter_get_basic` for `Properties.Get`, `GetLayout`, `Event`, `EventGroup`, and `AboutToShow`; return D-Bus `InvalidArgs` on malformed messages. (`src/linux_dbus_menu.c:304-323`, `src/linux_dbus_menu.c:339-347`, `src/linux_dbus_menu.c:408-421`)
- [ ] **Remove long synchronous D-Bus waits from UI-facing setup.** **Verified.** AppMenu registrar discovery can block for five seconds and StatusNotifier registration can block for three. Use asynchronous calls or short/configurable timeouts with retry/cache behavior. (`src/linux_dbus_menu.c:503-563`, `src/linux_dbus_menu.c:438-464`, `src/zmenu_with_dvui.zig:41-49`)
- [ ] **Support supplied PNG tray icons on Linux.** **Verified.** `icon_png` and `icon_file` are ignored; only a Freedesktop icon name is published. Implement StatusNotifierItem `IconPixmap`, support icon updates with `NewIcon`, and document theme lookup/fallback. (`src/ztray.zig:43-47`, `src/ztray.zig:483-494`, `src/linux_dbus_menu.c:213-220`, `src/linux_dbus_menu.c:275-284`)
- [ ] **Propagate menu shortcut metadata over D-BusMenu.** **Verified.** Linux flattening and D-Bus properties omit the common model’s `shortcut` data. Define shortcut mapping, emit the D-BusMenu `shortcut` property, and document display versus activation behavior. (`src/types.zig:63-88`, `src/linux.zig:89-101`, `src/linux_dbus_menu.c:92-107`)
- [ ] **Replace or surface the 512-item Linux menu limit.** **Verified.** Fixed C arrays reject menus after flattening 512 items, but callers receive generic errors. Use dynamic storage or prevalidate and return `MenuTooLarge`. (`src/linux_dbus_menu.c:17`, `src/linux_dbus_menu.c:27-35`, `src/linux_dbus_menu.c:601-644`, `src/linux.zig:107-127`)
- [ ] **Run Linux D-Bus functionality on a native runner.** **Verified.** Cross-builds select a no-op D-Bus stub, so they do not test real Linux behavior. Add isolated-session-bus tests on Linux. (`build.zig:44-51`, `src/linux_dbus_stub.c:12-35`)

### Lifecycle and event semantics

- [ ] **Define and implement main-menu lifecycle semantics.** **Verified.** macOS and Windows reject repeated installation; Linux silently accepts but does nothing. Add a public teardown/replacement API, or consistently return a documented `MenuAlreadyInstalled` error. (`src/macos_menu.m:94-108`, `src/macos_menu.m:152-157`, `src/windows.zig:66-68`, `src/windows.zig:177-180`, `src/linux.zig:107-109`, `src/zmenu.zig:45-65`)
- [ ] **Clean up Windows native menubar resources.** **Verified.** Menu construction has error exits after `CreateMenu`/`CreatePopupMenu` and no regular destruction path. Add transactional creation, `errdefer` cleanup, normal detach/destroy, and subclass cleanup. (`src/windows.zig:81-86`, `src/windows.zig:182-195`)
- [ ] **Make tray menu replacement transactional.** **Verified.** Existing native menus/items are discarded before replacement is fully constructed. Build temporary state first and swap only after successful construction. (`src/macos.zig:111-115`, `src/windows.zig:377-390`, `src/linux_dbus_menu.c:624-644`)
- [ ] **Define action-delivery behavior.** **Verified.** Every backend uses one atomic action-ID slot, so a later action overwrites an unpolled action. Implement a bounded FIFO with defined overflow behavior, or document “latest action wins.” (`src/macos_menu.m:25-29`, `src/macos_tray.m:23-28`, `src/windows.zig:66-69`, `src/linux_dbus_menu.c:394-421`, `src/zmenu_dvui.zig:7`, `src/zmenu.zig:76-85`)
- [ ] **Validate action IDs uniformly.** **Verified.** Linux/Windows reject negative IDs, but macOS accepts them even though polling treats negative values as “none.” Validate at the public boundary and document the supported range. (`src/zmenu.zig:76-85`, `src/linux.zig:89-91`, `src/windows.zig:161-165`, `src/macos_menu.m:117-127`, `src/macos_tray.m:96-106`)
- [ ] **Publish a complete main-thread/thread-safety contract.** **Verified.** `zmenu` partially documents main-thread installation, but tray/window APIs and mutable global state lack equivalent rules. State the contract for installation, updates, polling, pumping, and teardown; add assertions where practical. (`src/zmenu.zig:6-10`, `src/ztray.zig:19-85`, `src/windows.zig:66-77`, `src/linux_dbus_menu.c:27-40`)

### DVUI and zwindow parity

- [ ] **Do not silently discard DVUI shortcut synchronization failures.** **Verified.** Initial `syncKeybinds` errors are ignored. Propagate, report, or explicitly document retry behavior. (`src/zmenu_with_dvui.zig:45-49`, `src/zmenu_dvui.zig:117-145`)
- [ ] **Remove or explicitly expose DVUI’s 128-shortcut limit.** **Verified.** Fixed tracking storage returns `OutOfMemory` independently of allocator capacity. Use allocator-backed storage or a dedicated `TooManyShortcuts` error. (`src/zmenu_dvui.zig:16-18`, `src/zmenu_dvui.zig:125-138`)
- [ ] **Resolve zwindow option no-ops.** **Verified.** Windows ignores supplied color, alpha, and dark settings; X11/Wayland accept fields that are currently unused. Implement supported fields, split platform-specific options, or clearly document no-ops. (`src/window_common.zig:39-47`, `src/window_windows.zig:241-257`, `src/window_linux_x11.zig:1-3`, `src/window_linux_wayland.zig:1-7`)
- [ ] **Fix/document Wayland resource lifetime limits.** **Verified.** One display is supported, the registry/dynamic library are process-lifetime, and a 16-entry blur cache deliberately leaks when full. Provide deinitialization and growable storage, or make limits explicit and fail safely. (`src/window_linux_wayland.zig:115-130`, `src/window_linux_wayland.zig:201-208`, `src/window_linux_wayland.zig:249-252`, `src/window_linux_wayland.zig:272-282`)

## P2 — automated verification

- [ ] **Broaden unit coverage.** **Verified.** Current tests only cover shortcut-formatting helpers. Add tests for ID validation, item flattening, 512-item handling, lifecycle/replacement state transitions, and tray/menu error paths. (`src/types.zig:137-202`, `build.zig:356-364`)
- [ ] **Add Linux D-Bus protocol integration tests.** Exercise registrar detection, StatusNotifier registration, menu layout/property responses, malformed calls, action events, and watcher absence using an isolated session bus.
- [ ] **Add native smoke tests for macOS and Windows.** Cover AppKit/Win32 menu/tray initialization, resource teardown, and basic action dispatch on real runners.
- [ ] **Test DVUI behavior.** Verify shortcut registration, cleanup, fallback mode selection, and capacity/error behavior.
- [ ] **Test zwindow backends.** Add focused platform smoke harnesses for macOS, Windows, X11, and Wayland behavior and degradation paths.
- [ ] **Expand target coverage policy.** **Verified.** Existing compile CI omits explicit macOS architectures and Windows ARM64. Publish a support matrix and test each supported target. (`build.zig:373-385`)
- [ ] **Automate formatting and C/Objective-C analysis.** Add `zig fmt` checking plus Clang warnings/static analysis and sanitizer-capable test jobs for native sources.

## P2 — documentation and developer experience

- [ ] **Expand public platform documentation.** **Verified.** README ends after basic build/examples and omits lifecycle, platform limitations, and operational details. Document supported OS/architectures/Zig versions, event loop/pump requirements, thread rules, action semantics, ownership, and teardown. (`README.md:15-87`)
- [ ] **Document Linux desktop requirements and troubleshooting.** Cover session D-Bus, AppMenu registrar, StatusNotifier watcher, icon-theme lookup, desktops without support, and DVUI fallback behavior. (`src/zmenu_with_dvui.zig:1-4`, `src/linux_dbus_menu.c:438-464`)
- [ ] **Document lifecycle/error recovery.** Show initialization, installation, polling/pumping, shutdown, retry behavior, and reinstallation constraints for both menu and tray APIs. (`src/ztray.zig:78-85`, `src/zmenu.zig:35-43`)
- [ ] **Publish a zwindow support matrix.** Clearly identify each `FrameChrome` field’s support/degradation semantics by platform and compositor.
- [ ] **Add API documentation for ownership and input limits.** Specify retained/copied string and PNG byte lifetimes, maximum item counts, supported action IDs, and thread affinity.

## P2 — release, governance, and supply chain

- [ ] **Create an initial release process.** **Verified.** Package version is `0.0.0`; repository has no tags or changelog. Choose versioning policy, create changelog/release notes, tag releases, and document compatibility policy. (`build.zig.zon:2-5`)
- [ ] **Add contribution and security policies.** **Verified.** Add `CONTRIBUTING.md`, `SECURITY.md`, issue/PR templates, maintenance/support policy, and a release checklist.
- [ ] **Review third-party attribution and provenance.** **Verified review task.** Verify licenses and redistribution obligations for vendored `stb_image` and pinned dependencies; publish third-party notices and preferably an SBOM/provenance record. (`vendor/stb_image.h`, `build.zig.zon:6-21`, `LICENSE:1-21`)
- [ ] **Add dependency maintenance controls.** Keep pins, but add automated update review, reproducibility/cache verification, and vulnerability monitoring where applicable.

## P3 — suggested product and maintainability improvements

- [ ] **Consider richer menu items.** **Suggested.** Add nested submenus, checked/radio states, dynamic labels, and item icons to the common model. (`src/types.zig:77-98`)
- [ ] **Consider explicit tray click events.** **Suggested.** Expose click/double-click callbacks or polling events in addition to context-menu actions. (`src/windows.zig:27-36`, `src/windows.zig:434-457`)
- [ ] **Add PNG resource limits.** **Suggested.** Enforce maximum dimensions/decoded byte size before decoding caller-supplied icon PNGs on Windows. (`src/windows_tray_png.c:11-56`)
- [ ] **Improve type safety around native handles.** **Suggested.** Replace raw `*anyopaque` inputs with typed platform wrappers/helper constructors where feasible. (`src/window_root.zig:35-58`, `src/window_common.zig:3-27`)
- [ ] **Reduce duplicated backend logic.** **Suggested.** Consolidate overlapping platform tray/menu implementation paths to reduce drift. (`src/ztray.zig:99-537`, `src/macos.zig`, `src/windows.zig`, `src/linux.zig`)
- [ ] **Stabilize and namespace build APIs.** **Suggested.** Give public module factories a documented naming/compatibility policy to avoid further setup drift. (`build.zig:73-164`)

## Recommended implementation order

1. Package allowlist and README correctness.
2. Native CI and clean-consumer package validation.
3. Linux D-Bus argument validation and blocking-startup mitigation.
4. Explicit menubar/tray lifecycle with transactional updates and Windows cleanup.
5. Action-ID and action-queue semantics.
6. Linux icon/shortcut parity and documented capacity limits.
7. Broader unit, protocol, and native smoke coverage.
8. zwindow/DVUI limits and platform support documentation.
9. Release governance, attribution, and dependency-maintenance process.
