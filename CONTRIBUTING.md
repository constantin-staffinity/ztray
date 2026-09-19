# Contributing

## Prerequisites

- Zig 0.16.0 or newer
- On Linux, `libdbus-1-dev` for the native D-Bus backend

## Before opening a pull request

Run:

```sh
zig fetch
zig fmt build.zig src examples
zig build test
zig build ci
```

Keep changes scoped, include tests for behavior changes where practical, and update `README.md` or `TASKS.md` when public behavior or backlog status changes.

## Platform changes

Native code must preserve behavior on macOS, Windows, and Linux. Linux D-Bus changes should be exercised on a native Linux session bus; macOS and Windows integrations should be smoke-tested on their native platforms.
