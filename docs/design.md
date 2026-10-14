# SGDCORE Design

## Scope

SGDCORE is a C++20 scalar reverse-mode automatic differentiation library. A
`Tape` stores `Node` records in a vector. `Var<T>` is a non-owning handle to a
node. Each operation computes its forward value immediately and records parent
IDs and an opcode. Backward propagation visits nodes in descending ID order
and accumulates adjoints into earlier parents.

The design is intentionally scalar-first. It does not include tensors, GPU
support, neural networks, optimizers, or higher-order differentiation.

## Core types

| Type | Purpose |
|------|---------|
| `NodeId` | 32-bit index into one tape; the maximum value is reserved as invalid. |
| `OpCode` | Identifies the operation stored in a node. |
| `Node` | Stores an operation, parent IDs, forward value, and adjoint. |
| `Tape` | Owns nodes, validates recording and access, and runs backward propagation. |
| `Var<T>` | Non-owning tape pointer plus node ID and reset generation. |
| `OperationTraits<OpCode>` | Defines forward and local derivative formulas. |

## Graph invariants

`Tape::record()` is the low-level public graph-construction API. It validates
that parent IDs refer to existing nodes, that operation arity matches the
opcode, and that parent nodes precede their result. This ordering makes a
reverse pass over descending IDs a valid topological traversal. Node reads are
checked and returned by value so callers cannot mutate graph edges or retain
references invalidated by vector growth. Validation happens before appending;
the vector's strong exception guarantee leaves the graph unchanged if node
allocation fails. Literal overloads reserve both required nodes before adding
either. `Tape::reserve_nodes()` exposes the same capacity planning for callers
constructing graphs in batches.

Arithmetic operators accept built-in scalar literals on either side of a
variable. The literal is stored as a constant node on that variable's tape and
converted to `double`; no tape is constructed implicitly. Variable-to-variable
binary operations still require matching `Var<T>` types and a shared tape.

## Lifetime and reset

A `Var<T>` stores a raw `Tape*`; it does not own the tape and cannot detect its
destruction. The caller must keep each tape alive for as long as any associated
handle is used. Copying and moving `Tape` are disabled to keep its address
stable.

Reset clears all nodes and increments a generation counter. A handle records
the generation at construction and every use checks it. Thus reset handles
throw even if later recording reuses their numeric node IDs. This mechanism
cannot protect a handle after the tape itself has been destroyed.

Default handles are invalid. Cross-tape operations are rejected. Checked node
access, invalid roots, stale handles, and invalid recording requests fail with
standard exceptions (`std::logic_error`, `std::out_of_range`, or
`std::invalid_argument`, according to the violated contract).

## Values and mutation

Operation results are cached during recording. `Var::set_value()` is allowed
only on a variable leaf that has no dependent operation recorded yet. It
throws for constants, computed nodes, or variables already referenced by an
operation. Successful mutation clears adjoints. To evaluate a changed input
through an existing expression, create a new tape and record that expression
again.

Every backward call clears adjoints, seeds its root with one, then accumulates
local derivative contributions into parents. This means repeated backward
calls produce a fresh result rather than accumulating gradients across calls.
The traversal also tracks graph reachability separately from numeric adjoints:
all ancestors are processed even when their accumulated adjoint is zero, while
unrelated recorded branches are skipped. This preserves NaN signals from
reachable invalid derivatives without contaminating unrelated graph branches.

## Floating-point behavior

Operation forward and derivative rules use `double` arithmetic and the C++
`<cmath>` functions. The library does not clip inputs or throw custom domain
errors. In particular, the variable-exponent derivative for `pow(x, y)` uses
`x^y * log(x)`: for nonpositive bases, IEEE/platform math behavior can produce
NaN or infinity even when a negative base and integer-valued exponent produce
a real forward value. For `log(x)` at negative x, the local derivative is
explicitly NaN instead of the finite `1/x`; this avoids presenting a finite
number as a valid real-domain gradient. At zero, the standard boundary
derivatives of `log` and `sqrt` are positive infinity. Other singularities
follow the evaluated floating-point formulas. Domain and boundary behavior is
documented in the README and API reference and tested on common IEEE-754
targets.

The finite-difference helper requires a positive finite step. The appropriate
step magnitude depends on the function and input scale.

## Thread safety

Tape operations mutate node storage or adjoints and are not synchronized.
Concurrent access to one tape or its handles is unsupported. Independent tapes
can be used concurrently if no tape or handle is shared between threads.

## Build structure

The default CMake configuration builds only `sgdcore` and performs no
third-party downloads. Tests, benchmarks, and examples are opt-in through
`SGDCORE_BUILD_TESTS`, `SGDCORE_BUILD_BENCHMARKS`, and
`SGDCORE_BUILD_EXAMPLES`. Test and benchmark dependencies are fetched from
fixed upstream commit IDs when those options are enabled.
