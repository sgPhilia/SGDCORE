#include <sgdcore/sgdcore.hpp>

#include <benchmark/benchmark.h>

using namespace sgdcore;

namespace {

Var<double> build_linear(Tape& tape, std::size_t n) {
    auto x = variable(1.0, tape);
    auto c = constant(1.0, tape);
    auto y = x;
    for (std::size_t i = 0; i < n; ++i) {
        y = y + c;
    }
    return y;
}

Var<double> build_multiply_chain(Tape& tape, std::size_t n) {
    auto x = variable(1.001, tape);
    auto y = x;
    for (std::size_t i = 0; i < n; ++i) {
        y = y * x;
    }
    return y;
}

Var<double> build_mixed(Tape& tape, std::size_t n) {
    auto x = variable(0.5, tape);
    auto c = constant(0.7, tape);
    auto y = x;
    for (std::size_t i = 0; i < n; ++i) {
        y = sin(y) + y * c;
    }
    return y;
}

Var<double> build_unary_heavy(Tape& tape, std::size_t n) {
    auto x = variable(0.1, tape);
    auto y = x;
    for (std::size_t i = 0; i < n; ++i) {
        y = exp(sin(cos(y)));
    }
    return y;
}

} // namespace

#define SGDCORE_BM_RECORD(NAME, BUILDER)                              \
    static void BM_Record##NAME(benchmark::State& state) {            \
        const auto n = static_cast<std::size_t>(state.range(0));      \
        for (auto _ : state) {                                        \
            Tape tape;                                                \
            auto y = BUILDER(tape, n);                                \
            benchmark::DoNotOptimize(y.value());                      \
        }                                                             \
        state.SetItemsProcessed(n * state.iterations());              \
    }                                                                 \
    BENCHMARK(BM_Record##NAME)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

#define SGDCORE_BM_BACKWARD(NAME, BUILDER)                            \
    static void BM_Backward##NAME(benchmark::State& state) {          \
        const auto n = static_cast<std::size_t>(state.range(0));      \
        Tape tape;                                                    \
        Var<double> y;                                                \
        state.PauseTiming();                                          \
        y = BUILDER(tape, n);                                         \
        state.ResumeTiming();                                         \
        for (auto _ : state) {                                        \
            tape.backward(y.node());                                  \
            benchmark::DoNotOptimize(tape.node(y.node()).adjoint);    \
        }                                                             \
        state.SetItemsProcessed(n * state.iterations());              \
    }                                                                 \
    BENCHMARK(BM_Backward##NAME)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

SGDCORE_BM_RECORD(Linear, build_linear)
SGDCORE_BM_RECORD(Multiply, build_multiply_chain)
SGDCORE_BM_RECORD(Mixed, build_mixed)
SGDCORE_BM_RECORD(Unary, build_unary_heavy)

SGDCORE_BM_BACKWARD(Linear, build_linear)
SGDCORE_BM_BACKWARD(Multiply, build_multiply_chain)
SGDCORE_BM_BACKWARD(Mixed, build_mixed)
SGDCORE_BM_BACKWARD(Unary, build_unary_heavy)

static void BM_ClearAdjoints(benchmark::State& state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    Tape tape;
    auto y = build_mixed(tape, n);
    tape.backward(y.node());

    for (auto _ : state) {
        tape.clear_adjoints();
        benchmark::DoNotOptimize(tape.node(y.node()).adjoint);
    }
    state.SetItemsProcessed(n * state.iterations());
}
BENCHMARK(BM_ClearAdjoints)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

static void BM_Reset(benchmark::State& state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    for (auto _ : state) {
        Tape tape;
        build_mixed(tape, n);
        tape.reset();
        benchmark::DoNotOptimize(tape.size());
    }
    state.SetItemsProcessed(n * state.iterations());
}
BENCHMARK(BM_Reset)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);