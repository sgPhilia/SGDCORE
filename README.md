# SGDCORE

### Differentiable Computational Core

SGDCORE is a C++20 library for scalar reverse-mode automatic differentiation. It records scalar computations on an explicit `Tape`, then computes derivatives by traversing the graph backward from an output. It is aimed at users who want a small scalar autodiff library for learning, numerical experiments, and compact C++ applications.

SGDCORE does not provide tensors, GPU support, a neural-network framework, an optimizer subsystem, or higher-order differentiation.

## Features

SGDCORE supports the following operations. For binary operations, the derivatives are with respect to the left and right operands, respectively.

| Operation | Forward value | Local derivative(s) |
| --- | --- | --- |
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

`variable(value, tape)` creates an input and `constant(value, tape)` creates a fixed graph leaf. Both accept built-in arithmetic types (`std::is_arithmetic_v<T>`); graph values and adjoints, and the results of `value()` and `gradient()`, use `double`. Variable-to-variable operations require the same `Var<T>` type and variables on the same tape. Arithmetic operators and `pow` also accept built-in arithmetic literals on either side; each literal is converted to `double` and recorded as a constant on the variable's tape.

The public types are `Tape`, `Var<T>`, `Node`, `NodeId`, and `OpCode`. Handles expose `is_valid()`, `value()`, `adjoint()`, and guarded `set_value()`. A tape exposes `record()`, checked by-value `node()` access, `size()`, `reserve_nodes()`, `backward(NodeId)`, `clear_adjoints()`, and `reset()`; convenience functions include `backward(Var)`, `value()`, `gradient()`, `clear_adjoints(Tape)`, and `reset(Tape)`. For derivative checks, `central_difference(f, x, h)` computes `[f(x+h) - f(x-h)] / (2h)` (default `h = 1e-5`), and `close_enough(a, b, rel_tol, abs_tol)` compares values using relative and absolute tolerances.

## Quick start

```cpp
#include <sgdcore/sgdcore.hpp>
#include <iostream>

int main() {
    sgdcore::Tape tape;
    auto x = sgdcore::variable(2.0, tape);
    auto y = sgdcore::variable(3.0, tape);
    auto z = x * y + sgdcore::sin(x) + 2.0;

    sgdcore::backward(z);
    std::cout << "z = " << sgdcore::value(z) << '\n';
    std::cout << "dz/dx = " << sgdcore::gradient(x) << '\n';
    std::cout << "dz/dy = " << sgdcore::gradient(y) << '\n';
}
```

Expected output:

```text
z = 8.9093
dz/dx = 2.58385
dz/dy = 2
```

`backward(z)` seeds the output derivative to one. `gradient(x)` returns the derivative of `z` with respect to `x` after that pass.

## Build and development

Requirements are CMake 3.15 or newer and a C++20 compiler. The default configure builds only the core library and does not fetch third-party dependencies:

```bash
cmake -S . -B build
cmake --build build
```

Tests, examples, and benchmarks are optional and default to `OFF`:

| Option | Enables |
| --- | --- |
| `SGDCORE_BUILD_TESTS` | GoogleTest unit tests |
| `SGDCORE_BUILD_EXAMPLES` | Four standalone usage examples |
| `SGDCORE_BUILD_BENCHMARKS` | Google Benchmark suite |

Enabling tests fetches GoogleTest; enabling benchmarks fetches Google Benchmark. Both are pinned to upstream commit IDs and may require Git and network access during configuration. Examples have no fetched dependency. A core-only configure/build is suitable for offline use.

### Tests

Enable the test option explicitly before building and running CTest:

```bash
cmake -S . -B build-tests -DSGDCORE_BUILD_TESTS=ON
cmake --build build-tests
ctest --test-dir build-tests --output-on-failure
```

The tests cover tape and handle validity, operations and gradients, floating-point boundaries, repeated backward passes, and finite-difference comparisons. The installed-package consumer is also built and tested by the [CI workflow](.github/workflows/ci.yml).

For a multi-configuration generator, pass `--config Release` to the build and `-C Release` to CTest.

### Examples

```bash
cmake -S . -B build-examples -DSGDCORE_BUILD_EXAMPLES=ON
cmake --build build-examples
./build-examples/examples/sgdcore_example_simple
./build-examples/examples/sgdcore_example_chain
./build-examples/examples/sgdcore_example_finite_difference
./build-examples/examples/sgdcore_example_gradient_descent
```

The examples demonstrate:

