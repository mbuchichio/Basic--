# Roadmap Snapshot - 2025-10-17

## Decision

Create an independent repository named `basicpp` (this repo) that contains:

- The header-only runtime consumed by generated code.
- The future Basic++ toolchain (`bppc` transpiler, tests, documentation).

Any host application consumes the runtime as a dependency; end users write `.bpp` without ever touching C++.

## Initial split goals

- Isolate the generic contracts used by generated code (command, state machine, history, result, event bus, DI, etc.).
- Avoid host-specific dependencies (Win32, D2D, ...); stick to the C++ standard library.
- Give the runtime its own SemVer cadence and dedicated tests.
- Keep host integration simple (submodule or package) while the language evolves separately.

## Repository layout

```
basicpp/
  include/basicpp/            # public runtime headers
    core/{result.hpp,event.hpp,service_provider.hpp,clock.hpp,logger.hpp}
    command/{command.hpp,registry.hpp,shortcut_map.hpp}
    state/{state_machine.hpp,state.hpp,transition.hpp}
    history/{delta.hpp,history.hpp,coalescer.hpp,snapshot_fallback.hpp}
    testing/{selftest.hpp,test_registry.hpp}
  src/                        # transpiler CLI and supporting utilities
  tests/                      # runtime unit tests + transpiler integration tests
  docs/                       # contracts, language overview, roadmap
  tools/                      # build/transpilation scripts (to be defined)
  LICENSE, README.md, CHANGELOG.md
```

## Consuming the runtime library (choose one)

1. **Git submodule**
   - `git submodule add https://.../basicpp extern/basicpp`
   - Pros: easy to update. Cons: submodule management overhead.
2. **Vendoring (copying)**
   - Copy `/include/basicpp` into `extern/basicpp/include` (ideally via script).
   - Pros: simple. Cons: manual or scripted updates.
3. **Package (CMake/CPM/vcpkg)** *(future)*
   - Pros: clean consumption. Cons: initial setup effort.

Current recommendation: start with a submodule; publish a package once the transpiler stabilises.

## Design guardrails

- Keep everything under the `basicpp::` namespace; avoid global `using` in headers.
- Target standard C++20; begin life as header-only to reduce friction.
- Define interfaces (for example `ILogger`, `IClock`) and let hosts provide implementations.
- Do not include domain-specific types (Document, Snap, etc.).

## Delivery plan (short sprints)

### Sprint A - runtime bootstrap and CI (0.5-1 day)

- [x] Create repo, layout, MIT license, README describing the language vision.
- [x] Implement `core::result`, `command::registry`, `state::state_machine` (functional stubs).
- [x] Set up minimum CI (GitHub Actions matrix Windows/Linux running tests).
- [x] Write `docs/OVERVIEW.md` with the initial syntax.
- [x] Add the initial `bppc` CLI stub with declared commands (transpile/build/version).
- [x] Capture follow-up steps in README and this planning file.

### Sprint B - host adoption (1-2 days)

- [ ] Replace host-side implementations (`CommandRegistry`, `StateMachine`, etc.) with `basicpp` adapters.
- [ ] Keep the current history component; move `Coalescer` into `basicpp::history` when practical.
- [ ] Ensure `make test-all` (or equivalent) stays green. Tag `v0.1.0` and sync the chosen consumption method.

### Sprint C - transpiler MVP (2-3 days)

- [x] Define the token set and lexer stub (`token.hpp`, `lexer.hpp`/`.cpp`).
- [x] Add lexer tests covering key tokens and common failure modes.
- [x] Introduce a parser skeleton that recognises module declarations.
- [ ] Define the basic grammar (`docs/OVERVIEW.md` <-> parser).
- [ ] Implement lexer integration, parser, and AST under `src/`.
- [ ] Generate C++ for the initial subset (const, command, state, function).
- [ ] Extend `bppc` so it accepts `.bpp` and emits `.cpp` alongside the runtime.

### Sprint D - standalone experience (2-3 days)

- [ ] Extend `bppc` with a `build` subcommand that runs the full pipeline (bpp -> C++ -> compiler -> artefact).
- [ ] Autodetect common toolchains (MSVC, clang, gcc) and surface simplified flags (`--target wasm`, `--target native`).
- [ ] Package runtime + CLI into platform-specific releases.
- [ ] Ship project templates (`bppc init console`, `bppc init gui`) with preconfigured CMake for advanced users.

## CI and versioning

- Target SemVer `v0.1.0` once the first host integrates the runtime.
- Continuous integration: Windows + Linux with warnings treated as errors for headers and full test execution.
- Publish artefacts (header-only zip, `bppc` binaries) once the transpiler is ready.

## Minimum documentation set

- `README.md`: language and runtime vision.
- `docs/OVERVIEW.md`: syntax outline and compilation pipeline.
- `docs/CONTRACTS.md`: runtime interface guarantees.
- `docs/PLANNING.md`: living roadmap (this file).

## Risks and mitigations

- Drift between host projects and Basic++ -> downstream integration tests keyed to `basicpp` tags; maintain an `extern/basicpp.version` file.
- Over-generalising -> keep scope focused on proven patterns.
- Dependency management -> runtime stays header-only; ship the transpiler as a binary or package.
- Generated C++ compilation -> validate against MSVC, clang, gcc, and Emscripten in CI before releases.
