# Release checklist

1. Update `build.zig.zon` version using semantic versioning.
2. Move unreleased changelog entries to a dated release section.
3. Run `zig fetch`, `zig fmt build.zig src examples`, `zig build test`, and `zig build ci`.
4. Verify native CI passes on Linux, Windows, and macOS.
5. Verify a clean consumer can fetch and build the package.
6. Review third-party notices and dependency pins.
7. Create an annotated version tag and GitHub release with changelog notes.
