#ifndef SIMD_TRAITS_SSE_HPP
#define SIMD_TRAITS_SSE_HPP

#include "SimdTraits.h"

namespace Simd
{
    
template <>
struct Traits<float32>
{
    static constexpr bool enabled = true;
    static constexpr bool isFloating = true;
    static constexpr size_t alignment = 16;
    static constexpr size_t width = 4;
    using Reg = __m128;

    static Reg  Load(float const* _p)       { return _mm_load_ps(_p); }
    static Reg  LoadU(float const* _p)      { return _mm_loadu_ps(_p); }
    static void Store(float* _p, Reg _v)    { _mm_store_ps(_p, _v); }
    static void StoreU(float* _p, Reg _v)   { _mm_storeu_ps(_p, _v); }
    
    static Reg Zero()                                      { return _mm_set1_ps(0.0f); }
    static Reg One()                                       { return _mm_set1_ps(1.0f); }
    static Reg Set(float _x, float _y, float _z, float _w) { return _mm_set_ps(_w, _z, _y, _x); }
    static Reg Set1(float _s)                              { return _mm_set1_ps(_s); }
    static Reg SetW0(float _x, float _y, float _z)         { return _mm_set_ps(0.0f, _z, _y, _x); }
    static Reg SetW1(float _x, float _y, float _z)         { return _mm_set_ps(1.0f, _z, _y, _x); }
    
    static Reg Add(Reg _x, Reg _y)                  { return _mm_add_ps(_x, _y); }
    static Reg Sub(Reg _x, Reg _y)                  { return _mm_sub_ps(_x, _y); }
    static Reg Mul(Reg _x, Reg _y)                  { return _mm_mul_ps(_x, _y); }
    static Reg Div(Reg _x, Reg _y)                  { return _mm_div_ps(_x, _y); }
    static Reg Neg(Reg _x)                          { Reg mask = _mm_set1_ps(-0.0f); return _mm_xor_ps(mask, _x); }
    static Reg MulAdd(Reg _x, Reg _y, Reg _z)       { return _mm_add_ps(_mm_mul_ps(_x, _y), _z); }
    static Reg Abs(Reg _x)                          { return _mm_andnot_ps(_mm_set1_ps(-0.0f), _x); }
    static Reg Min(Reg _x, Reg _y)                  { return _mm_min_ps(_x, _y); }
    static Reg Max(Reg _x, Reg _y)                  { return _mm_max_ps(_x, _y); }
    static Reg Clamp(Reg _x, Reg _min, Reg _max)    { return Min(Max(_x, _min), _max); }
    
    static Reg Sqrt(Reg _x)                         { return _mm_sqrt_ps(_x); }
    static Reg RSqrt(Reg _x)                        { return _mm_rsqrt_ps(_x); }
    static Reg Rcp(Reg _x)                          { return _mm_rcp_ps(_x); }
    
    // Dot4 / Dot3 : result in x only, other lanes are 0 with SSE4.1 but undefined with SSE2
#if defined(SIMD_SSE41)
    static Reg Dot4(Reg _x, Reg _y)                 { return _mm_dp_ps(_x, _y, 0xF1); }
    static Reg Dot4Splat(Reg _x, Reg _y)            { return _mm_dp_ps(_x, _y, 0xFF); }
    static Reg Dot3(Reg _x, Reg _y)                 { return _mm_dp_ps(_x, _y, 0x71); }
#else
    static Reg Dot4(Reg _x, Reg _y)                 { return HAdd(_mm_mul_ps(_x, _y)); }
    static Reg Dot4Splat(Reg _x, Reg _y)            { __m128 m = _mm_mul_ps(_x, _y);
                                                        __m128 s = _mm_add_ps(m, _mm_shuffle_ps(m, m, _MM_SHUFFLE(1, 0, 3, 2)));
                                                        return _mm_add_ps(s, _mm_shuffle_ps(s, s, _MM_SHUFFLE(2, 3, 0, 1))); }
    static Reg Dot3(Reg _x, Reg _y)                 { __m128 m = _mm_mul_ps(_x, _y);
                                                        __m128 y = _mm_shuffle_ps(m, m, _MM_SHUFFLE(1, 1, 1, 1));
                                                        __m128 z = _mm_movehl_ps(m, m);
                                                        return _mm_add_ss(_mm_add_ss(m, y), z); }
#endif
    static Reg HAdd(Reg _x)                         { __m128 hi = _mm_movehl_ps(_x, _x); __m128 s  = _mm_add_ps(_x, hi);
                                                        return _mm_add_ss(s, _mm_shuffle_ps(s, s, _MM_SHUFFLE(1, 1, 1, 1))); }
    
