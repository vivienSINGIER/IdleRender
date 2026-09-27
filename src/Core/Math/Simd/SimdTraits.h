#ifndef SIMD_TRAITS_H
#define SIMD_TRAITS_H

#include "define.h"
#include "SimdConfig.h"

namespace Simd
{
    template <typename T>
    struct Traits
    {
        static constexpr bool enabled = false;
        static constexpr bool isFloating = false;
        static constexpr size_t alignment = alignof(T);
    };
}

#ifdef SIMD_SSE2
#include "SimdTraits_SSE.hpp"
#endif

#endif
