# SGDCORE v0.1

A minimal, header-friendly C++20 reverse-mode automatic differentiation core.

## Features

- Tape-based reverse-mode AD with explicit tape ownership
- No global tape, no virtual functions per node
- Small public API: `Tape`, `Var<T>`, `variable`, `constant`, `backward`, `gradient`
- Arithmetic: `+`, `-`, `*`, `/`, unary `-`
- Elementary: `sin`, `cos`, `tan`, `exp`, `log`, `sqrt`, `pow`
- Finite-difference verification utilities
- Tests with GoogleTest, benchmarks with Google Benchmark
- Continuous integration with GitHub Actions

## Quick start

```cpp
#include <sgdcore/sgdcore.hpp>
#include <iostream>

int main() {
    sgdcore::Tape tape;
    auto x = sgdcore::variable(2.0, tape);
    auto y = sgdcore::variable(3.0, tape);
    auto z = x * y + sgdcore::sin(x);

    sgdcore::backward(z);
    std::cout << "dz/dx = " << sgdcore::gradient(x) << "\n";
}
```

## Build

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Test

```bash
cmake -B build -S . -DSGDCORE_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Benchmark

```bash
cmake -B build -S . -DSGDCORE_BUILD_BENCHMARKS=ON
cmake --build build -j
./build/benchmarks/sgdcore_benchmark
```

## Upload to GitHub

```bash
chmod +x setup_repo.sh
./setup_repo.sh https://github.com/YOUR_USERNAME/SGDCORE.git
```

## License

MIT. See [LICENSE](LICENSE).