    static float GetX(Reg _x)                              { return _mm_cvtss_f32(_x); }
    static float GetY(Reg _x)                              { return GetX(Shuffle<1, 0, 2, 3>(_x)); }
    static float GetZ(Reg _x)                              { return GetX(Shuffle<2, 1, 0, 3>(_x)); }
    static float GetW(Reg _x)                              { return GetX(Shuffle<3, 1, 2, 0>(_x)); }
    
    static Reg CmpEq(Reg _x, Reg _y)               { return _mm_cmpeq_ps(_x, _y); }
    static Reg CmpNeq(Reg _x, Reg _y)              { return _mm_cmpneq_ps(_x, _y); }
    static Reg CmpLt(Reg _x, Reg _y)               { return _mm_cmplt_ps(_x, _y); }
    static Reg CmpLe(Reg _x, Reg _y)               { return _mm_cmple_ps(_x, _y); }
    static Reg CmpGt(Reg _x, Reg _y)               { return _mm_cmpgt_ps(_x, _y); }
    static Reg CmpGe(Reg _x, Reg _y)               { return _mm_cmpge_ps(_x, _y); }
    
    static int  MoveMask(Reg _x)                    { return _mm_movemask_ps(_x); }
    static bool AllTrue(int _mask)                  { return _mask == 0xF; }
    static bool AnyTrue(int _mask)                  { return _mask != 0; }
    
    // Blend : takes _y where _mask is set. SSE4.1 only reads the sign bit of each lane, SSE2 uses every bit
#if defined(SIMD_SSE41)
    static Reg  Blend(Reg _x, Reg _y, Reg _mask)    { return _mm_blendv_ps(_x, _y, _mask); }
#else
    static Reg  Blend(Reg _x, Reg _y, Reg _mask)    { return _mm_or_ps(_mm_and_ps(_mask, _y), _mm_andnot_ps(_mask, _x)); }
    
#endif
    static Reg  And(Reg _x, Reg _y)                 { return _mm_and_ps(_x, _y); }
    static Reg  Or(Reg _x, Reg _y)                  { return _mm_or_ps(_x, _y); }
    static Reg  Xor(Reg _x, Reg _y)                 { return _mm_xor_ps(_x, _y); }
    static Reg  NotAnd(Reg _x, Reg _y)                { return _mm_andnot_ps(_x, _y); }
    
    template <int X, int Y, int Z, int W>
    static Reg Shuffle(Reg _x)                      { return _mm_shuffle_ps(_x, _x, _MM_SHUFFLE(W, Z, Y, X)); }
    template <int X, int Y, int Z, int W>
    static Reg Shuffle2(Reg _x, Reg _y)             { return _mm_shuffle_ps(_x, _y, _MM_SHUFFLE(W, Z, Y, X)); }
    template <int I>
    static Reg Broadcast(Reg _x)          { return Shuffle<I, I, I, I>(_x); }
    
    static void Transpose4x4(Reg& _x, Reg& _y, Reg& _z, Reg& _w)
    {
        Reg t0 = _mm_unpacklo_ps(_x, _y);
        Reg t1 = _mm_unpacklo_ps(_z, _w);
        Reg t2 = _mm_unpackhi_ps(_x, _y);
        Reg t3 = _mm_unpackhi_ps(_z, _w);
        
        _x = _mm_movelh_ps(t0, t1);
        _y = _mm_movehl_ps(t0, t1);
        _z = _mm_movelh_ps(t2, t3);
        _w = _mm_movehl_ps(t2, t3);
    }
};
    
}    
    
#endif


