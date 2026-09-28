#ifndef SIMD_CONFIG_H_DEFINED
#define SIMD_CONFIG_H_DEFINED

// #define SIMD_FORCE_SCALAR 1

#ifndef SIMD_FORCE_SCALAR

#if defined(_M_X64) || defined(__x86_64__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2) || defined(__SSE2__)
    #define SIMD_SSE2 1
    #include <immintrin.h>
    #include <xmmintrin.h>
    #if defined(__AVX__)
        #define SIMD_SSE41 1
        #define SIMD_AVX   1
        #include <immintrin.h>
    #elif defined(__SSE4_1__)
        #define SIMD_SSE41 1
        #include <smmintrin.h>
    #endif
    #if defined(__AVX2__)
        #define SIMD_AVX2 1
    #endif
#elif defined(_M_ARM64) || defined(__ARM_NEON)
    #define SIMD_NEON 1
    #include <arm_neon.h>
#endif

#endif

#endif
