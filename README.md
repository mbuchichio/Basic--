# Basic++ (bpp)

Basic++ is a modern BASIC-flavoured language that transpiles to C++ (and, via your toolchain, to native code or WebAssembly). The language focuses on expressive sugar and encapsulated patterns so that developers can stay in BASIC syntax while targeting high-performance C++ runtimes.

This repository contains two pieces:

- **Transpiler toolchain** (WIP): converts `.bpp` sources into clean C++ that you can feed to your regular compiler.
- **Runtime library (`include/basicpp/...`)**: a lightweight, header-only standard library offering the core building blocks required by generated code (command dispatch, state machines, history helpers, etc.).

The goal is to offer a modern BASIC that emits idiomatic C++, letting developers produce portable artefacts without writing C++ by hand.

## Current status

- Runtime layer bootstrap covering `basicpp::core`, `basicpp::command`, `basicpp::state`, `basicpp::history`, and `basicpp::testing`.
- Minimal self-test harness (see `tests/`) plus CLI integration coverage to keep behaviour stable while the language front-end evolves.
- CLI `bppc` accepts `transpile <file.bpp>` and parses module headers, imports, constants, state machines, command blocks, and function blocks. It now writes the generated `.cpp` beside the input (override with `--out`) and can dump the lexer stream via `--tokens` for debugging.
- GitHub Actions runs `cmake` + `ctest` on Ubuntu and Windows for every push and pull request.

Work in progress:

1. Lower command/function bodies to real runtime-backed C++ instead of stubbed TODOs.
2. Flesh out the CLI “build” pipeline (bpp -> C++ -> compiler) and multi-file support.
3. Package the runtime + CLI so downstream projects can depend on Basic++ without cloning this repository.

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
