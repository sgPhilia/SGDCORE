# SGDCORE

### Differentiable Computational Core

[![CI](https://github.com/sgPhilia/SGDCORE/actions/workflows/ci.yml/badge.svg)](https://github.com/sgPhilia/SGDCORE/actions/workflows/ci.yml)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/std/the-standard)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Release](https://img.shields.io/github/v/release/sgPhilia/SGDCORE)](https://github.com/sgPhilia/SGDCORE/releases)

SGDCORE is a lightweight **C++20 reverse-mode automatic differentiation core** for scalar computational graphs.

It records arithmetic and mathematical operations into an explicit `Tape`, then traverses the recorded graph backward to propagate adjoints and obtain gradients.

The project is intentionally small and inspectable. It is not a machine-learning framework, tensor library, or general-purpose numerical computing system.

---

## Table of Contents

* [Overview](#overview)
* [Motivation](#motivation)
* [What is SGDCORE?](#what-is-sgdcore)
* [Why a Differentiable Computational Core?](#why-a-differentiable-computational-core)
* [Design Philosophy](#design-philosophy)
* [Key Features](#key-features)
* [Current Status](#current-status)
* [Requirements](#requirements)
* [Quick Start](#quick-start)
* [Basic Usage](#basic-usage)
* [How Automatic Differentiation Works](#how-automatic-differentiation-works)
* [Reverse-Mode Automatic Differentiation](#reverse-mode-automatic-differentiation)
* [Computational Graph](#computational-graph)
* [Tape Architecture](#tape-architecture)
* [Variables](#variables)
* [Operations](#operations)
* [Supported Operations](#supported-operations)
* [Gradient Propagation](#gradient-propagation)
* [Gradient Verification](#gradient-verification)
* [Numerical Verification](#numerical-verification)
* [Testing](#testing)
* [Benchmarking](#benchmarking)
* [API Overview](#api-overview)
* [Project Structure](#project-structure)
* [Architecture](#architecture)
* [Memory and Lifetime](#memory-and-lifetime)
* [Design Decisions](#design-decisions)
* [Limitations](#limitations)
* [Non-Goals](#non-goals)
* [Roadmap](#roadmap)
* [Development](#development)
* [Extending SGDCORE](#extending-sgdcore)
* [Adding a Differentiable Operation](#adding-a-differentiable-operation)
* [Correctness Philosophy](#correctness-philosophy)
* [Performance Philosophy](#performance-philosophy)
* [FAQ](#faq)
* [Contributing](#contributing)
* [License](#license)

---

## Overview

Differentiable programming is the practice of constructing programs whose derivatives can be computed automatically.

SGDCORE provides a small implementation of **reverse-mode automatic differentiation** in modern C++. The core workflow is:

1. Represent expressions as a computational graph.
2. Record operations into an explicit `Tape`.
3. Evaluate forward values.
4. Traverse the graph backward.
5. Accumulate adjoints.
6. Read gradients from input variables.

SGDCORE is currently **scalar-first**. Its primary use case is scalar expressions represented through `Var<T>`, with `double` as the normal numerical type.

Higher-dimensional abstractions such as tensors and matrices are outside the current v0.1 scope.

---

## Motivation

SGDCORE started as an exploration of how automatic differentiation can be implemented from first principles in modern C++.

Instead of hiding the entire differentiation process behind a large framework, SGDCORE keeps the main mechanisms explicit:

```text
Expression
    │
    ▼
Computational Graph
    │
    ▼
Tape Recording
    │
    ▼
Forward Evaluation
    │
    ▼
Backward Propagation
    │
    ▼
Gradients
```

The goal is to keep this path small enough to inspect, test, understand, and extend.

---

## What is SGDCORE?

The name **SGDCORE** refers to:

> **Differentiable Computational Core**

The name describes the project's focus on the underlying computational infrastructure of differentiable programs rather than specifically referring to stochastic gradient descent.

For example:

```cpp
auto z = x * y + sgdcore::sin(x);
```

SGDCORE records the operations required to construct the expression as nodes in a computational graph.

Conceptually:

```text
          z
          │
      ┌───┴───┐
      │   +   │
      └─┬───┬─┘
        │   │
       x*y sin(x)
       │
     ┌─┴─┐
     x   y
```

After:

```cpp
sgdcore::backward(z);
```

the graph is traversed in reverse and the input adjoints become available through:

```cpp
sgdcore::gradient(x);
sgdcore::gradient(y);
```

---

## Why a Differentiable Computational Core?

A small autodiff core can be useful for:

* Mathematical optimization
* Scientific computing
* Sensitivity analysis
* Numerical experimentation
* Differentiable programming
* Learning how automatic differentiation systems work

The purpose of SGDCORE is not to provide every feature required by these fields.

Instead, it focuses on making the fundamental mechanism understandable:

```text
operation
   ↓
node
   ↓
graph
   ↓
backward pass
   ↓
gradient
```

---

## Design Philosophy

### Small Core

The core should remain small enough to read and understand without navigating a large framework.

SGDCORE intentionally avoids unnecessary layers between the user expression and the computational graph.

### Explicit Tape

The computational graph is associated with an explicit `Tape`.

This makes the lifetime and ownership of the recorded computation visible to the user.

### C++20

SGDCORE uses C++20 as its language baseline.

The implementation relies on standard C++ facilities rather than compiler-specific extensions.

### Correctness First

Forward evaluation alone is not sufficient for an autodiff system.

SGDCORE therefore validates derivative implementations through:

* Unit tests
* Analytical expectations
* Finite-difference checks
* Composed computational graphs

### Minimal API

The public interface is intentionally small.

The core workflow can be expressed using:

```cpp
Tape
Var<T>
variable()
constant()
backward()
gradient()
value()
```

### Extensibility

Operations are represented through operation identifiers and operation traits.

This keeps forward evaluation and derivative rules associated with the same operation definition.

---

## Key Features

* C++20 reverse-mode automatic differentiation
* Explicit tape-based graph recording
* Scalar differentiable variables through `Var<T>`
* Arithmetic operations
* Elementary mathematical functions
* Gradient accumulation
* Finite-difference verification utilities
* GoogleTest-based tests
* Google Benchmark-based benchmarks
* CMake build system
* GitHub Actions CI
* MIT license

---

## Current Status

**Current stage: v0.1**

SGDCORE v0.1 focuses on establishing a small, testable scalar reverse-mode autodiff core.

### Implemented

* Scalar reverse-mode automatic differentiation
* `Tape`
* `Var<T>`
* `Node`
* `NodeId`
* `OpCode`
* Arithmetic operations
* Elementary mathematical functions
* Backward propagation
* Gradient accumulation
* Finite-difference verification
* Unit tests
* Benchmarks
* Examples
* CI

### Planned

Potential future work includes:

* Additional differentiable operations
* Improved tape storage and memory reuse
* Cleaner user-defined operation interfaces
* Vector and matrix wrappers
* More explicit JVP/VJP abstractions
* Exploration of higher-order differentiation

Planned items are not part of the current v0.1 API unless explicitly implemented.

---

## Requirements

Typical requirements:

* C++20-compatible compiler
* CMake
* Git

The repository currently uses CMake for configuration and building, with GoogleTest and Google Benchmark used by the testing and benchmarking components.

---

## Quick Start

Clone the repository:

```bash
git clone https://github.com/sgPhilia/SGDCORE.git
cd SGDCORE
```

Configure a Release build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

Build:

```bash
cmake --build build -j
```

Run the tests:

```bash
ctest --test-dir build --output-on-failure
```

---

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

    std::cout << "z      = " << sgdcore::value(z) << '\n';
    std::cout << "dz/dx  = " << sgdcore::gradient(x) << '\n';
    std::cout << "dz/dy  = " << sgdcore::gradient(y) << '\n';

    return 0;
}
```

For:

```text
z = x * y + sin(x)
```

with:

```text
x = 2
y = 3
```

the derivatives are:

```text
dz/dx = y + cos(x)
      ≈ 2.583853163...

dz/dy = x
      = 2
```

---

## How Automatic Differentiation Works

There are three common approaches to derivative computation.

| Method                    | Approach                                          | Typical Result                          |
| ------------------------- | ------------------------------------------------- | --------------------------------------- |
| Numerical differentiation | Evaluate perturbed inputs                         | Approximation                           |
| Symbolic differentiation  | Manipulate mathematical expressions               | Symbolic formula                        |
| Automatic differentiation | Apply derivative rules through program operations | Derivative of the evaluated computation |

### Numerical Differentiation

A central finite difference approximates:

```text
f'(x) ≈ [f(x + h) - f(x - h)] / (2h)
```

It is useful for verification but is affected by truncation and floating-point round-off.

### Symbolic Differentiation

Symbolic differentiation transforms mathematical expressions into other mathematical expressions.

For example:

```text
f(x) = x² + sin(x)
```

can be transformed into:

```text
f'(x) = 2x + cos(x)
```

### Automatic Differentiation

Automatic differentiation decomposes a computation into elementary operations and applies the chain rule to those operations.

It does not approximate the derivative through perturbation.

Its numerical result is nevertheless subject to floating-point arithmetic.

---

## Reverse-Mode Automatic Differentiation

SGDCORE uses reverse-mode automatic differentiation.

The process can be viewed as two major stages.

### Forward Pass

The expression is evaluated while the computational graph is recorded.

```text
Inputs
  │
  ▼
Operations
  │
  ▼
Forward values
  │
  ▼
Recorded graph
```

### Backward Pass

The graph is traversed in reverse.

```text
Output adjoint
      │
      ▼
Local derivatives
      │
      ▼
Parent adjoints
      │
      ▼
Input gradients
```

For a scalar output `z`, the reverse pass begins with:

```text
dz/dz = 1
```

The chain rule is then applied at every node.

For:

```text
z = x * y + sin(x)
```

we obtain:

```text
dz/dx = y + cos(x)

dz/dy = x
```

The main advantage of reverse mode is that a single backward traversal can produce derivatives with respect to many inputs when the output is scalar.

---

## Computational Graph

A computational graph represents a computation as connected operations and values.

Conceptually:

```text
Input Variables
      │
      ▼
  Operations
      │
      ▼
 Intermediate Values
      │
      ▼
    Output
```

SGDCORE records the graph as `Node` objects associated with a `Tape`.

A node represents a recorded computation and may contain information such as:

| Component            | Purpose                                    |
| -------------------- | ------------------------------------------ |
| Operation identifier | Identifies the operation                   |
| Parent references    | Identify dependencies                      |
| Forward value        | Stores the evaluated value                 |
| Adjoint              | Stores accumulated reverse-mode derivative |

The exact internal representation should be considered an implementation detail rather than part of the high-level user API.

---

## Tape Architecture

The `Tape` is the central storage object for a recorded computation.

Conceptually:

```text
Tape
 │
 ├── Node 0
 ├── Node 1
 ├── Node 2
 ├── Node 3
 └── ...
```

Each recorded operation produces another graph node.

The tape provides the storage required for:

* Input variables
* Constants
* Intermediate results
* Operation information
* Forward values
* Backward adjoints

### Why an Explicit Tape?

Explicit tape ownership makes the computational context visible.

For example:

```cpp
sgdcore::Tape tape;

auto x = sgdcore::variable(1.0, tape);
```

`x` references a node associated with `tape`.

This makes the lifetime relationship between the graph and its handles explicit.

---

## Variables

`Var<T>` is a lightweight handle referring to a recorded node.

Typical construction:

```cpp
auto x = sgdcore::variable(2.0, tape);
```

A constant can be constructed with:

```cpp
auto c = sgdcore::constant(5.0, tape);
```

Values can be accessed with:

```cpp
sgdcore::value(x);
```

After a backward pass, gradients can be accessed with:

```cpp
sgdcore::gradient(x);
```

The primary numerical use case in v0.1 is `double`.

### Variable vs Constant

Variables participate in differentiation as inputs.

Constants participate in forward computation but do not represent differentiable input parameters.

---

## Operations

An operation connects input nodes to a newly recorded output node.

For example:

```cpp
auto z = x + y;
```

conceptually performs:

```text
x ─────┐
       │
       ▼
      add ───► z
       ▲
       │
y ─────┘
```

An operation needs two kinds of information:

1. How to compute its forward value.
2. How to compute its local derivatives during the backward pass.

SGDCORE associates operation definitions with `OpCode` and `OperationTraits`.

---

## Supported Operations

### Arithmetic

| Operation      | Expression | Derivative               |
| -------------- | ---------- | ------------------------ |
| Addition       | `x + y`    | `dx = 1`, `dy = 1`       |
| Subtraction    | `x - y`    | `dx = 1`, `dy = -1`      |
| Multiplication | `x * y`    | `dx = y`, `dy = x`       |
| Division       | `x / y`    | `dx = 1/y`, `dy = -x/y²` |
| Negation       | `-x`       | `dx = -1`                |

### Elementary Functions

| Function          | Expression  | Derivative                         |
| ----------------- | ----------- | ---------------------------------- |
| Sine              | `sin(x)`    | `cos(x)`                           |
| Cosine            | `cos(x)`    | `-sin(x)`                          |
| Tangent           | `tan(x)`    | `1/cos²(x)`                        |
| Exponential       | `exp(x)`    | `exp(x)`                           |
| Natural logarithm | `log(x)`    | `1/x`                              |
| Square root       | `sqrt(x)`   | `1/(2sqrt(x))`                     |
| Power             | `pow(x, y)` | `∂x = yx^(y-1)`, `∂y = x^y log(x)` |

The derivative of `pow(x, y)` with respect to `y` requires a domain in which `log(x)` is defined.

Domain restrictions of the underlying mathematical functions still apply.

---

## Gradient Propagation

Reverse-mode differentiation propagates adjoints through the graph.

For:

```cpp
auto z = x * y;
```

the local derivatives are:

```text
∂z/∂x = y
∂z/∂y = x
```

If the output has adjoint `a`, the contributions become:

```text
adjoint(x) += a * y
adjoint(y) += a * x
```

### Gradient Accumulation

A node can contribute to the same variable through multiple paths.

For example:

```cpp
auto y = x * x + x;
```

mathematically:

```text
y = x² + x
```

therefore:

```text
dy/dx = 2x + 1
```

The backward pass must accumulate both contributions to `x`.

This accumulation is fundamental to reverse-mode automatic differentiation.

---

## Gradient Verification

SGDCORE uses finite differences as an independent numerical check of derivative implementations.

For a scalar function:

```text
f'(x) ≈ [f(x + h) - f(x - h)] / (2h)
```

This can detect:

* Sign errors
* Incorrect derivative factors
* Incorrect operation implementations
* Errors in composed expressions

Finite differences are approximate and therefore provide validation evidence rather than a mathematical proof.

---

## Numerical Verification

A typical verification workflow is:

```text
             Analytical AD
                  │
                  ▼
              Gradient
                  │
                  │ compare
                  ▼
          Finite Difference
                  │
                  ▼
              Difference
```

For example:

```cpp
double fd =
    sgdcore::central_difference(f, x_value, 1e-5);
```

The result can then be compared using an appropriate numerical tolerance.

### Step Size

The finite-difference step size affects accuracy.

A step that is too large can increase truncation error.

A step that is too small can increase floating-point cancellation and round-off error.

Therefore, a value such as:

```text
1e-5
```

should be treated as a starting point rather than a universal optimal value.

---

## Testing

SGDCORE uses GoogleTest for automated testing.

The repository contains dedicated tests covering core behavior, operations, autodiff, and numerical verification.

Build the test configuration:

```bash
cmake -S . -B build -DSGDCORE_BUILD_TESTS=ON
cmake --build build -j
```

Run:

```bash
ctest --test-dir build --output-on-failure
```

The test suite is intended to verify several layers:

```text
Core behavior
      ↓
Operation behavior
      ↓
Autodiff behavior
      ↓
Numerical verification
```

---

## Benchmarking

SGDCORE includes Google Benchmark-based benchmarks.

Configure:

```bash
cmake -S . -B build -DSGDCORE_BUILD_BENCHMARKS=ON
```

Build:

```bash
cmake --build build -j
```

Run:

```bash
./build/benchmarks/sgdcore_benchmark
```

Benchmark results depend on:

* CPU
* Compiler
* Compiler flags
* Standard library
* Operating system
* Build configuration

Therefore, benchmark results should be interpreted in the context of the environment in which they were measured.

---

## API Overview

### Core Types

| Type                       | Purpose                            |
| -------------------------- | ---------------------------------- |
| `sgdcore::Tape`            | Stores a computational graph       |
| `sgdcore::Var<T>`          | Handle to a graph node             |
| `sgdcore::NodeId`          | Identifies a graph node            |
| `sgdcore::Node`            | Internal graph node representation |
| `sgdcore::OpCode`          | Identifies an operation            |
| `sgdcore::OperationTraits` | Defines operation behavior         |

### Construction

```cpp
auto x = sgdcore::variable(value, tape);
auto c = sgdcore::constant(value, tape);
```

### Differentiation

```cpp
sgdcore::backward(z);
```

### Value and Gradient

```cpp
double v = sgdcore::value(x);
double g = sgdcore::gradient(x);
```

### Mathematical Operations

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

---

## Project Structure

The repository is organized around the core library, tests, benchmarks, examples, documentation, and build infrastructure.

```text
SGDCORE/
├── .github/
│   └── workflows/
│
├── include/
│   └── sgdcore/
│
├── src/
│
├── tests/
│
├── benchmarks/
│
├── examples/
│
├── docs/
│
├── CMakeLists.txt
├── README.md
├── CHANGELOG.md
├── LICENSE
├── setup_repo.sh
└── .gitignore
```

The public umbrella header is:

```cpp
#include <sgdcore/sgdcore.hpp>
```

---

## Architecture

The high-level architecture can be summarized as:

```text
                User Expression
                       │
                       ▼
                 Var<T> Handles
                       │
                       ▼
                  Operations
                       │
                       ▼
                     Tape
                       │
                       ▼
              Computational Graph
                       │
                       ▼
                 backward()
                       │
                       ▼
                  Adjoints
                       │
                       ▼
                  gradient()
```

The important separation is:

```text
Var<T>
  │
  └── references graph state

Operation
  │
  └── defines forward/local derivative behavior

Tape
  │
  └── stores graph state

backward()
  │
  └── propagates adjoints
```

---

## Memory and Lifetime

The tape provides the lifetime context for graph nodes.

A `Var<T>` is a handle rather than an owner of the graph itself.

Conceptually:

```text
Tape
 │
 ├── Node
 ├── Node
 ├── Node
 └── Node
      ▲
      │
    Var<T>
```

Therefore, code using a `Var<T>` must respect the lifetime of the associated `Tape`.

When the tape is destroyed, the graph storage it owns is destroyed as well.

Operations that reset or clear graph state can also affect the validity of previously obtained handles, depending on the specific operation and implementation.

For this reason, graph lifetime should remain explicit in user code.

---

## Design Decisions

### Explicit Tape Ownership

The graph is associated with a user-created `Tape` rather than a hidden global context.

This makes computational state explicit.

### Scalar-First Design

Starting with scalar expressions keeps the initial implementation small.

There are no tensor shapes, broadcasting rules, device abstractions, or matrix semantics in the core v0.1 design.

### Reverse Mode

Reverse mode is well suited to scalar-output functions where gradients with respect to multiple inputs are required.

### Minimal Public API

The core API intentionally exposes only the concepts required to construct expressions and differentiate them.

### Verification

Derivative implementations are treated as code that requires independent verification.

Analytical tests and numerical checks complement each other.

---

## Limitations

The current v0.1 scope does not include:

* Tensor operations
* Matrix operations
* GPU backends
* Neural-network layers
* Optimizers
* Automatic batching
* Higher-order differentiation
* Multithreaded graph execution
* Framework integrations
* Large-scale tensor computation

These limitations describe the current scope of the project.

---

## Non-Goals

SGDCORE is not intended to be:

* A complete machine-learning framework
* A tensor computation framework
* A general numerical computing environment
* A production optimizer or solver
* A replacement for established automatic-differentiation systems

The project instead focuses on a small and inspectable differentiable computational core.

---

## Roadmap

### v0.1

* Scalar reverse-mode autodiff
* Explicit tape
* `Var<T>`
* Core arithmetic operations
* Elementary mathematical operations
* Gradient accumulation
* Numerical verification
* Tests
* Benchmarks
* Examples
* CI

### Next

* Additional operations
* Tape storage improvements
* Memory reuse
* Improved custom-operation interface
* Expanded verification coverage

### Future

* Vector and matrix wrappers
* More explicit JVP/VJP abstractions
* Higher-order differentiation exploration
* Additional numerical integrations

### Exploration

Potential research directions include:

* Checkpointing
* Recomputation strategies
* Alternative tape storage strategies
* Static graph compilation

Roadmap items are exploratory and do not imply a release schedule.

---

## Development

Clone:

```bash
git clone https://github.com/sgPhilia/SGDCORE.git
cd SGDCORE
```

Configure a development build:

```bash
cmake -S . -B build \
    -DSGDCORE_BUILD_TESTS=ON \
    -DSGDCORE_BUILD_BENCHMARKS=ON \
    -DSGDCORE_BUILD_EXAMPLES=ON
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

Run examples when built:

```bash
./build/examples/sgdcore_example_simple
./build/examples/sgdcore_example_chain
```

### CMake Options

Available options should be kept synchronized with `CMakeLists.txt`.

Common project options include:

| Option                     | Purpose              |
| -------------------------- | -------------------- |
| `SGDCORE_BUILD_TESTS`      | Build the test suite |
| `SGDCORE_BUILD_BENCHMARKS` | Build benchmarks     |
| `SGDCORE_BUILD_EXAMPLES`   | Build examples       |

---

## Extending SGDCORE

SGDCORE is designed to grow from the scalar core outward.

A typical extension workflow is:

```text
New Operation
      │
      ▼
Operation Definition
      │
      ▼
Forward Test
      │
      ▼
Analytical Gradient Test
      │
      ▼
Finite-Difference Test
      │
      ▼
Benchmark
      │
      ▼
Documentation
```

This keeps new functionality connected to the same correctness workflow as the existing operations.

---

## Adding a Differentiable Operation

A new operation generally requires several pieces.

### 1. Add the Operation Identifier

Add the operation to the appropriate `OpCode` definition.

### 2. Define Operation Behavior

Add the corresponding operation traits.

The implementation should define:

* Forward evaluation
* Local derivative with respect to the left input
* Local derivative with respect to the right input where applicable

### 3. Add the Public Operation

Expose the operation through the appropriate function or operator.

### 4. Add Tests

Test:

* Forward values
* Basic edge cases
* Analytical derivatives
* Composed expressions

### 5. Add Numerical Verification

Where applicable, compare the analytical derivative against a finite-difference approximation.

### 6. Add Benchmarks

Add a benchmark when the operation or implementation introduces a meaningful performance consideration.

### 7. Update Documentation

Update:

* Supported operations
* API documentation
* README
* Design documentation

---

## Correctness Philosophy

SGDCORE treats derivative correctness as a separate concern from forward computation.

The verification pipeline is:

```text
              Implementation
                    │
                    ▼
               Unit Tests
                    │
                    ▼
          Analytical Expectations
                    │
                    ▼
        Finite-Difference Checks
                    │
                    ▼
          Composed Graph Tests
```

Each layer catches different classes of problems.

For example, an operation may produce the correct forward value while containing an incorrect derivative rule.

Finite differences provide an independent numerical check, but they are still subject to floating-point and approximation error.

---

## Performance Philosophy

SGDCORE does not make a claim of being the fastest automatic differentiation system.

Performance depends on implementation details and workload characteristics such as:

* Number of graph nodes
* Graph shape
* Operation count
* Memory allocation
* Tape layout
* Cache behavior
* Backward traversal
* Gradient accumulation
* Cost of mathematical functions
* Compiler optimization
* Hardware

The benchmark suite exists to provide measurements for the implementation and to make performance changes observable.

---

## FAQ

### What is SGDCORE?

SGDCORE is a small C++20 core for scalar computational graphs and reverse-mode automatic differentiation.

### Why is it called SGDCORE?

The name expands to **Differentiable Computational Core**. It refers to the computational core required for differentiable programs rather than specifically to stochastic gradient descent.

### Why reverse-mode autodiff?

Reverse mode can efficiently compute derivatives of many inputs with respect to a scalar output through a single backward traversal.

### Why use a Tape?

The tape provides explicit storage and lifetime for the recorded computational graph.

### What is an adjoint?

An adjoint is the reverse-mode quantity propagated backward through the computational graph.

For a node representing `v`, its adjoint can be interpreted as:

```text
∂output / ∂v
```

for the current scalar output.

### Does SGDCORE use finite differences to calculate gradients?

No.

Finite differences are used for **verification**.

The gradients themselves are computed through reverse-mode automatic differentiation.

### Does SGDCORE support tensors?

Not in the current v0.1 scalar core.

### Does SGDCORE support GPUs?

Not currently.

### Is SGDCORE a machine-learning framework?

No.

It is an automatic differentiation core.

### What C++ standard is required?

C++20.

### How are gradients calculated?

Operations are recorded into a computational graph and the graph is traversed backward using the chain rule.

### How are gradients verified?

The implementation can compare autodiff results against finite-difference approximations.

---

## Contributing

Contributions are welcome.

Useful contributions include:

* New differentiable operations
* Additional test coverage
* Numerical verification
* Edge-case handling
* Documentation improvements
* Performance improvements
* Bug fixes
* Architecture improvements

For a new differentiable operation, include where appropriate:

* Forward-value tests
* Analytical derivative tests
* Finite-difference verification
* Edge-case tests
* Benchmark coverage
* Documentation updates

---

## License

SGDCORE is released under the MIT License.
See [LICENSE](LICENSE) for the full license text.

---

## Final Note

SGDCORE is intentionally small.
The project focuses on one question:

> **What is the smallest clear and testable core required to represent and differentiate a scalar computational graph in modern C++?**

Everything else can be built on top of that foundation.
