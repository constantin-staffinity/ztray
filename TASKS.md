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
- [x] **Support supplied PNG tray icons on Linux.** Completed in `427769a`: PNGs are decoded to ARGB, published through StatusNotifier `IconPixmap`, and announce `NewIcon`; `linux_icon_name` remains supported for Freedesktop theme lookup. Native D-Bus protocol coverage remains tracked below. (`src/linux_tray_png.c`, `src/linux_dbus_menu.c:254-324`, `src/linux_dbus_menu.c:695-745`)
- [x] **Propagate menu shortcut metadata over D-BusMenu.** Completed in `6b5829d`: Linux publishes the D-BusMenu `shortcut` (`aas`) property; `primary` and `ctrl` map to `Control`, with Shift, Alt, and Super retained. This is menu display metadata, not a global shortcut registration. (`src/linux.zig:43-124`, `src/linux_dbus_menu.c:99-147`)
- [x] **Replace or surface the 512-item Linux menu limit.** Completed in `cfb50e7`: Zig flattening prevalidates the 512-item backend limit and returns `MenuTooLarge`. (`src/linux.zig:29-170`, `src/ztray.zig:480-537`)
- [x] **Run Linux D-Bus functionality on a native runner.** Completed in `1813991`: CI builds the real Linux backend inside `dbus-run-session`, in addition to cross-builds that use the stub. Protocol-level assertions remain tracked below. (`.github/workflows/ci.yml:25-47`, `build.zig:44-52`)

### Lifecycle and event semantics

- [x] **Define main-menu lifecycle semantics.** Completed in `502a7da`: native menubars are documented as process-lifetime and repeated installation returns `MenuAlreadyInstalled` consistently. A future teardown/replacement API remains an enhancement. (`src/zmenu.zig:39-70`, `src/macos.zig:67-83`, `src/windows.zig:187-211`, `src/linux.zig:112-134`)
- [x] **Clean up Windows native menubar resources.** Construction-error cleanup completed in `6b9b659`; `1b0bcb7` destroys the tracked native menu and resets menu state when the owner window is destroyed. (`src/windows.zig:187-210`, `src/windows.zig:478-510`)
- [x] **Make tray menu replacement transactional.** Completed across Linux (`4f4a2dd`), Windows (`7a2565c`), and macOS (`3f0bb2a`): a new native menu is fully built before replacing the active menu. (`src/macos_tray.m:90-130`, `src/ztray.zig:141-168`, `src/linux_dbus_menu.c:665-711`)
- [x] **Define action-delivery behavior.** Completed in `4d9a337`: public polling APIs document latest-action-wins semantics until polled. (`src/zmenu.zig:82-95`, `src/ztray.zig:69-84`)
- [x] **Validate action IDs uniformly.** Completed in `6b68084`: the public menu/tray boundaries reject negative action IDs before platform dispatch. (`src/types.zig:100-112`, `src/zmenu.zig:58-68`, `src/ztray.zig:60-68`)
- [x] **Publish a complete main-thread/thread-safety contract.** Completed in `165fdf6`: menu and tray API docs require the owning UI thread for installation, updates, polling, pumping, and teardown. Runtime assertions remain a future enhancement. (`src/zmenu.zig:6-12`, `src/ztray.zig:5-12`)

### DVUI and zwindow parity

- [x] **Do not silently discard DVUI shortcut synchronization failures.** Completed in `2e6cd98`: initial shortcut synchronization errors now propagate through `InstallMainMenuError`. (`src/zmenu_with_dvui.zig:43-53`, `src/zmenu.zig:35-44`)
- [x] **Remove or explicitly expose DVUI’s 128-shortcut limit.** Completed in `d576128`: shortcut IDs use allocator-backed storage owned by the menu arena. (`src/zmenu_dvui.zig:16-18`, `src/zmenu_dvui.zig:114-165`)
- [x] **Resolve zwindow option no-ops.** Completed documentation in `54edcd8`: README support matrix identifies platform-specific supported and ignored `FrameChrome` fields. Implementing additional platform fields remains a future enhancement. (`README.md:70-78`)
- [x] **Fix/document Wayland resource lifetime limits.** Completed in `170eb40`: blur-cache overflow releases its proxy and disables blur for that surface instead of leaking. The 16-surface cap and single-display/process-lifetime backend model are documented. (`src/window_linux_wayland.zig:115-130`, `src/window_linux_wayland.zig:272-346`, `README.md:70-78`)

