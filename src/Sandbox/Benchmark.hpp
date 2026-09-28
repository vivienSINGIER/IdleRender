#ifndef BENCHMARK_H_DEFINED
#define BENCHMARK_H_DEFINED

#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Disables the compiler's auto-vectorizer on the timed loops, so the benchmark
// measures your code per operation instead of what the compiler rewrote it into.
#if defined(_MSC_VER) && !defined(__clang__)
    #define BENCH_NO_VECTORIZE __pragma(loop(no_vector))
#elif defined(__clang__)
    #define BENCH_NO_VECTORIZE _Pragma("clang loop vectorize(disable) interleave(disable)")
#else
    #define BENCH_NO_VECTORIZE
#endif

// Generic micro-benchmark base class.
//
// Derive from it, override Run(), and call the Bench* helpers on arrays of
// inputs generated at runtime. Each helper times `reps` passes over the whole
// array, repeats that `samples` times, and records the BEST average cost of
// one operation in nanoseconds (the run with the least outside interference).
//
// Rules the helpers enforce so the optimizer can't cheat:
//  - input arrays must have a power-of-two size (used as an index mask)
//  - each pass reads the inputs at an offset that depends on the pass number,
//    so passes are different work and can't be merged
//  - every result is written to an output buffer (independent iterations,
//    no dependency chain) and each pass is consumed through a volatile sink
//
// Rule the helpers follow so MSVC generates a clean loop:
//  - everything the loop needs (data pointers, count, mask, the operation)
//    is copied into LOCAL variables before the loop. MSVC has no type-based
//    alias analysis: anything reached through memory (a reference capture,
//    a std::vector's internal pointer) is reloaded after every store.
//    Locals whose address is never taken can stay in registers.
class Benchmark
{
public:
    struct Result
    {
        std::string name;
        double      nsPerOp   = 0.0;
        bool        isSection = false;
    };

    explicit Benchmark(std::string _name, int _reps = 10000, int _samples = 3)
        : m_name(std::move(_name)), m_reps(_reps), m_samples(_samples)
    {
        assert(m_reps > 0 && m_samples > 0);
    }

    virtual ~Benchmark() = default;

    virtual void Run() {}

    void SetVerbose(bool _verbose) { m_isVerbose = _verbose; }

    void DisplayResults() const
    {
        // Plain comparison instead of std::max: <windows.h> defines min/max
        // macros that break std::max / std::min calls
        size_t width = 0;
        for (Result const& r : m_results)
            if (!r.isSection && r.name.size() > width)
                width = r.name.size();

        std::cout << "\033[96m" << m_name << "\033[0m  ("
                  << m_reps << " passes, best of " << m_samples << ")\n";

        for (Result const& r : m_results)
        {
            if (r.isSection)
            {
                std::cout << "\n\033[93m" << r.name << "\033[0m\n";
                continue;
            }

            std::cout << "  " << std::left  << std::setw(int(width) + 2) << r.name
                      << std::right << std::fixed << std::setprecision(3)
                      << std::setw(10) << r.nsPerOp << " ns/op\n";
        }
        std::cout << '\n';
    }

    std::vector<Result> const& GetResults() const { return m_results; }

protected:
    ///////////////////////////////////////////////////////////////////////////
    // Grouping

