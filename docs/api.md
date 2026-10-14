# SGDCORE API Reference

Include the public API with:

```cpp
#include <sgdcore/sgdcore.hpp>
```

All public symbols are in namespace `sgdcore`.

## Scalar types

`variable(value, tape)` and `constant(value, tape)` accept types for which
`std::is_arithmetic_v<T>` is true. They return `Var<T>`, but every node stores
its value and adjoint as `double`; values are converted to `double` when
recorded. Binary operators require both operands to have the same `Var<T>`
type. For consistent behavior and precision, use `double` inputs unless
interoperating with an existing arithmetic type is useful.

`Var<T>` handles scalar variables and constants. Constants are included in
forward expressions but do not represent independent inputs. Derivatives are
obtained for variable nodes by reading their adjoints.

`T` must be a built-in arithmetic type. A graph stores values and adjoints as
`double`, including when `T` is `float` or an integer type. Binary `Var` to
`Var` operations require the same `T`; mixed `Var<T>` types are intentionally
not implicitly converted.

## Typical use

```cpp
sgdcore::Tape tape;
auto x = sgdcore::variable(2.0, tape);
auto y = sgdcore::variable(3.0, tape);
auto offset = sgdcore::constant(1.0, tape);
auto output = x * y + sgdcore::sin(x) + offset;

sgdcore::backward(output);
double result = sgdcore::value(output);
double d_output_d_x = sgdcore::gradient(x);
double d_output_d_y = sgdcore::gradient(y);
```

`backward(output)` seeds the output adjoint to one. Each call clears previous
adjoints first. `gradient(variable)` is the derivative of the most recent
backward root with respect to that variable; variables outside the root's
ancestor graph have zero adjoints.

## Operations

| API | Forward value | Local derivatives |
|-----|---------------|-------------------|
| `a + b` | `a + b` | `(1, 1)` |
| `a - b` | `a - b` | `(1, -1)` |
| `a * b` | `a * b` | `(b, a)` |
| `a / b` | `a / b` | `(1/b, -a/b²)` |
| `-a` | `-a` | `-1` |
| `sin(a)` | sine | `cos(a)` |
| `cos(a)` | cosine | `-sin(a)` |
| `tan(a)` | tangent | `1 + tan(a)²` |
| `exp(a)` | exponential | `exp(a)` |
| `log(a)` | natural logarithm | `1/a` for `a > 0` |
| `sqrt(a)` | square root | `1/(2 sqrt(a))` |
| `pow(a, b)` | power | `(b a^(b-1), a^b log(a))` |

Binary `Var` operands and `pow(Var, Var)` require variables recorded on the
same tape. The arithmetic operators also accept built-in arithmetic scalar
literals on either side, such as `x + 2.0`, `2.0 * x`, or `3 - x`. These
overloads record the literal as a constant on the variable's tape and convert
it directly to `double`; they do not change the `Var<T>` type or narrow the
literal through `T`. `pow` accepts a scalar base or exponent by the same rule.
No tape is created or owned implicitly. Mixed `Var<T>` types remain a compile-
time error; explicitly choose a common type when combining variables.

## Errors, handles, and mutation

`Var<T>` is a non-owning handle. Its `Tape` must stay alive for every use of the
handle. Since handles contain a raw `Tape*`, using a handle after tape
destruction cannot be detected and is invalid. `Tape` is neither copyable nor
movable. `Tape::reset()` invalidates all existing handles; subsequent use throws
`std::logic_error`, even if a new node reuses the same ID.

Default handles throw `std::logic_error` when used. Out-of-range node IDs and
backward roots throw `std::out_of_range`. Invalid operation arity or opcode,
and cross-tape variable combinations, throw `std::invalid_argument`.

`Var::set_value()` can change only a variable leaf that has not yet been used
by a recorded operation. Constants, computed values, and referenced variable
leaves throw `std::logic_error`. A successful change clears adjoints. To change
an input used by a graph, create a fresh tape and record the expression again.

The lower-level `Tape::record()` accepts tape-local parent IDs. Parents must
already exist and match the opcode's arity. `Tape::node()` checks the ID and
returns a copy of the node, not a mutable reference into the graph.

### Compatibility notes

For handle safety, `Tape` is now non-copyable and non-movable, and `Tape::node()`
no longer provides mutable access (or a reference that can be invalidated by
vector growth). Code that modified `tape.node(id).adjoint` must use the public
gradient APIs instead. `Var::set_value()` also now throws after a dependent
operation has been recorded; rebuild the graph on a fresh tape to use changed
inputs.

## Floating-point policy

All evaluation uses `double` arithmetic and the C++ standard math functions.
The library does not clamp inputs or throw domain-specific exceptions. On
common IEEE-754 platforms:

- Division by zero produces an infinity or NaN according to the operands.
- `log(0)` produces negative infinity and a positive infinite local derivative.
- `log(x)` for `x < 0` produces NaN, and its local derivative is explicitly
  NaN so a finite `1/x` cannot be mistaken for a valid gradient outside the
  real logarithm domain.
- `sqrt(x)` for `x < 0` produces NaN; at zero its derivative is positive
  infinity.
- `pow(x, y)` follows `std::pow`. A negative base with an integer-valued
  exponent can have a real forward value and a real base derivative, while
  the derivative with respect to a variable exponent is NaN because it uses
  `x^y log(x)`. At a zero base, derivative results depend on the exponent and
  usual floating-point rules.

Backward propagation visits only ancestors of the requested root. It computes
their local derivatives even where an accumulated adjoint is numerically zero,
so a reachable NaN derivative is not hidden by a zero multiplier. Unrelated
recorded branches do not contribute. At singular or invalid points, check
results using `std::isfinite`, `std::isinf`, and `std::isnan`; finite-difference
checks should only be used in smooth domains.

`central_difference(f, x, h)` requires a positive, finite `h` and throws
`std::invalid_argument` otherwise. The caller selects a useful step magnitude
for the scale and conditioning of `f` near `x`.

## Thread safety

`Tape` and its `Var` handles are mutable and have no internal synchronization.
Do not access the same tape or its handles concurrently when any access may
record nodes, reset the tape, mutate a value, or run backward. Separate tapes
can be used concurrently when each thread owns and accesses only its own tape
and handles.

## Recording and memory

`Tape::record()` validates all arguments before appending a node. If vector
allocation fails while appending, the tape remains unchanged. Scalar-literal
expressions reserve space for both their constant and result before recording
either node. `Tape::reserve_nodes(n)` can reserve capacity for `n` additional
nodes without changing the graph. Node IDs are
32-bit, with the maximum ID reserved as invalid; recording throws
`std::overflow_error` when the tape reaches its representable node limit.
`reset()` invalidates handles by incrementing a 64-bit generation. Generation
wraparound would require 2^64 resets of the same tape and is not treated as a
supported lifecycle.
