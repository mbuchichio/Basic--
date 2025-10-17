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

## Grammar (draft)

The following EBNF-style grammar captures the subset we are targeting for the transpiler MVP. Newlines can be written as physical line breaks or semicolons. Indentation in the examples above is optional and serves readability only.

```
module            ::= "module" identifier EOL import_section? declaration_section?

import_section    ::= import_decl+
import_decl       ::= "import" dotted_identifier EOL

declaration_section ::= declaration+
declaration          ::= const_decl | state_decl | command_decl | function_decl

const_decl        ::= "const" identifier "=" expression EOL

state_decl        ::= "state" identifier "=" identifier EOL transition+
transition        ::= "on" identifier "=>" identifier EOL

command_decl      ::= "command" identifier parameter_list EOL block "end" "command" EOL

function_decl     ::= "function" identifier parameter_list return_clause? EOL block "end" "function" EOL

parameter_list    ::= "(" parameter_list_contents? ")"
parameter_list_contents ::= parameter ("," parameter)*
parameter         ::= identifier

return_clause     ::= "as" type_spec

block             ::= statement*

statement         ::= assignment | if_stmt | while_stmt | for_stmt
                    | command_call | return_stmt | expression_stmt

assignment        ::= identifier "=" expression EOL
if_stmt           ::= "if" expression "then" EOL block elseif_clause* else_clause? "end" "if" EOL
elseif_clause     ::= "elseif" expression "then" EOL block
else_clause       ::= "else" EOL block
while_stmt        ::= "while" expression EOL block "end" "while" EOL
for_stmt          ::= "for" identifier "in" range_expression EOL block "end" "for" EOL
command_call      ::= "call" identifier argument_list EOL
return_stmt       ::= "return" expression? EOL
expression_stmt   ::= expression EOL

argument_list     ::= "(" argument_list_contents? ")"
argument_list_contents ::= expression ("," expression)*

range_expression  ::= expression ".." expression

expression        ::= equality
equality          ::= comparison (("==" | "<>") comparison)*
comparison        ::= term (("<" | "<=" | ">" | ">=") term)*
term              ::= factor (("+" | "-") factor)*
factor            ::= unary (("*" | "/") unary)*
unary             ::= ("-" | "not") unary | primary
primary           ::= literal | identifier | call_expression | "(" expression ")"

call_expression   ::= identifier argument_list

literal           ::= integer_literal | float_literal | string_literal | "true" | "false"

dotted_identifier ::= identifier ("." identifier)*
type_spec         ::= dotted_identifier
```

The grammar is intentionally conservative. As the parser grows we can add constructs (pattern matching, user-defined records, richer expressions) and update this section to mirror reality.

## Build pipeline (planned)

1. Parse `.bpp` modules into an AST.
2. Perform semantic checks (type inference, command/state wiring).
3. Emit C++ translation units under `generated/`.
4. Link the generated units with the runtime headers.
5. Invoke the target compiler (native toolchain or `wasm32-unknown`) to produce binaries.

Until steps 1-3 land, you can experiment directly with the runtime library and tests in this repository.
