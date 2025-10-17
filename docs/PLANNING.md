# Roadmap Snapshot — 2025-10-17

## Decisión

Crear **repo independiente** `basicpp` (este repositorio) que aloja:

- El **runtime header-only** consumido por el código generado.
- El futuro **toolchain del lenguaje Basic++** (`bppc` transpiler, pruebas y docs).

Armador y otros hosts consumirán la librería como dependencia; el usuario final trabaja en `.bpp` sin tocar C++.

## Objetivos del split inicial

- **Aislar contratos genéricos** compartidos por el código generado (Command, StateMachine, History, Result, EventBus, DI, etc.).
- **Cero dependencias** de Armador/Win32/D2D: solo estándar C++.
- **SemVer propio** y **tests dedicados** para el runtime.
- Integración sencilla en Armador (submódulo o paquete); evolución del lenguaje en repositorio separado.

## Layout del repo

```
basicpp/
  include/basicpp/            # headers públicos (runtime)
    core/{result.hpp,event.hpp,service_provider.hpp,clock.hpp,logger.hpp}
    command/{command.hpp,registry.hpp,shortcut_map.hpp}
    state/{state_machine.hpp,state.hpp,transition.hpp}
    history/{delta.hpp,history.hpp,coalescer.hpp,snapshot_fallback.hpp}
    testing/{selftest.hpp,test_registry.hpp}
  src/                        # transpiler CLI y utilidades auxiliares
  tests/                      # unit tests runtime + integration tests transpiler
  docs/                       # contratos, overview del lenguaje, roadmap
  tools/                      # scripts de build/transpilado (por definir)
  LICENSE, README.md, CHANGELOG.md
```

## Integración en Armador (elige 1)

1. **Submódulo Git**
   - `git submodule add https://…/basicpp extern/basicpp`
   - Pros: actualizable; Contras: gestión de submódulos.
2. **Vendoring (copiado)**
   - Copiar `/include/basicpp` a `extern/basicpp/include` (con script).
   - Pros: simple; Contras: actualizar manual/script.
3. **Paquete (CMake/CPM/vcpkg)** *(más adelante)*
   - Pros: clean; Contras: setup inicial.

Sugerencia actual: **submódulo**; cuando el transpiler esté estable, empaquetar.

## Guardrails de diseño

- Namespace **`basicpp::`**; sin `using` globales en headers.
- Solo C++20 estándar; comenzar **header-only** para reducir fricción.
- Definir interfaces (`ILogger`, `IClock`) y delegar implementaciones al host.
- **Sin tipos de dominio** (Document, Snap, etc.).

## Plan de migración (dos sprints cortos)

### Sprint A — bootstrap runtime & CI (½–1 día)

- Crear repo, layout, LICENSE MIT, README con visión del lenguaje.
- Implementar `core::result`, `command::registry`, `state::state_machine` (stubs funcionales).
- Tests mínimos que pasen en CI (GitHub Actions matrix {Win, Linux}).
- Documento `docs/OVERVIEW.md` describiendo la sintaxis inicial.

### Sprint B — adopción en Armador (1–2 días)

- Reemplazar `CommandRegistry` y `StateMachine` internos por `basicpp` (adaptadores finos).
- Mantener `History` actual; portar `Coalescer` a `basicpp::history` si es simple.
- `make test-all` verde. Etiquetar release `v0.1.0` y sincronizar submódulo.

### Sprint C — transpiler MVP (2–3 días)

- Definir gramática básica (`docs/OVERVIEW.md` ⇄ parser).
- Implementar lexer + parser + AST en `src/`.
- Generar C++ para subset (const, command, state, function).
- CLI `bppc` que reciba `.bpp` y escupa `.cpp` + dependa de runtime.

## CI y versionado

- **SemVer**: `v0.1.0` al integrar en Armador.
- CI: Windows + Linux, `-Werror` para headers y ejecución de tests.
- Publicar artefactos (zip header-only, binarios `bppc`) cuando el transpiler esté listo.

## Documentación mínima

- `README.md`: visión del lenguaje y runtime.
- `docs/OVERVIEW.md`: sintaxis y pipeline de compilación.
- `docs/CONTRACTS.md`: garantías de las interfaces runtime.
- `docs/PLANNING.md`: roadmap vivo (este archivo).

## Riesgos y mitigaciones

- **Drift** entre Armador y Basic++ → tests de integración en Armador usando tags de `basicpp`. Mantener archivo `extern/basicpp.version`.
- **Sobregeneralizar** → Mantener scope acotado; sólo patrones probados.
- **Gestión de dependencias** → Runtime header-only; transpiler se entrega como binario o paquete.
- **Compilación C++ generada** → Validar output contra compiladores targets (MSVC, clang, gcc, Emscripten) en CI antes de releases.
