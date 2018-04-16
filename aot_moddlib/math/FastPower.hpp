//
//  FastPower.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 1/16/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#ifndef FastPower_h
#define FastPower_h

// based on
// http://www.hxa.name/articles/content/fast-pow-adjustable_hxa7241_2007.html

namespace moddlib
{
    namespace detail
    {
        template <uint Precision>
        struct FastPowLookup
        {
            FastPowLookup()
            {
                const float P2_23 = 8388608.0f;
                
                /* step along table elements and x-axis positions */
                float zeroToOne = 1.0f / ((float)(1 << Precision) * 2.0f);
                for (size_t i = 0;  i < (1 << Precision);  ++i)
                {
                    /* make y-axis value for table element */
                    float f = (std::pow(2.0f, zeroToOne) - 1.0f) * P2_23;
                    _table[i] = (uint)( f < P2_23 ? f : (P2_23 - 1.0f) );
                    zeroToOne += 1.0f / (float)(1 << Precision);
                }
            }
            
            constexpr uint precision()
            {
                return Precision;
            }
            
            uint operator[](const size_t index)
            {
                return _table[index];
            }

        private:
        
            std::array<uint, 1 << Precision> _table;
        };
    }

    template <size_t Precision>
    struct FastPow
    {
        const float LOG2 = std::log(2.f);
        const float P2_23 = 8388608.0f;
        
        FastPow(const FastPow<Precision>& other) = default;
        FastPow(float radix) : _ilog2(P2_23 * (std::log(radix) / LOG2)) {}
        
        FastPow& operator =(const FastPow<Precision>& other)
        {
            _ilog2 = other._ilog2;
            return *this;
        }
        
        float operator()(float val)
        {
            static detail::FastPowLookup<Precision> table;
            return fastPow(table, val);
        }
        
    private:

        template <typename FastPowerLookupT>
        float fastPow(FastPowerLookupT& table, float val)
        {
            /* build float bits */
           auto i = (uint)( (val * _ilog2) + (127.0f * P2_23) );

           /* replace mantissa with lookup */
           union { uint i; float f; }
           result = { (i & 0xFF800000) | table[(i & 0x7FFFFF) >> (23 - table.precision())] };

           /* convert bits to float */
           return result.f;
        }
        
    private:
        float _ilog2;
    };

    struct FastPowHD : public FastPow<12>
    {
        FastPowHD(float radix) : FastPow<12>(radix) {}
    };
}

#endif /* FastPower_h */
