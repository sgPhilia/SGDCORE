# Changelog

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