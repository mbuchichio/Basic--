# Basic++ (bpp)

Basic++ is a modern BASIC-flavoured language that transpiles to C++ (and, via your toolchain, to native code or WebAssembly). The language focuses on expressive sugar and encapsulated patterns so that developers can stay in BASIC syntax while targeting high-performance C++ runtimes.

This repository contains two pieces:

- **Transpiler toolchain** (WIP): converts `.bpp` sources into clean C++ that you can feed to your regular compiler.
- **Runtime library (`include/basicpp/...`)**: a lightweight, header-only standard library offering the core building blocks required by generated code (command dispatch, state machines, history helpers, etc.).

The goal is to offer a modern BASIC that emits idiomatic C++, letting developers produce portable artefacts without writing C++ by hand.

## Current status

- Runtime layer bootstrap covering `basicpp::core`, `basicpp::command`, `basicpp::state`, `basicpp::history`, and `basicpp::testing`.
- Minimal self-test harness with smoke tests to keep behaviour stable while the language front-end evolves.
- CLI `bppc` accepts `transpile <file.bpp>` and currently parses the module header (pass `--tokens` to dump lexer output for debugging).
- GitHub Actions runs `cmake` + `ctest` on Ubuntu and Windows for every push and pull request.

Work in progress:

1. Parser and AST for `.bpp` syntax.
2. Code generator that lowers AST nodes into the runtime primitives.
3. CLI transpiler pipeline (bpp -> C++ -> chosen compiler -> binary/Wasm).
4. Standalone build tooling so users can produce `.exe`/`.dll` artefacts without touching raw C++ toolchains manually.

## Building the runtime tests

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

Adjust `CMAKE_CXX_STANDARD` if you need a newer language level; the default is C++20.

On Windows with MinGW installed, you can run the bundled helper instead:

```bat
make.bat
```

## Embedding the runtime in other projects

Until the transpiler emits artefacts automatically, you can consume the runtime library directly:

```cmake
add_subdirectory(basicpp)
target_link_libraries(my_target PRIVATE basicpp)
```

Because the runtime is header-only, vendoring the `include/basicpp` directory is also an option for experimental builds.

## Standalone usage (planned)

The long-term experience for Basic++ is:

- Ship a cross-platform CLI (`bppc`) that bundles a minimal C++ toolchain or auto-discovers system compilers.
- Allow `bppc build game.bpp` to transpile, compile, and emit native executables or shared libraries in a single step.
- Provide templates (`bppc init console`, `bppc init wasm`) with sensible defaults so projects start compiling out-of-the-box.
- Offer prebuilt binaries of the CLI with embedded runtime headers, so users can adopt the language without cloning this repository.

Until that tooling is ready, the runtime and docs here help explore the language design while the standalone workflow takes shape.

## License

Basic++ is released under the MIT License. See `LICENSE` for details.
