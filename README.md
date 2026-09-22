# FrameWorks

FrameWorks is a cross-platform graphics and vector design application built
with a C++ engine and a Qt Quick interface.

The project is at an early foundation stage. The current executable provides a
native workspace, C++ scene-graph canvas, editable layers, selection,
properties, duplication, deletion, visibility, and undo/redo. It is not yet a
complete professional design suite.

## Targets

- macOS
- Windows
- Linux
- ChromeOS through Qt for WebAssembly

## Requirements

- CMake 3.25 or newer
- A C++23 compiler
- Qt 6.8 or newer with Quick and QML

## Build

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Browser build

The browser foundation uses Zig to compile its C++ engine surface to
WebAssembly without Emscripten. This probe is the first extraction point for
moving the native document model into a shared platform-neutral core:

```sh
./web/build.sh build-web
python3 -m http.server 8000 --directory build-web
```

Open `http://127.0.0.1:8000` in a browser. The page reports when the
WebAssembly engine has loaded and exposes working blank/recent document state.

## Architecture

- `src/` contains the C++ document model and scene-graph canvas.
- `ui/` contains the shared Qt Quick interface.
- `tests/` contains native model tests.

The QML layer owns presentation and interaction composition. C++ owns document
state, editing operations, history, file formats, rendering, and collaboration
state. Platform-specific filesystem and window services remain behind narrow
interfaces so the same interface can run as a native application or WebAssembly
build.

## License

MIT
