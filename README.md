# Basic++ (bpp)

Basic++ is a modern BASIC-flavoured language that transpiles to C++ (and, via your toolchain, to native code or WebAssembly). The language focuses on expressive sugar and encapsulated patterns so that developers can stay in BASIC syntax while targeting high-performance C++ runtimes.

This repository contains two pieces:

- **Transpiler toolchain** (WIP): converts `.bpp` sources into clean C++ that you can feed to your regular compiler.
- **Runtime library (`include/basicpp/…`)**: a lightweight, header-only standard library offering the core building blocks required by generated code (command dispatch, state machines, history helpers, etc.).

Think "Xojo, but generating idiomatic C++"—the goal is to ship portable artefacts without ever touching C++ manually.

## Current status

- Bootstrap of the runtime layer (`basicpp::core`, `basicpp::command`, `basicpp::state`, `basicpp::history`, `basicpp::testing`).
- Minimal self-test harness and smoke tests to keep behaviour stable while the language front-end evolves.
- CMake build configured for header-only consumption and test execution.

Work in progress:

1. Parser and AST for `.bpp` syntax.
2. Code generator that lowers AST nodes into the runtime primitives.
3. CLI transpiler pipeline (bpp → C++ → chosen compiler → binary/Wasm).

## Building the runtime tests

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

Adjust `CMAKE_CXX_STANDARD` if you need a newer language level; the default is C++20.

## Embedding the runtime in other projects

Until the transpiler emits artefacts automatically, you can consume the runtime library directly:

```cmake
add_subdirectory(basicpp)
target_link_libraries(my_target PRIVATE basicpp)
```

Because the runtime is header-only, vendoring the `include/basicpp` directory is also an option for experimental builds.

## License

Basic++ is released under the MIT License. See `LICENSE` for details.