- `simple`: build an expression and read its value and gradients; output matches the quick-start example.
- `chain`: compose a short repeated operation chain; prints `y = 1.30825` and `dy/dx = 0.00332989`.
- `finite_difference`: compare autodiff and central differences for `sin(x) * exp(x)`; both derivatives are near `2.2373281198`.
- `gradient_descent`: minimize `(x - 3)^2` with ten plain gradient-descent steps; final `x` is `2.969767`.

For multi-configuration generators, build with `cmake --build build-examples --config Release`; executables are under `examples/Release/` (or the selected configuration directory). The shell executable paths above show the single-configuration Unix layout.

### Benchmarks

```bash
cmake -S . -B build-benchmarks -DSGDCORE_BUILD_BENCHMARKS=ON
cmake --build build-benchmarks
./build-benchmarks/benchmarks/sgdcore_benchmark
```

The benchmark suite measures tape recording, backward propagation, adjoint clearing, and reset across linear, multiplication, mixed-operation, and unary-heavy graphs. Benchmark numbers depend on the machine and build configuration; none are claimed here. For multi-configuration generators, select a configuration with `--config Release` and run the executable from the corresponding `benchmarks/Release/` directory.

### Sanitizers

With GCC or Clang, AddressSanitizer and UndefinedBehaviorSanitizer can be enabled for a test build using compiler flags:

```bash
cmake -S . -B build-sanitize -DSGDCORE_BUILD_TESTS=ON \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build-sanitize
ctest --test-dir build-sanitize --output-on-failure
```

These flags are compiler-specific and are not intended for MSVC. The CI workflow does not configure sanitizer builds.

## Install and use from another CMake project

Install to a custom prefix (the command below uses a Unix-like shell):

```bash
cmake -S . -B build-install -DCMAKE_INSTALL_PREFIX="$PWD/install"
cmake --build build-install
cmake --install build-install
```

The installed CMake package is `SGDCORE` (project version `0.1.0`) and exports `SGDCORE::sgdcore`:

```cmake
find_package(SGDCORE 0.1 CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE SGDCORE::sgdcore)
```

For a non-system prefix, configure the downstream project with `-DCMAKE_PREFIX_PATH=/path/to/install`. The imported target provides the public include directory and propagates the C++20 compile requirement. The library has no third-party runtime dependency.

## API and numerical contracts

A `Var<T>` is a non-owning handle; keep its `Tape` alive for every use. The raw tape pointer cannot detect tape destruction, so using a handle after its tape is destroyed is invalid. `Tape::reset()` invalidates all existing handles. Default or stale handles throw `std::logic_error`, out-of-range IDs and roots throw `std::out_of_range`, and invalid operations or cross-tape operands throw `std::invalid_argument`.

Forward values are cached when operations are recorded. `set_value()` may change only a variable leaf that has no recorded dependent operation; constants, computed nodes, and referenced leaves cannot be changed. Rebuild the graph on a fresh tape to evaluate changed inputs. Each `backward()` clears previous adjoints and computes a fresh set of gradients. Tapes and handles are not synchronized for concurrent access; use independent tapes across threads.

Evaluation uses `double` and standard C++ math behavior, without clipping or custom domain exceptions. Division by zero may produce infinity or NaN. `log(0)` is negative infinity with a positive infinite derivative; negative inputs to `log` produce NaN forward values and derivatives. Negative inputs to `sqrt` produce NaN, and `sqrt(0)` has a positive infinite derivative. `pow` follows `std::pow`; with a variable exponent, its derivative is `x^y * log(x)`, which is generally not a finite real derivative for nonpositive bases. A negative base with an integer-valued exponent may still have a real forward value and base derivative. Reachable invalid derivative paths propagate NaN even with a zero incoming adjoint; unrelated branches do not contribute. `central_difference()` requires a positive finite step, and the useful magnitude depends on the function and input scale.

See the [API reference](docs/api.md) for complete overload, exception, and edge-case details, and [design notes](docs/design.md) for graph and implementation invariants.

## Repository layout

- `include/sgdcore/` — public headers
- `src/tape.cpp` — tape recording and reverse propagation
- `tests/` — unit tests and installed-package consumer
- `examples/` — scalar expression, chain, finite-difference, and gradient-descent examples
- `benchmarks/` — tape and backward-pass benchmark cases
- `docs/api.md` — API and numerical behavior
- `docs/design.md` — implementation design

## License

MIT; see [LICENSE](LICENSE).
