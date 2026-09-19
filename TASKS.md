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
- [x] **Replace or surface the 512-item Linux menu limit.** Completed in `cfb50e7`: Zig flattening prevalidates the 512-item backend limit and returns `MenuTooLarge`. (`src/linux.zig:29-170`, `src/ztray.zig:480-537`)
- [ ] **Run Linux D-Bus functionality on a native runner.** **Verified.** Cross-builds select a no-op D-Bus stub, so they do not test real Linux behavior. Add isolated-session-bus tests on Linux. (`build.zig:44-51`, `src/linux_dbus_stub.c:12-35`)

### Lifecycle and event semantics

- [x] **Define main-menu lifecycle semantics.** Completed in `502a7da`: native menubars are documented as process-lifetime and repeated installation returns `MenuAlreadyInstalled` consistently. A future teardown/replacement API remains an enhancement. (`src/zmenu.zig:39-70`, `src/macos.zig:67-83`, `src/windows.zig:187-211`, `src/linux.zig:112-134`)
- [ ] **Clean up Windows native menubar resources.** Construction-error cleanup completed in `6b9b659`; normal teardown/subclass removal requires a future menubar teardown API. (`src/windows.zig:177-211`)
- [ ] **Make tray menu replacement transactional.** Linux completed in `4f4a2dd` and Windows in `7a2565c`; macOS replacement remains destructive. Build temporary state first and swap only after successful construction. (`src/macos.zig:114-118`, `src/ztray.zig:377-397`, `src/linux_dbus_menu.c:619-667`)
- [x] **Define action-delivery behavior.** Completed in `4d9a337`: public polling APIs document latest-action-wins semantics until polled. (`src/zmenu.zig:82-95`, `src/ztray.zig:69-84`)
- [x] **Validate action IDs uniformly.** Completed in `6b68084`: the public menu/tray boundaries reject negative action IDs before platform dispatch. (`src/types.zig:100-112`, `src/zmenu.zig:58-68`, `src/ztray.zig:60-68`)
- [x] **Publish a complete main-thread/thread-safety contract.** Completed in `165fdf6`: menu and tray API docs require the owning UI thread for installation, updates, polling, pumping, and teardown. Runtime assertions remain a future enhancement. (`src/zmenu.zig:6-12`, `src/ztray.zig:5-12`)

### DVUI and zwindow parity

- [x] **Do not silently discard DVUI shortcut synchronization failures.** Completed in `2e6cd98`: initial shortcut synchronization errors now propagate through `InstallMainMenuError`. (`src/zmenu_with_dvui.zig:43-53`, `src/zmenu.zig:35-44`)
- [x] **Remove or explicitly expose DVUI’s 128-shortcut limit.** Completed in `d576128`: shortcut IDs use allocator-backed storage owned by the menu arena. (`src/zmenu_dvui.zig:16-18`, `src/zmenu_dvui.zig:114-165`)
- [x] **Resolve zwindow option no-ops.** Completed documentation in `54edcd8`: README support matrix identifies platform-specific supported and ignored `FrameChrome` fields. Implementing additional platform fields remains a future enhancement. (`README.md:70-78`)
- [ ] **Fix/document Wayland resource lifetime limits.** **Verified.** One display is supported, the registry/dynamic library are process-lifetime, and a 16-entry blur cache deliberately leaks when full. Provide deinitialization and growable storage, or make limits explicit and fail safely. (`src/window_linux_wayland.zig:115-130`, `src/window_linux_wayland.zig:201-208`, `src/window_linux_wayland.zig:249-252`, `src/window_linux_wayland.zig:272-282`)

## P2 — automated verification

- [ ] **Broaden unit coverage.** Action-ID validation unit coverage was added in `6b68084`; add tests for item flattening, 512-item handling, lifecycle/replacement state transitions, and tray/menu error paths. (`src/types.zig:147-218`, `build.zig:356-364`)
- [ ] **Add Linux D-Bus protocol integration tests.** Exercise registrar detection, StatusNotifier registration, menu layout/property responses, malformed calls, action events, and watcher absence using an isolated session bus.
- [ ] **Add native smoke tests for macOS and Windows.** Cover AppKit/Win32 menu/tray initialization, resource teardown, and basic action dispatch on real runners.
- [ ] **Test DVUI behavior.** Verify shortcut registration, cleanup, fallback mode selection, and capacity/error behavior.
- [ ] **Test zwindow backends.** Add focused platform smoke harnesses for macOS, Windows, X11, and Wayland behavior and degradation paths.
- [ ] **Expand target coverage policy.** **Verified.** Existing compile CI omits explicit macOS architectures and Windows ARM64. Publish a support matrix and test each supported target. (`build.zig:373-385`)
- [ ] **Automate formatting and C/Objective-C analysis.** Zig formatting check completed in `3d73cc6`; add Clang warnings/static analysis and sanitizer-capable test jobs for native sources. (`.github/workflows/ci.yml:32-34`)

## P2 — documentation and developer experience

- [ ] **Expand public platform documentation.** Lifecycle, event loop, thread-affinity, action semantics, Linux caveats, and zwindow support matrix were documented in `54edcd8` and `e5eaa33`; add explicit OS/architecture support policy and ownership details. (`README.md:70-98`)
- [ ] **Document Linux desktop requirements and troubleshooting.** Session D-Bus, AppMenu registrar, icon theme, and DVUI fallback behavior are documented in `e5eaa33`; add StatusNotifier watcher troubleshooting. (`README.md:94-98`)
- [x] **Document lifecycle/error recovery.** Completed in `e5eaa33`: covers initialization constraints, polling/pumping, shutdown, and reinstallation behavior. (`README.md:84-98`)
- [x] **Publish a zwindow support matrix.** Completed in `54edcd8`. (`README.md:70-78`)
- [ ] **Add API documentation for ownership and input limits.** Thread affinity, action IDs, and Linux item maximum are documented in `e5eaa33`; add retained/copied input lifetimes and PNG size limit. (`README.md:84-98`)

## P2 — release, governance, and supply chain

- [ ] **Create an initial release process.** **Verified.** Package version is `0.0.0`; repository has no tags or changelog. Choose versioning policy, create changelog/release notes, tag releases, and document compatibility policy. (`build.zig.zon:2-5`)
- [ ] **Add contribution and security policies.** **Verified.** Add `CONTRIBUTING.md`, `SECURITY.md`, issue/PR templates, maintenance/support policy, and a release checklist.
- [ ] **Review third-party attribution and provenance.** **Verified review task.** Verify licenses and redistribution obligations for vendored `stb_image` and pinned dependencies; publish third-party notices and preferably an SBOM/provenance record. (`vendor/stb_image.h`, `build.zig.zon:6-21`, `LICENSE:1-21`)
- [ ] **Add dependency maintenance controls.** Keep pins, but add automated update review, reproducibility/cache verification, and vulnerability monitoring where applicable.

## P3 — suggested product and maintainability improvements

- [ ] **Consider richer menu items.** **Suggested.** Add nested submenus, checked/radio states, dynamic labels, and item icons to the common model. (`src/types.zig:77-98`)
- [ ] **Consider explicit tray click events.** **Suggested.** Expose click/double-click callbacks or polling events in addition to context-menu actions. (`src/windows.zig:27-36`, `src/windows.zig:434-457`)
- [x] **Add PNG resource limits.** Completed in `db8c8c2`: Windows rejects PNGs exceeding 1024×1024 pixels or 4 MiB decoded RGBA data before allocation. (`src/windows_tray_png.c:11-24`)
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
