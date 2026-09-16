# SGDCORE Design

## What SGDCORE is

SGDCORE is a small C++20 library for scalar reverse-mode automatic
differentiation. It records a computational graph into an explicit `Tape`,
then walks that graph backward to compute gradients.

The project is intentionally minimal. It does not include tensors, neural
networks, GPU support, expression templates, or multithreading.

## Core types

| Type | Purpose |
|------|---------|
| `NodeId` | 32-bit handle to a node in the tape. |
| `OpCode` | Tag identifying the operation stored in a node. |
| `Node` | POD-like struct: opcode, two parent ids, forward value, adjoint. |
| `Tape` | Owns a `std::vector<Node>` and runs the backward pass. |
| `Var<T>` | Non-owning handle to a node in a specific `Tape`. |
| `OperationTraits<OpCode>` | Forward and derivative functions for each operation. |

## Tape ownership

Every `Var<T>` stores a pointer to the `Tape` that owns its node. This makes
dependencies explicit and prevents accidental use of a `Var` after its tape
has been destroyed.

## Forward pass

During expression evaluation, each operator creates a new node. The node's
forward value is computed immediately by `OperationTraits<OpCode>::forward`.
Values are cached so the backward pass does not recompute them.

Example:

```cpp
auto x = sgdcore::variable(2.0, tape);
auto y = sgdcore::variable(3.0, tape);
auto z = x * y + sgdcore::sin(x);
