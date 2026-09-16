# SGDCORE

### C++20 Reverse-Mode Automatic Differentiation Core

[![CI](https://github.com/sgPhilia/SGDCORE/actions/workflows/ci.yml/badge.svg)](https://github.com/sgPhilia/SGDCORE/actions/workflows/ci.yml)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/std/the-standard)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

SGDCORE is a small, explicit-tape C++20 library for scalar reverse-mode
automatic differentiation. It records arithmetic and mathematical operations
into a `Tape`, then traverses the recorded graph backward to compute exact
gradients.

The project is intentionally minimal: no global tape, no virtual dispatch per
operation, no tensor framework, and no external runtime dependencies for the
core library. It is designed as a transparent foundation for learning,
experimenting with, and extending automatic differentiation in modern C++.

## Table of Contents

- [Overview](#overview)
- [Why SGDCORE?](#why-sgdcore)
- [Design Goals](#design-goals)
- [Key Features](#key-features)
- [Current Status](#current-status)
- [Quick Start](#quick-start)
- [Basic Usage](#basic-usage)
- [How Automatic Differentiation Works](#how-automatic-differentiation-works)
- [Computational Graph](#computational-graph)
- [Reverse-Mode Differentiation](#reverse-mode-differentiation)
- [Tape](#tape)
- [Variables](#variables)
- [Operations](#operations)
- [Supported Operations](#supported-operations)
- [API Overview](#api-overview)
- [Building from Source](#building-from-source)
- [Running Tests](#running-tests)
- [Running Verification](#running-verification)
- [Running Benchmarks](#running-benchmarks)
- [Examples](#examples)
- [Project Structure](#project-structure)
- [Architecture](#architecture)
- [Design Decisions](#design-decisions)
- [Memory and Lifetime](#memory-and-lifetime)
- [Correctness and Numerical Verification](#correctness-and-numerical-verification)
- [Performance Considerations](#performance-considerations)
- [Limitations](#limitations)
- [Non-Goals](#non-goals)
- [Roadmap](#roadmap)
- [Development Guide](#development-guide)
- [Adding a New Differentiable Operation](#adding-a-new-differentiable-operation)
- [FAQ](#faq)
- [License](#license)

## Overview

Automatic differentiation (AD) is the process of evaluating the derivative of
a function specified by a computer program. Unlike numerical differentiation,
which approximates derivatives, AD applies the chain rule systematically
through the sequence of elementary operations performed by the program.
The result is an exact derivative up to floating-point round-off.

SGDCORE implements **reverse-mode automatic differentiation**. In reverse
mode, the program first evaluates the function and records the computational
graph, then propagates derivative information backward from the output to the
inputs. This is particularly efficient when there is one scalar output and
many inputs, because a single backward pass computes all input gradients.

SGDCORE's current scope is deliberately narrow:

- Scalar `double` expressions.
- Explicit `Tape` ownership.
- A small set of arithmetic and elementary-function operations.
- Unit tests, benchmarks, and finite-difference verification.

It does not attempt to be a machine-learning framework, a tensor library, or
a competitor to large-scale AD systems.

## Why SGDCORE?

Most production AD libraries are large and optimized for specific workflows.
While powerful, they can obscure the underlying mechanics of reverse-mode
differentiation. SGDCORE exists to provide a small, readable implementation
where the full path from a variable to a gradient is visible:

```text
Variable
    │
    ▼
Operation
    │
    ▼
Computational Graph
    │
    ▼
Tape
    │
    ▼
Backward Pass
    │
    ▼
Gradient
```

This makes SGDCORE useful for:

- Understanding how reverse-mode AD works in practice.
- Prototyping new operations or autodiff extensions.
- Teaching the relationship between computational graphs and derivatives.
- Building small, dependency-light C++ applications that need exact gradients.

SGDCORE does not claim to be faster or more feature-rich than established
libraries. It claims to be small, explicit, and easy to inspect.

## Design Goals

- **Modern C++20**: use current language features while keeping the code
  readable.
- **Small API surface**: a new user should be productive with a handful of
  functions and types.
- **Explicit tape ownership**: every `Var<T>` belongs to a specific `Tape`,
  making dependencies and lifetimes obvious.
- **Reverse-mode differentiation**: compute gradients of scalar outputs with
  respect to inputs in a single pass.
- **Clear computational graph representation**: the tape directly represents
  the graph as a sequence of nodes.
- **Testability**: every operation has unit tests and finite-difference
  verification where applicable.
- **Minimal external dependencies**: the core library needs only CMake and a
  C++20 compiler.
- **Understandable implementation**: the code should be readable by a
  competent C++ developer without specialized autodiff knowledge.
- **Extensibility**: new operations can be added by extending `OpCode`,
  specializing `OperationTraits`, and adding an operator or free function.

## Key Features

- C++20 scalar reverse-mode automatic differentiation.
- Explicit `Tape` ownership with `Var<T>` handles.
- Arithmetic operators: `+`, `-`, `*`, `/`, unary `-`.
- Elementary functions: `sin`, `cos`, `tan`, `exp`, `log`, `sqrt`, `pow`.
- Free functions for differentiation: `backward`, `gradient`, `value`.
- Finite-difference verification utilities.
- GoogleTest unit tests.
- Google Benchmark performance benchmarks.
- CMake build system.
- GitHub Actions CI on Ubuntu, macOS, and Windows.
- MIT license.

## Current Status

**Status: early-stage / v0.1**

The current release establishes the core reverse-mode differentiation engine
and validates its correctness.

### Implemented

- Scalar reverse-mode autodiff.
- Core types: `Tape`, `Var<T>`, `Node`, `NodeId`, `OpCode`.
- Operation traits for arithmetic and elementary functions.
- `backward()` gradient propagation.
- Unit tests covering core behavior, operations, autodiff, and verification.
- Benchmarks for tape recording and backward-pass performance.
- Usage examples.
- CI pipeline.

### Planned

- Additional mathematical operations.
- User-defined operation hooks.
- JVP/VJP abstractions.
- Vector and matrix wrappers built on the scalar core.
- Higher-order differentiation exploration.

## Quick Start

```bash
git clone https://github.com/sgPhilia/SGDCORE.git
cd SGDCORE

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

ctest --test-dir build --output-on-failure
```

## Basic Usage

```cpp
#include <sgdcore/sgdcore.hpp>
#include <iostream>

int main() {
    sgdcore::Tape tape;

    auto x = sgdcore::variable(2.0, tape);
    auto y = sgdcore::variable(3.0, tape);

    auto z = x * y + sgdcore::sin(x);

    sgdcore::backward(z);

    std::cout << "z      = " << sgdcore::value(z)      << "\n";
    std::cout << "dz/dx  = " << sgdcore::gradient(x)   << "\n";
    std::cout << "dz/dy  = " << sgdcore::gradient(y)   << "\n";

    return 0;
}
```

Output:

```text
z      = 6.9093
dz/dx  = 2.58385
dz/dy  = 2
```

### Line-by-line explanation

- `sgdcore::Tape tape;` creates an empty tape that will own the computational
  graph.
- `sgdcore::variable(2.0, tape)` creates a differentiable input node with
  value `2.0`.
- `x * y + sgdcore::sin(x)` records multiplication, sine, and addition nodes
  into the tape.
- `sgdcore::backward(z)` starts reverse-mode differentiation at `z`.
- `sgdcore::gradient(x)` returns the accumulated adjoint at `x`.

Internally:

```text
z = x * y + sin(x)
∂z/∂x = y + cos(x) = 3.0 + cos(2.0) ≈ 2.58385
∂z/∂y = x            = 2.0
```

## How Automatic Differentiation Works

There are three common ways to compute derivatives:

| Method | Basic Idea | Result | Typical Use |
|--------|-----------|--------|-------------|
| Numerical | Approximate with small perturbations | Approximate | Quick checks, black-box functions |
| Symbolic | Manipulate mathematical expressions | Exact formula | Computer algebra systems |
| Automatic | Propagate derivatives through program operations | Exact (up to round-off) | Machine learning, optimization, physics |

SGDCORE uses automatic differentiation because it combines the exactness of
symbolic differentiation with the ability to handle arbitrary program
control flow, provided that flow can be expressed as a sequence of
differentiable operations.

## Computational Graph

A computational graph is a directed acyclic graph where:

- **Nodes** represent values.
- **Edges** represent dependencies between values.
- **Operations** transform input nodes into output nodes.

For the expression:

```cpp
auto z = x * y + sgdcore::sin(x);
```

the graph looks like:

```text
      x ────┐
            ▼
            * ◄──── y
            │
            ▼
            + ────► z
           ▲
           │
      x ──► sin
```

Each node stores:

- The operation that produced it.
- Its parent node ids.
- Its forward value.
- Its adjoint (accumulated during backward propagation).

The tape records nodes in the order they are created, which is a topological
order of the graph.

## Reverse-Mode Differentiation

Reverse-mode differentiation computes the gradient of a scalar output with
respect to all inputs in two phases:

1. **Forward phase**: evaluate the function and record the graph.
2. **Backward phase**: propagate adjoints from the output back to the inputs.

Starting from the output node with adjoint `1.0`, each operation distributes
its adjoint to its parents according to the chain rule.

For `z = x * y + sin(x)`:

```text
Forward:
  a = x * y
  b = sin(x)
  z = a + b

Backward:
  ∂z/∂z = 1
  ∂z/∂a = 1
  ∂z/∂b = 1
  ∂z/∂x = ∂z/∂a * ∂a/∂x + ∂z/∂b * ∂b/∂x
        = 1 * y + 1 * cos(x)
        = y + cos(x)
  ∂z/∂y = ∂z/∂a * ∂a/∂y
        = 1 * x
        = x
```

This is why reverse mode is efficient for scalar outputs: every input
gradient is obtained in one backward traversal.

## Tape

`Tape` is the central owner of the computational graph.

Responsibilities:

- Allocate and store `Node` objects.
- Create variable and constant nodes.
- Record the result of each operation as a new node.
- Run the backward pass.
- Clear adjoints or reset state.

`Tape` stores nodes in a `std::vector<Node>`. Each operation appends one
node, so node ids are increasing and parents always have smaller ids than
their children. This ordering makes a single reverse pass sufficient.

### Why explicit tape ownership?

An explicit tape makes several things clear:

- Which graph a `Var<T>` belongs to.
- When the graph is destroyed.
- That gradients are scoped to a specific computation.
- That mixing variables from different graphs is an error.

```cpp
sgdcore::Tape tape;
auto x = sgdcore::variable(1.0, tape);
// x is valid only as long as tape exists.
```

## Variables

`Var<T>` is a lightweight, non-owning handle to a node in a `Tape`.

Default: `Var<double>`.

- `variable(value, tape)` creates an input node whose gradient is computed.
- `constant(value, tape)` creates a fixed-value node.
- `value(var)` returns the forward value.
- `gradient(var)` returns the accumulated adjoint after `backward`.

Variables and constants differ only in their `OpCode`. Both participate in
the graph, but the backward pass skips leaf nodes when propagating.

## Operations

Every differentiable operation in SGDCORE is represented by:

1. An `OpCode` value.
2. A specialization of `OperationTraits<OpCode>` defining forward evaluation
   and partial derivatives.
3. An operator or free function that records the operation on the tape.

When you write:

```cpp
auto z = x + y;
```

SGDCORE:

1. Looks up the forward result using `OperationTraits<OpCode::Add>::forward`.
2. Records a new node with opcode `Add` and parents `x` and `y`.
3. Returns a `Var<T>` handle to the new node.

During `backward`, the tape uses the same traits to compute partial
derivatives and accumulate adjoints into the parents.

## Supported Operations

### Arithmetic

| Operation | C++ | Expression | Partial Derivatives |
|-----------|-----|------------|---------------------|
| Addition | `x + y` | `x + y` | `∂/∂x = 1`, `∂/∂y = 1` |
| Subtraction | `x - y` | `x - y` | `∂/∂x = 1`, `∂/∂y = -1` |
| Multiplication | `x * y` | `x * y` | `∂/∂x = y`, `∂/∂y = x` |
| Division | `x / y` | `x / y` | `∂/∂x = 1/y`, `∂/∂y = -x / y²` |
| Negation | `-x` | `-x` | `∂/∂x = -1` |

### Elementary functions

| Function | C++ | Derivative |
|----------|-----|------------|
| Sine | `sin(x)` | `cos(x)` |
| Cosine | `cos(x)` | `-sin(x)` |
| Tangent | `tan(x)` | `1 / cos²(x)` |
| Exponential | `exp(x)` | `exp(x)` |
| Natural log | `log(x)` | `1 / x` |
| Square root | `sqrt(x)` | `1 / (2 * sqrt(x))` |
| Power | `pow(x, y)` | `∂/∂x = y * x^(y-1)`, `∂/∂y = x^y * log(x)` |

Only these operations are currently implemented.

## API Overview

### Core types

| Type | Description |
|------|-------------|
| `sgdcore::Tape` | Owns the computational graph. |
| `sgdcore::Var<T>` | Handle to a node; default `Var<double>`. |
| `sgdcore::NodeId` | 32-bit handle to a node. |
| `sgdcore::OpCode` | Tag identifying an operation. |
| `sgdcore::Node` | Internal node struct. |
| `sgdcore::OperationTraits<OpCode>` | Forward and derivative rules. |

### Construction

```cpp
auto x = sgdcore::variable(value, tape);
auto c = sgdcore::constant(value, tape);
```

### Differentiation

```cpp
sgdcore::backward(var);      // run reverse-mode AD from var
```

### Gradient access

```cpp
double g = sgdcore::gradient(var);
double v = sgdcore::value(var);
```

### Mathematical operations

```cpp
auto a = x + y;
auto b = x - y;
auto c = x * y;
auto d = x / y;
auto e = -x;

auto f = sgdcore::sin(x);
auto g = sgdcore::cos(x);
auto h = sgdcore::tan(x);
auto i = sgdcore::exp(x);
auto j = sgdcore::log(x);
auto k = sgdcore::sqrt(x);
auto l = sgdcore::pow(x, y);
```

### Verification

```cpp
auto f = [](double v) { return std::sin(v); };
double fd = sgdcore::central_difference(f, x.value(), 1e-5);
```

## Building from Source

Requirements:

- C++20 compiler (GCC 11+, Clang 14+, MSVC 2022)
- CMake 3.14 or newer

Default build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Build with all optional components:

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DSGDCORE_BUILD_TESTS=ON \
    -DSGDCORE_BUILD_BENCHMARKS=ON \
    -DSGDCORE_BUILD_EXAMPLES=ON
cmake --build build -j
```

Build only the library:

```bash
cmake -S . -B build -DSGDCORE_BUILD_TESTS=OFF -DSGDCORE_BUILD_BENCHMARKS=OFF -DSGDCORE_BUILD_EXAMPLES=OFF
cmake --build build -j
```

## Running Tests

```bash
cmake -S . -B build -DSGDCORE_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

The test suite is organized into:

- `test_core.cpp` — tape internals and node management.
- `test_ops.cpp` — arithmetic and elementary-function evaluation.
- `test_autodiff.cpp` — reverse-mode gradient correctness.
- `test_verification.cpp` — finite-difference comparison tests.

## Running Verification

Verification tests compare SGDCORE gradients against central finite
differences:

```text
f'(x) ≈ [f(x + h) - f(x - h)] / (2h)
```

Run them as part of the normal test suite:

```bash
ctest --test-dir build --output-on-failure
```

The `test_verification.cpp` file contains checks such as:

```cpp
auto f = [](double x) { return std::sin(x) * std::exp(x); };
double ad = ad_gradient(x);
double fd = sgdcore::central_difference(f, x, 1e-5);
EXPECT_TRUE(sgdcore::close_enough(ad, fd, 1e-6, 1e-9));
```

Finite differences are a validation tool, not a proof of correctness. They
catch implementation mistakes but introduce their own truncation and
round-off error.

## Running Benchmarks

```bash
cmake -S . -B build -DSGDCORE_BUILD_BENCHMARKS=ON
cmake --build build -j
./build/benchmarks/sgdcore_benchmark
```

The benchmark suite measures:

- Cost of recording linear, multiplication, mixed, and unary-heavy graphs.
- Cost of the backward pass for each graph shape.
- Cost of clearing adjoints and resetting the tape.

Benchmark output will vary by hardware and compiler; run locally for actual
numbers.

## Examples

### `examples/simple.cpp`

Demonstrates basic variables, arithmetic, `sin`, and gradient retrieval.

```bash
./build/examples/sgdcore_example_simple
```

### `examples/chain.cpp`

Demonstrates a chained computation where the same variable is reused across
multiple operations.

```bash
./build/examples/sgdcore_example_chain
```

## Project Structure

```text
SGDCORE/
├── include/sgdcore/      Public headers
│   ├── sgdcore.hpp       Single include
│   ├── core/             Tape, Var, Node, NodeId, OpCode
│   ├── operations/       OperationTraits, arithmetic, elementary functions
│   ├── autodiff/         backward(), gradient(), value()
│   └── verification/     finite_difference, tolerance
├── src/                  Implementation
│   └── tape.cpp
├── tests/                GoogleTest tests
├── benchmarks/           Google Benchmark benchmarks
├── examples/             Usage examples
├── docs/                 Architecture and design notes
├── .github/workflows/    CI configuration
├── CMakeLists.txt
├── README.md
├── CHANGELOG.md
├── LICENSE
└── setup_repo.sh
```

## Architecture

SGDCORE's architecture is a pipeline:

```text
             ┌─────────────┐
             │    Var<T>   │
             └──────┬──────┘
                    │
                    ▼
             ┌─────────────┐
             │  Operation  │
             │   (OpCode)  │
             └──────┬──────┘
                    │
                    ▼
             ┌─────────────┐
             │     Tape    │
             └──────┬──────┘
                    │
                    ▼
             ┌─────────────┐
             │   Backward  │
             └──────┬──────┘
                    │
                    ▼
             ┌─────────────┐
             │  Gradients  │
             └─────────────┘
```

A `Var<T>` is a handle. An operation creates a node on the tape. The tape
stores the graph. `backward()` traverses the graph in reverse and fills in
adjoints. `gradient()` reads those adjoints.

## Design Decisions

### Explicit Tape

Requiring users to create and pass a `Tape` makes graph ownership explicit.
There is no risk of accidentally mixing computations from different graphs,
and the lifetime of every `Var<T>` is tied to a single, visible object.

### Scalar-First Design

Starting with scalar `double` expressions keeps the initial core small. The
tape, node, and operation traits can be understood without tensor notation
or broadcast semantics. Higher-dimensional abstractions can be built on top
later.

### Reverse Mode

Reverse mode is the natural starting point for a gradient-oriented library.
A single backward pass yields all input gradients for a scalar output,
matching common optimization and machine-learning use cases.

### Small Core

SGDCORE avoids expression templates, operator-overloading trickery, and
large dependencies. The implementation is meant to be read and modified.

### C++20

C++20 provides a stable, widely supported baseline. SGDCORE uses it for
standard library features and modern type traits without relying on
experimental compiler extensions.

## Memory and Lifetime

- A `Tape` owns all nodes allocated during its lifetime.
- A `Var<T>` stores a raw pointer to its tape and a node id.
- Using a `Var<T>` after its `Tape` has been destroyed is undefined behavior.
- `Tape::reset()` clears all nodes and invalidates all `Var<T>` handles from
  that tape.
- `Tape::clear_adjoints()` resets gradients without destroying the graph,
  allowing multiple backward passes.

These constraints match the current implementation.

## Correctness and Numerical Verification

SGDCORE validates derivative implementations through several layers:

```text
Implementation
      │
      ▼
Unit Tests (operation correctness)
      │
      ▼
Analytical Expectations (known derivatives)
      │
      ▼
Finite-Difference Verification
      │
      ▼
Composed Graph Tests (chain rule, multiple usages)
```

Finite differences provide an independent numerical check. They are not a
formal proof, but they are effective at catching sign errors, missing
factors, and incorrect partial derivatives.

## Performance Considerations

Factors that influence SGDCORE performance:

- **Graph size**: more nodes mean more memory and more work in both forward
  and backward passes.
- **Tape allocation**: nodes are appended to a single vector; growth and
  cache behavior matter for large graphs.
- **Operation complexity**: elementary functions such as `sin`, `exp`, and
  `pow` are more expensive than arithmetic.
- **Backward traversal**: every non-leaf node visits its parents once.
- **Multiple usages**: a node used several times accumulates gradients from
  every path, which is correct but adds work.

The included benchmarks measure these characteristics across several graph
shapes. No absolute performance claims are made.

## Limitations

SGDCORE currently does not provide:

- Tensor or matrix operations.
- GPU acceleration.
- Neural-network layers or optimizers.
- Automatic batching.
- Higher-order derivatives.
- Expression templates for compile-time graph construction.
- Multithreading support.
- Integration with Eigen, NumPy, PyTorch, or other frameworks.

These limitations are intentional for v0.1.

## Non-Goals

SGDCORE is not trying to become:

- A complete machine-learning framework.
- A replacement for mature AD libraries such as Stan, autodiff, or Enzyme.
- A tensor computation library.
- A production-grade optimizer or solver.

It is a small, inspectable autodiff core.

## Roadmap

### Current (v0.1)

- Scalar reverse-mode autodiff.
- Core arithmetic and elementary functions.
- Explicit tape ownership.
- Unit tests, benchmarks, examples.
- CI and documentation.

### Next

- Additional elementary and special functions.
- Improved tape storage and memory reuse.
- Cleaner user-defined operation interface.

### Future

- Vector and matrix wrappers built on the scalar core.
- JVP/VJP abstractions.
- Higher-order differentiation exploration.
- Optional integrations with small linear-algebra libraries.

### Research / Exploration

- Static graph compilation.
- Checkpointing and memory/recomputation trade-offs.

No release dates are promised.

## Development Guide

Clone and configure:

```bash
git clone https://github.com/sgPhilia/SGDCORE.git
cd SGDCORE
cmake -S . -B build -DSGDCORE_BUILD_TESTS=ON -DSGDCORE_BUILD_BENCHMARKS=ON -DSGDCORE_BUILD_EXAMPLES=ON
```

Build:

```bash
cmake --build build -j
```

Run tests:

```bash
ctest --test-dir build --output-on-failure
```

Run benchmarks:

```bash
./build/benchmarks/sgdcore_benchmark
```

Run examples:

```bash
./build/examples/sgdcore_example_simple
./build/examples/sgdcore_example_chain
```

## Adding a New Differentiable Operation

To add a new operation to SGDCORE:

1. Add a value to `OpCode` in `include/sgdcore/core/opcode.hpp`.
2. Specialize `OperationTraits<OpCode>` in
   `include/sgdcore/operations/traits.hpp` with `forward`, `pdiff_lhs`, and
   `pdiff_rhs` (or only `pdiff_lhs` for unary operations).
3. Add an operator or free function in the appropriate header under
   `include/sgdcore/operations/`.
4. Add unit tests in `tests/`.
5. Add finite-difference verification where applicable.
6. Add a benchmark if performance is relevant.
7. Update `README.md` and `docs/design.md`.

## FAQ

**What is SGDCORE?**  
A small C++20 library for scalar reverse-mode automatic differentiation.

**Why reverse-mode autodiff?**  
It computes all input gradients for a scalar output in a single backward
pass, which is efficient for optimization and machine-learning-style
workloads.

**Why is there a Tape?**  
The tape owns the computational graph. Explicit ownership keeps lifetimes
and dependencies clear.

**Why not use symbolic differentiation?**  
Symbolic differentiation produces explicit formulas but struggles with
arbitrary program control flow. AD handles program-defined functions
directly.

**Why not use finite differences?**  
Finite differences are simple but approximate and numerically fragile. AD
computes exact derivatives up to floating-point round-off.

**Does SGDCORE support tensors?**  
No. v0.1 is scalar-only.

**Is SGDCORE a machine-learning framework?**  
No. It is a small autodiff core, not a framework.

**What C++ standard is required?**  
C++20.

**How are gradients computed?**  
By recording operations into a tape and propagating adjoints backward
through the recorded graph.

## License

SGDCORE is released under the MIT License. See [LICENSE](LICENSE).