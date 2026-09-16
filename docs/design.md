# SGDCORE v0.1 Design

## Goals

- Provide a correct, minimal reverse-mode AD foundation.
- Keep the public API small and explicit.
- Avoid implicit global state, virtual dispatch per node, and unnecessary dependencies.

## Core abstractions

- **NodeId**: lightweight 32-bit handle with a reserved sentinel value.
- **OpCode**: compact 8-bit enumeration of supported operations.
- **Node**: plain struct storing opcode, operand handles, cached forward value, and adjoint.
- **Tape**: owns a contiguous vector of nodes, records operations, and performs backward propagation.
- **Var<T>**: non-owning handle to a tape and a node id.

## Forward evaluation

Every operator records a new node whose forward value is computed immediately using `OperationTraits<OpCode>::forward`. This keeps backward propagation simple because node values are already available.

## Backward propagation

Because nodes are recorded in evaluation order and operands always reference earlier nodes, a single reverse traversal from the root node to node 0 correctly accumulates adjoints. This handles multiple usages automatically.

## Extensibility

New operations can be added by:

1. Extending `OpCode`.
2. Specializing `OperationTraits`.
3. Adding the corresponding `Var<T>` operator or function overload.

## Limitations

v0.1 intentionally excludes tensors, GPU support, multithreading, and neural-network layers.