    void Section(const char* _name)
    {
        m_results.push_back({ _name, 0.0, true });
        if (m_isVerbose)
            std::cout << "\n" << _name << std::endl;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Methods returning a value: result = op()

    template <typename Op>
    void BenchNullary(const char* _name, size_t _count, Op _op)
    {
        using R = std::decay_t<std::invoke_result_t<Op&>>;
        BenchLoop<R>(_name, _count, [_op](size_t, size_t) { return _op(); });
    }

    ///////////////////////////////////////////////////////////////////////////
    // Methods returning a value: result = op(a)

    template <typename A, typename Op>
    void BenchUnary(const char* _name, std::vector<A> const& _a, Op _op)
    {
        using R = std::decay_t<std::invoke_result_t<Op&, A const&>>;

        // Raw pointer captured BY VALUE: no reload through the vector each iteration
        A const* pa = _a.data();
        BenchLoop<R>(_name, _a.size(), [pa, _op](size_t, size_t j) { return _op(pa[j]); });
    }

    ///////////////////////////////////////////////////////////////////////////
    // Methods returning a value: result = op(a, b)

    template <typename A, typename B, typename Op>
    void BenchBinary(const char* _name, std::vector<A> const& _a, std::vector<B> const& _b, Op _op)
    {
        assert(_a.size() == _b.size() && "Input arrays must have the same size");
        using R = std::decay_t<std::invoke_result_t<Op&, A const&, B const&>>;

        A const* pa = _a.data();
        B const* pb = _b.data();
        BenchLoop<R>(_name, _a.size(), [pa, pb, _op](size_t i, size_t j) { return _op(pa[i], pb[j]); });
    }

    ///////////////////////////////////////////////////////////////////////////
    // Methods modifying the object: copy = a; op(copy)
    // The copy exists in every build, so it doesn't bias comparisons.
    // It also keeps values stable across passes (no drift to inf / denormals).

    template <typename A, typename Op>
    void BenchInPlace(const char* _name, std::vector<A> const& _a, Op _op)
    {
        size_t const count = _a.size();
        size_t const mask  = CheckedMask(count);
        std::vector<A> out(count);

        Measure(_name, count, [&](size_t r)
        {
            // Locals: loaded once per pass, kept in registers during the loop
            A const* const src = _a.data();
            A*       const dst = out.data();
            size_t   const n   = count;
            size_t   const m   = mask;
            Op             op  = _op;

            BENCH_NO_VECTORIZE
            for (size_t i = 0; i < n; ++i)
            {
                dst[i] = src[(i + r) & m];
                op(dst[i]);
            }
            Sink(dst[r & m]);
        });
    }

    ///////////////////////////////////////////////////////////////////////////
    // Methods modifying the object: copy = a; op(copy, b)

    template <typename A, typename B, typename Op>
    void BenchInPlace(const char* _name, std::vector<A> const& _a, std::vector<B> const& _b, Op _op)
    {
        assert(_a.size() == _b.size() && "Input arrays must have the same size");
        size_t const count = _a.size();
        size_t const mask  = CheckedMask(count);
        std::vector<A> out(count);

        Measure(_name, count, [&](size_t r)
        {
            A const* const srcA = _a.data();
            B const* const srcB = _b.data();
            A*       const dst  = out.data();
            size_t   const n    = count;
            size_t   const m    = mask;
            Op             op   = _op;

            BENCH_NO_VECTORIZE
            for (size_t i = 0; i < n; ++i)
            {
                dst[i] = srcA[i];
                op(dst[i], srcB[(i + r) & m]);
            }
            Sink(dst[r & m]);
        });
    }

private:
    std::string         m_name;
    int                 m_reps;
    int                 m_samples;
    std::vector<Result> m_results;
    bool                m_isVerbose = true;

    inline static volatile unsigned char s_sink = 0;

    static size_t CheckedMask(size_t _count)
    {
        assert(_count > 0 && (_count & (_count - 1)) == 0 && "Input size must be a power of two");
        return _count - 1;
    }

    // Folds every byte of the value into a volatile write.
    // A volatile write is an observable side effect: the compiler must
    // compute the whole value to produce it.
    template <typename T>
    static void Sink(T const& _value)
    {
        static_assert(std::is_trivially_copyable_v<T>, "Sink requires a trivially copyable type");
        unsigned char bytes[sizeof(T)];
        std::memcpy(bytes, &_value, sizeof(T));

        unsigned char acc = 0;
        for (unsigned char b : bytes)
            acc ^= b;
        s_sink = acc;
    }

    // One warm-up pass, then `m_samples` timed runs of `m_reps` passes each.
    // Keeps the fastest run: interference (other processes, frequency changes)
    // can only make a run slower, never faster, so the minimum is the cleanest.
    template <typename Pass>
    void Measure(const char* _name, size_t _count, Pass&& _pass)
    {
        _pass(0); // warm-up: caches, branch predictor, CPU clock ramp-up

        double best = -1.0;
        for (int s = 0; s < m_samples; ++s)
        {
            auto start = std::chrono::steady_clock::now();
            for (int r = 0; r < m_reps; ++r)
                _pass(size_t(r));
            auto end = std::chrono::steady_clock::now();

            double ns = std::chrono::duration<double, std::nano>(end - start).count()
                      / (double(_count) * m_reps);

            if (best < 0.0 || ns < best)
                best = ns;
        }

        m_results.push_back({ _name, best, false });

        if (m_isVerbose)
            std::cout << "  " << _name << ": " << std::fixed << std::setprecision(3)
                      << best << " ns/op" << std::endl; // endl flushes, so it shows immediately
    }

    // Shared loop for everything that returns a value.
    // gen(i, j): i = plain index, j = index offset by the pass number.
    //
    // Every result is written to its own slot in an output buffer, so
    // iterations are independent and the CPU can overlap them. (Summing into
    // one accumulator would chain every iteration to the previous one and
    // measure the latency of the addition instead of the method.)
    template <typename R, typename Gen>
    void BenchLoop(const char* _name, size_t _count, Gen _gen)
    {
        size_t const mask = CheckedMask(_count);

        // bool gets a 1-byte buffer: std::vector<bool> packs bits, which would
        // add bit manipulation to every store
        using Stored = std::conditional_t<std::is_same_v<R, bool>, unsigned char, R>;
        std::vector<Stored> out(_count);

        Measure(_name, _count, [&](size_t r)
        {
            Stored* const dst = out.data();
            size_t  const n   = _count;
            size_t  const m   = mask;
            Gen           gen = _gen; // local copy: its captured pointers can live in registers

            BENCH_NO_VECTORIZE
            for (size_t i = 0; i < n; ++i)
                dst[i] = static_cast<Stored>(gen(i, (i + r) & m));

            Sink(dst[r & m]);
        });
    }
};

#endif
