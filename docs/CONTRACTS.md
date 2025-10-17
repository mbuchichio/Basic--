# Contracts

This document captures the guarantees offered by the public `basicpp` interfaces. It will evolve alongside the library; keep it succinct and focused on observable behaviour.

## core::result

- Models a success-or-error outcome without exceptions.
- `has_value()` returns true if and only if `value()` is accessible.
- `value()` and `error()` throw `std::logic_error` when accessed in the wrong state.
- `value_or(fallback)` returns either the stored value or the provided fallback, without modifying internal state.

## command::registry

- Handlers are registered by key; registering the same key twice overwrites the previous handler.
- `dispatch` returns `core::result<TResult, std::string>` containing either the handler result or a textual error describing the lookup failure or handler failure.

## state::state_machine

- Deterministic transitions: at most one transition per (state, event) pair.
- `dispatch` without a matching transition returns an error describing the failure and leaves the current state unchanged.
- Transition callbacks, when configured, run after the state has been updated.

## history::coalescer

- Aggregates updates inside a specified time window using a caller-provided combine function.
- `consume(now)` emits a value only after the window elapses or immediately if forced via `consume(now, true)` (to be added).
- Resetting clears pending data without triggering the combine function.

## testing::selftest

- Provides a minimal registry-based harness. Each `BASICPP_TEST` body runs independently.
- Test registration is static; duplicate names overwrite the previous definition.
- The default main exits with non-zero when any test raises an exception.
