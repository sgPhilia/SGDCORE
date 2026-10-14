# Changelog

## [Unreleased]

### Added
- Finite-difference comparison and gradient-descent examples.
- Arithmetic and power overloads for numeric literals on either side of a
  variable.
- Public API reference with scalar, lifetime, error, and numerical contracts.
- CMake install/export support with an `SGDCORE::sgdcore` package target.
- Ancestor-aware backward traversal so reachable invalid derivatives are not
  masked by zero adjoints, while unrelated branches remain isolated.

### Changed
- The default CMake build now builds only the core library; tests, examples,
  and benchmarks are opt-in.
- `Tape` is non-copyable and non-movable, `Tape::node()` returns a checked copy,
  and `set_value()` rejects variables already referenced by recorded operations.
- Test and benchmark dependencies now use pinned upstream commits.
- Negative-input logarithm gradients now return NaN instead of a finite value
  outside the real logarithm domain.
- `Var<T>` now rejects non-arithmetic scalar types at compile time.

## [0.1.0] - 2026-01-15

### Added
- Tape-based reverse-mode automatic differentiation core.
- `Tape`, `Node`, `NodeId`, `OpCode`, and `Var<T>` abstractions.
- Arithmetic operators: `+`, `-`, `*`, `/`, unary `-`.
- Elementary functions: `sin`, `cos`, `tan`, `exp`, `log`, `sqrt`, `pow`.
- `backward()` and `gradient()` API.
- Finite-difference verification utilities.
- GoogleTest unit tests and Google Benchmark benchmarks.
- Example programs and design documentation.
- MIT license and GitHub Actions CI.
