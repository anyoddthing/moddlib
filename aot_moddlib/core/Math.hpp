//
//  Math.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 04/10/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#ifndef Math_h
#define Math_h

#include "Types.hpp"

namespace moddlib
{
    template<class T>
    constexpr T pi = T(3.141592653589793238462643383279502884197169399375105820974944592307816406286);

    template <typename OutT, typename InT>
    static inline OutT punning_cast(InT val)
    {
        union { InT in; OutT out; } cast = { val };
        return cast.out;
    }

    static inline float approxLog2(float val)
    {
        auto i = punning_cast<int32_t>(val);
        return (((i & 0x7f800000) >> 23) - 0x7f)+(i & 0x007fffff) / static_cast<float>(0x800000);
    }

    static inline void normalizeArray(float* data, size_t size)
    {
        auto minMax = std::minmax_element(data, data + size);
        auto max    = std::max(std::abs(*minMax.first), *minMax.second);
        auto gain   = 1.0f / std::max(max, 0.0000001f);

        std::for_each(data, data + size, [=](fref val) { val *= gain; });
    }

    static inline float wrapPhase(float val)
    {
        return (val > 1.0f) ? val - 2.0f : val;
    }

    static inline float incrementWrapPhase(float val, float phaseIncrement)
    {
        return wrapPhase(val + phaseIncrement);
    }

    constexpr static inline uint32_t roundDownToPower2(uint32_t val)
    {
        val |= val >> 1;
        val |= val >> 2;
        val |= val >> 4;
        val |= val >> 8;
        val |= val >> 16;
        return val - (val >> 1);
    }

    constexpr static inline uint32_t roundUpToPower2(uint32_t val)
    {
        val--;
        val |= val >> 1;
        val |= val >> 2;
        val |= val >> 4;
        val |= val >> 8;
        val |= val >> 16;
        return val + 1;
    }
}

#endif /* Math_h */
