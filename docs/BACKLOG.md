# Basic++ Backlog Snapshot (2025-10-18)

This backlog captures ideas that surfaced during the 2025-10-18 brainstorming. It intentionally casts a wide net; treat it as a parking lot rather than a committed roadmap. Items closer to the top are more aligned with the current repo. Everything is written in English to match the rest of the documentation.

---

## 1. Near-term (feeds current Sprint C/D work)

- Lower generated command/function bodies to real runtime-backed C++ (replace the current TODO stubs and wire in statements, results, and errors).
- Extend `bppc` with a `build` pipeline (transpile → compile) and multi-module support, including compiler discovery and output structure.
- Decide how the runtime headers are distributed to downstream projects (submodule, package, vendoring script) and document the chosen path.
- Flesh out module imports (`USE`, namespace aliasing) and ensure the parser/AST can support namespaced lookups.
- Sketch governance for feature flags / optional modules so the core stays tight while allowing experiments.

## 2. Mid-term language/runtime themes

- Enrich the language surface: hierarchical namespaces (`Alpha.Beta`), `USE`/`ALIAS`, `FOR EACH`, query-style expressions (`SELECT/WHERE`), and event constructs.
- Integrate higher-level patterns: command history/undo-redo, observer/event bus, finite state machines, strategy swapping, iterator/builder helpers.
- Plan for FFI blocks (`LIB ... END LIB`) so generated C++ can bind to external libraries safely.
- Explore a structured debug story (`#line` mapping, error surfacing, diagnostics in `bppc`).

## 3. Data engine concept (future investigation)

- Prototype an in-memory SQLite-backed model (`obj`, `prop`, `rel`, delta/undo tables) to represent documents, scenes, or domain graphs.
- Validate how Basic++ syntax would express CRUD over that model (`Db.Obj.New`, `Db.Prop.Set`, `Query.Frames().Where(...)`).
- Investigate FTS5/RTREE usage for text and geometry, plus snapshotting via `ATTACH` for persistence.

## 4. Audio runtime ideas (future)

- Evaluate miniaudio/BASS as backends; define `IAudioBackend`, node graphs, and RT schedulers (sample-accurate events, lock-free messaging).
- Draft Basic++ surface APIs for audio graph creation and scheduling (`Audio.Node.Osc`, `RT.At`).
- Map out FFI integration for existing audio SDKs.

## 5. Graphics & UI (future exploratory)

- Decide on the portable renderer baseline (SDL2) and what “WindowsPro” (Direct2D/DirectWrite) brings for typography-heavy scenarios.
- Specify `IRenderer2D`/`IText` abstraction layers and quality modes.
- Consider optional GPU filters (OpenGL/Vulkan compute) for advanced workflows.

## 6. GPU as coprocessor (future)

- Research compute backends (D3D11, OpenGL) and how they could serve filters/FFT/pixel operations.
- Define API shape (`Gpu.Dispatch`, `Gpu.Filter.*`) and scheduling primitives.

## 7. Armador integration ideas

- Outline an `IScriptHost` seam so Armador (C++) can hand commands/data to Basic++ for evaluation, returning deltas.
- Stage integration: Armador MVP → scripting stub → Basic++ evaluation → migrate automation/logic incrementally.

## 8. Tooling & platform support

- Incremental builds and caching for transpiled `.cpp` artefacts.
- Semantic linter (`Reflect.*`), analyzer suggestions, and perf diagnostics (fps, frame timing).
- IDE experience: VS Code extension with snippets, commands, hover info.
- Project templates (`bppc init app2d`, `init audio-demo`, etc.).

## 9. Guiding principles (keep referencing)

- “Blocks as folders”: hierarchical, composable namespaces.
- Clean transpilation to C++ with zero runtime penalty.
- SQLite (or similar) as the canonical semantic store for undo/history/relations.
- GPU/Audio as opt-in coprocessors rather than heavy frameworks.
- Armador remains the host shell; Basic++ supplies the programmable layer.
- Pace sustainably: small core first, feature expansion later, documentation always current.

---

> Tip: When promoting items from this backlog into the main `docs/PLANNING.md`, trim the scope and anchor them to concrete sprints. Everything else can remain here as ideation until it is ready to graduate.