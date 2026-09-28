#ifndef VECTOR4_BENCHMARK_H_DEFINED
#define VECTOR4_BENCHMARK_H_DEFINED

#include <random>
#include <vector>

#include "Benchmark.hpp"
#include "../Core/Math/Vector/Vector4.h" // adjust to your include paths
#include "../Core/Math/Matrix/Matrix4.h"

// Usage:
//     Vector4Benchmark bench;
//     bench.Run();
//     bench.DisplayResults();
//
// Build once in your SIMD configuration and once with SIMD_FORCE_SCALAR,
// then compare the two outputs line by line.

class Vector4Benchmark : public Benchmark
{
public:
    Vector4Benchmark()
        : Benchmark(Simd::Traits<float>::enabled ? "Vector4<float> [SIMD]" : "Vector4<float> [SCALAR]", 10000)
    {
        GenerateData();
    }

    void Run() override
    {
        // Local copies captured by value in the lambdas below
        float const s  = m_s;
        V const     lo = m_lo;
        V const     hi = m_hi;
        M const&    m  = m_m;

        Section("Arithmetic");
        BenchBinary("operator+(Vector4)",        m_a, m_b, [](V const& x, V const& y) { return x + y; });
        BenchBinary("operator-(Vector4)",        m_a, m_b, [](V const& x, V const& y) { return x - y; });
        BenchUnary ("operator-() (negate)",      m_a,      [](V const& x) { return -x; });
        BenchBinary("operator*(Vector4)",        m_a, m_b, [](V const& x, V const& y) { return x * y; });
        BenchBinary("operator/(Vector4)",        m_a, m_b, [](V const& x, V const& y) { return x / y; });
        BenchUnary ("operator*(float)",          m_a,      [s](V const& x) { return x * s; });
        BenchUnary ("operator/(float)",          m_a,      [s](V const& x) { return x / s; });
        BenchUnary ("operator*(float, Vector4)", m_a,      [s](V const& x) { return s * x; });
        BenchUnary ("operator*(Matrix4)",        m_a,      [&m](V const& x) { return x * m; });

        Section("Compound assignment");
        BenchInPlace("operator+=(Vector4)", m_a, m_b, [](V& x, V const& y) { x += y; });
        BenchInPlace("operator-=(Vector4)", m_a, m_b, [](V& x, V const& y) { x -= y; });
        BenchInPlace("operator*=(Vector4)", m_a, m_b, [](V& x, V const& y) { x *= y; });
        BenchInPlace("operator/=(Vector4)", m_a, m_b, [](V& x, V const& y) { x /= y; });
        BenchInPlace("operator*=(float)",   m_a,      [s](V& x) { x *= s; });
        BenchInPlace("operator/=(float)",   m_a,      [s](V& x) { x /= s; });
        BenchInPlace("operator*=(Matrix4)", m_a,      [&m](V& x) { x *= m; });

        Section("Comparison");
        BenchBinary("operator==",          m_a, m_b, [](V const& x, V const& y) { return x == y; });
        BenchBinary("operator!=",          m_a, m_b, [](V const& x, V const& y) { return x != y; });
        BenchUnary ("IsNull",              m_a,      [](V const& x) { return x.IsNull(); });
        BenchBinary("NearlyEqual",         m_a, m_b, [](V const& x, V const& y) { return V::NearlyEqual(x, y); });
        BenchBinary("NearlyEqual(margin)", m_a, m_b, [](V const& x, V const& y) { return V::NearlyEqual(x, y, 0.5f); });

        Section("Geometry");
        BenchBinary ("Dot (member)",       m_a, m_b, [](V const& x, V const& y) { return x.Dot(y); });
        BenchBinary ("Dot (static)",       m_a, m_b, [](V const& x, V const& y) { return V::Dot(x, y); });
        BenchUnary  ("Length",             m_a,      [](V const& x) { return x.Length(); });
        BenchUnary  ("LengthSquared",      m_a,      [](V const& x) { return x.LengthSquared(); });
        BenchUnary  ("Normalized",         m_a,      [](V const& x) { return x.Normalized(); });
        BenchUnary  ("Normalize (static)", m_a,      [](V const& x) { return V::Normalize(x); });
        BenchInPlace("SelfNormalize",      m_a,      [](V& x) { x.SelfNormalize(); });

        Section("Utility");
        BenchUnary  ("Abs (member)",   m_a,      [](V const& x) { return x.Abs(); });
        BenchUnary  ("Abs (static)",   m_a,      [](V const& x) { return V::Abs(x); });
        BenchUnary  ("Clamp (member)", m_a,      [lo, hi](V const& x) { return x.Clamp(lo, hi); });
        BenchUnary  ("Clamp (static)", m_a,      [lo, hi](V const& x) { return V::Clamp(x, lo, hi); });
        BenchBinary ("Min",            m_a, m_b, [](V const& x, V const& y) { return V::Min(x, y); });
        BenchBinary ("Max",            m_a, m_b, [](V const& x, V const& y) { return V::Max(x, y); });
        // No input: the value is hoisted out of the loop, so these mostly measure a store
        BenchNullary("Zero",           N,        [] { return V::Zero(); });
        BenchNullary("One",            N,        [] { return V::One(); });
    }

private:
    using V = Vector4<float>;
    using M = Matrix4<float>;

    static constexpr size_t N = 1024; // power of two; 16 KB per array stays in cache

    std::vector<V> m_a;  // arbitrary values in [-10, 10]
    std::vector<V> m_b;  // strictly positive values in [10, 30] (safe divisors)
    V     m_lo;          // clamp bounds
    V     m_hi;
    float m_s = 0.0f;    // runtime scalar
    M     m_m;

    void GenerateData()
    {
        // Runtime-generated data: the compiler cannot precompute any result
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> any(-10.0f, 10.0f);
        std::uniform_real_distribution<float> pos(10.0f, 30.0f);

        m_a.resize(N);
        m_b.resize(N);
        for (size_t i = 0; i < N; ++i)
        {
            m_a[i] = V(any(rng), any(rng), any(rng), any(rng));
            m_b[i] = V(pos(rng), pos(rng), pos(rng), pos(rng));
        }

        m_lo = V(-5.0f);
        m_hi = V(5.0f);
        m_s  = pos(rng);

        // Assumes Matrix4 exposes rows[4] as Vector4, as your operator*(Matrix4) uses
        for (int r = 0; r < 4; ++r)
            m_m.rows[r] = V(any(rng), any(rng), any(rng), any(rng));
    }
};

#endif