## P2 — automated verification

- [ ] **Broaden unit coverage.** Action-ID validation unit coverage was added in `6b68084`; add tests for item flattening, 512-item handling, lifecycle/replacement state transitions, and tray/menu error paths. (`src/types.zig:147-218`, `build.zig:356-364`)
- [ ] **Add Linux D-Bus protocol integration tests.** The real backend is built under an isolated session bus in `1813991`; add protocol assertions for registrar detection, StatusNotifier registration, menu layout/property responses, malformed calls, action events, and watcher absence.
- [ ] **Add native smoke tests for macOS and Windows.** Cover AppKit/Win32 menu/tray initialization, resource teardown, and basic action dispatch on real runners.
- [ ] **Test DVUI behavior.** Verify shortcut registration, cleanup, fallback mode selection, and capacity/error behavior.
- [ ] **Test zwindow backends.** Add focused platform smoke harnesses for macOS, Windows, X11, and Wayland behavior and degradation paths.
- [x] **Expand target coverage policy.** Completed in `f22379a` and `e56072b`: compile CI covers Linux x86_64/aarch64 and Windows x86_64/ARM64; native CI covers current macOS, Windows, and Linux runners. (`build.zig:374-382`, `README.md:102-106`)
- [ ] **Automate formatting and C/Objective-C analysis.** Zig formatting check completed in `3d73cc6`; add Clang warnings/static analysis and sanitizer-capable test jobs for native sources. (`.github/workflows/ci.yml:32-34`)

## P2 — documentation and developer experience

- [ ] **Expand public platform documentation.** Lifecycle, event loop, thread-affinity, action semantics, Linux caveats, and zwindow support matrix were documented in `54edcd8` and `e5eaa33`; add explicit OS/architecture support policy and ownership details. (`README.md:70-98`)
- [x] **Document Linux desktop requirements and troubleshooting.** Completed in `e5eaa33` and `6080a54`: session D-Bus, AppMenu registrar, StatusNotifier watcher, icon theme, and DVUI fallback behavior are documented. (`README.md:96-100`)
- [x] **Document lifecycle/error recovery.** Completed in `e5eaa33`: covers initialization constraints, polling/pumping, shutdown, and reinstallation behavior. (`README.md:84-98`)
- [x] **Publish a zwindow support matrix.** Completed in `54edcd8`. (`README.md:70-78`)
- [ ] **Add API documentation for ownership and input limits.** Thread affinity, action IDs, and Linux item maximum are documented in `e5eaa33`; add retained/copied input lifetimes and PNG size limit. (`README.md:84-98`)

## P2 — release, governance, and supply chain

- [ ] **Create an initial release process.** Changelog and release checklist completed in `254b422`; choose an initial package version, publish compatibility policy, and create annotated release tags. (`CHANGELOG.md`, `RELEASE.md`, `build.zig.zon:2-5`)
- [x] **Add contribution and security policies.** Completed in `8e5fdab`, `450a48f`, `dfc41b6`, and `77a7161`: contribution/security policies, bug/PR templates, and maintenance support policy are present. (`CONTRIBUTING.md`, `SECURITY.md`, `SUPPORT.md`, `.github/ISSUE_TEMPLATE/bug_report.md`, `.github/pull_request_template.md`)
- [ ] **Review third-party attribution and provenance.** Third-party notices completed in `44e280a`; maintain the release-time license/dependency review and add SBOM/provenance generation. (`THIRD_PARTY_NOTICES.md`, `build.zig.zon:6-21`, `LICENSE:1-21`)
- [ ] **Add dependency maintenance controls.** GitHub Actions update review completed in `745ceed`; retain commit pins and add reproducibility/cache verification plus vulnerability monitoring where applicable. (`.github/dependabot.yml`, `build.zig.zon:6-21`)

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
