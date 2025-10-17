# Basic++ Language Overview

This document sketches the initial surface of the Basic++ (`.bpp`) language. It is intentionally lightweight so the parser and transpiler can iterate quickly. Expect revisions as the toolchain evolves.

## Design goals

- Preserve the readability and low barrier of classic BASIC.
- Offer first-class constructs for patterns we repeatedly implement in C++ (commands, state machines, undo history).
- Generate deterministic, idiomatic C++ without asking the user to touch C++ sources.
- Keep semantics close to the runtime contracts documented in `CONTRACTS.md`.

## Source structure

A Basic++ project is a collection of modules (`.bpp` files). Each module can declare:

```
module Main

import System.Timer
import Basicpp.Command

const Version = "0.1.0"

state AppState = Idle
    on Start => Running
    on Stop  => Idle

command SayHello(context)
    print "Hello, " & context.name
end command

function Main()
    registrar = CommandRegistry()
    registrar.register("hello", SayHello)
    registrar.dispatch("hello", { name = "world" })
end function
```

- `module` names the translation unit; transpiler emits a matching C++ namespace.
- `import` resolves to runtime headers and generated support code.
- `state` declares a state machine; the compiler wires it to `basicpp::state::state_machine`.
- `command` declares a callable command mapped to `basicpp::command::registry` entries.
- `function Main()` is the entry point; the transpiler wraps it into `int main()` for native builds.

## Types

Primitive types map to standard C++ types:

| Basic++ | C++           |
|---------|---------------|
| `Integer` | `std::int64_t` |
| `Float`   | `double`       |
| `String`  | `std::string`  |
| `Boolean` | `bool`         |

Structured literals use braces:

```
let user = { name = "Ada", age = 37 }
let scores = [10, 20, 30]
```

Records become lightweight structs; arrays become `std::vector`.

## Control flow

```
if condition then
    ...
elseif other then
    ...
else
    ...
end if

for i in 0..10
    print i
end for

while pending
    handle()
end while
```

Loops and conditionals lower to straightforward C++ constructs. Range-based `for` desugars into index loops.

## Error handling

Basic++ favors explicit results over exceptions. Functions returning `Result<T>` compile to `basicpp::core::result<T, String>`:

```
function LoadConfig(path) as Result<Config>
    if not FileExists(path) then
        return Error("Config missing")
    end if
    return Ok(ParseConfig(path))
end function
```

In generated C++, `Error` and `Ok` map to `core::result::err` and `core::result::ok`.

## Commands and state machines

```
state DocumentState = Empty
    on TypeEvent => Dirty
    on SaveEvent => Clean

command Save(doc)
    if doc.state = Dirty then
        persist(doc)
        doc.state = Clean
    end if
end command
```

The transpiler emits transition tables and registry wiring using `basicpp::state` and `basicpp::command` primitives.

## Build pipeline (planned)

1. Parse `.bpp` modules into an AST.
2. Perform semantic checks (type inference, command/state wiring).
3. Emit C++ translation units under `generated/`.
4. Link the generated units with the runtime headers.
5. Invoke the target compiler (native toolchain or `wasm32-unknown`) to produce binaries.

Until steps 1–3 land, you can experiment directly with the runtime library and tests in this repository.
