# ztray task backlog

Audit date: 2026-09-19  
Scope: tracked first-party source, package/build configuration, examples, documentation, and repository metadata. Items marked **Verified** are directly evidenced in the current codebase. Items marked **Suggested** are improvements rather than confirmed defects.

## Release blockers

- [x] **Fix the published package allowlist.** Completed in `91c399a`: added `vendor` and `LICENSE` to `.paths`. Clean fetched-consumer validation remains part of CI follow-up. (`build.zig.zon:23-32`)
- [x] **Make README setup snippets compile.** Completed in `2faa09c`: documented the required `zmenu` module and `createZmenuDvuiModule`. (`README.md:21-55`)
- [x] **Correct README example executable names.** Completed in `2d0e850`. (`README.md:94-102`)
- [x] **Establish real CI.** Completed in `7329058`: added native Linux, Windows, and macOS build/test workflow. Formatting/static analysis and clean package-consumer validation remain follow-up tasks. (`.github/workflows/ci.yml`)

## P1 — correctness, lifecycle, and reliability

### Linux D-Bus and native shell integration

- [x] **Validate all incoming D-Bus method signatures.** Completed in `db7aaf9`: validate `Properties.Get`, `GetLayout`, `Event`, `EventGroup`, and `AboutToShow` signatures and return D-Bus `InvalidArgs`. (`src/linux_dbus_menu.c:305-438`)
- [x] **Remove long synchronous D-Bus waits from UI-facing setup.** Completed in `ccd2196` and `763ee37`: StatusNotifier registration and AppMenu registrar discovery now use nonblocking D-Bus requests. Registrar lookup is cached for the connection lifetime; retry policy remains a future enhancement. (`src/linux_dbus_menu.c:455-479`, `src/linux_dbus_menu.c:516-574`)
- [ ] **Support supplied PNG tray icons on Linux.** **Verified.** `icon_png` and `icon_file` are ignored; only a Freedesktop icon name is published. Implement StatusNotifierItem `IconPixmap`, support icon updates with `NewIcon`, and document theme lookup/fallback. (`src/ztray.zig:43-47`, `src/ztray.zig:483-494`, `src/linux_dbus_menu.c:213-220`, `src/linux_dbus_menu.c:275-284`)
- [ ] **Propagate menu shortcut metadata over D-BusMenu.** **Verified.** Linux flattening and D-Bus properties omit the common model’s `shortcut` data. Define shortcut mapping, emit the D-BusMenu `shortcut` property, and document display versus activation behavior. (`src/types.zig:63-88`, `src/linux.zig:89-101`, `src/linux_dbus_menu.c:92-107`)
- [ ] **Replace or surface the 512-item Linux menu limit.** **Verified.** Fixed C arrays reject menus after flattening 512 items, but callers receive generic errors. Use dynamic storage or prevalidate and return `MenuTooLarge`. (`src/linux_dbus_menu.c:17`, `src/linux_dbus_menu.c:27-35`, `src/linux_dbus_menu.c:601-644`, `src/linux.zig:107-127`)
- [ ] **Run Linux D-Bus functionality on a native runner.** **Verified.** Cross-builds select a no-op D-Bus stub, so they do not test real Linux behavior. Add isolated-session-bus tests on Linux. (`build.zig:44-51`, `src/linux_dbus_stub.c:12-35`)

### Lifecycle and event semantics

- [ ] **Define and implement main-menu lifecycle semantics.** **Verified.** macOS and Windows reject repeated installation; Linux silently accepts but does nothing. Add a public teardown/replacement API, or consistently return a documented `MenuAlreadyInstalled` error. (`src/macos_menu.m:94-108`, `src/macos_menu.m:152-157`, `src/windows.zig:66-68`, `src/windows.zig:177-180`, `src/linux.zig:107-109`, `src/zmenu.zig:45-65`)
- [ ] **Clean up Windows native menubar resources.** **Verified.** Menu construction has error exits after `CreateMenu`/`CreatePopupMenu` and no regular destruction path. Add transactional creation, `errdefer` cleanup, normal detach/destroy, and subclass cleanup. (`src/windows.zig:81-86`, `src/windows.zig:182-195`)
- [ ] **Make tray menu replacement transactional.** Linux completed in `4f4a2dd`; macOS and Windows replacements remain destructive. Build temporary state first and swap only after successful construction. (`src/macos.zig:111-115`, `src/windows.zig:377-390`, `src/linux_dbus_menu.c:612-658`)
- [ ] **Define action-delivery behavior.** **Verified.** Every backend uses one atomic action-ID slot, so a later action overwrites an unpolled action. Implement a bounded FIFO with defined overflow behavior, or document “latest action wins.” (`src/macos_menu.m:25-29`, `src/macos_tray.m:23-28`, `src/windows.zig:66-69`, `src/linux_dbus_menu.c:402-429`, `src/zmenu_dvui.zig:7`, `src/zmenu.zig:79-88`)
- [x] **Validate action IDs uniformly.** Completed in `6b68084`: the public menu/tray boundaries reject negative action IDs before platform dispatch. (`src/types.zig:100-112`, `src/zmenu.zig:58-67`, `src/ztray.zig:53-61`)
- [ ] **Publish a complete main-thread/thread-safety contract.** **Verified.** `zmenu` partially documents main-thread installation, but tray/window APIs and mutable global state lack equivalent rules. State the contract for installation, updates, polling, pumping, and teardown; add assertions where practical. (`src/zmenu.zig:6-10`, `src/ztray.zig:19-85`, `src/windows.zig:66-77`, `src/linux_dbus_menu.c:27-40`)

### DVUI and zwindow parity

- [x] **Do not silently discard DVUI shortcut synchronization failures.** Completed in `2e6cd98`: initial shortcut synchronization errors now propagate through `InstallMainMenuError`. (`src/zmenu_with_dvui.zig:43-53`, `src/zmenu.zig:35-44`)
- [x] **Remove or explicitly expose DVUI’s 128-shortcut limit.** Completed in `d576128`: shortcut IDs use allocator-backed storage owned by the menu arena. (`src/zmenu_dvui.zig:16-18`, `src/zmenu_dvui.zig:114-165`)
- [ ] **Resolve zwindow option no-ops.** **Verified.** Windows ignores supplied color, alpha, and dark settings; X11/Wayland accept fields that are currently unused. Implement supported fields, split platform-specific options, or clearly document no-ops. (`src/window_common.zig:39-47`, `src/window_windows.zig:241-257`, `src/window_linux_x11.zig:1-3`, `src/window_linux_wayland.zig:1-7`)
- [ ] **Fix/document Wayland resource lifetime limits.** **Verified.** One display is supported, the registry/dynamic library are process-lifetime, and a 16-entry blur cache deliberately leaks when full. Provide deinitialization and growable storage, or make limits explicit and fail safely. (`src/window_linux_wayland.zig:115-130`, `src/window_linux_wayland.zig:201-208`, `src/window_linux_wayland.zig:249-252`, `src/window_linux_wayland.zig:272-282`)

## P2 — automated verification

- [ ] **Broaden unit coverage.** Action-ID validation unit coverage was added in `6b68084`; add tests for item flattening, 512-item handling, lifecycle/replacement state transitions, and tray/menu error paths. (`src/types.zig:147-218`, `build.zig:356-364`)
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
