//
//  FIR.hpp
//  moddlib
//
//  Created by Daniel Doubleday on 1/5/18.
//  Copyright © 2018 Daniel Doubleday. All rights reserved.
//

#pragma once

namespace moddlib
{
    template <uint TapSize>
    struct FIR
    {
        FIR() : _coefs(TapSize), _scoefs(simd::pad(simd::stride * TapSize)), _buffer(simd::pad(TapSize))
        {
            reset();
        }
        
        template <typename ContainerT>
        void setCoefs(const ContainerT& coefs)
        {
            std::copy(coefs.begin(), coefs.end(), _coefs.begin());
            setSimdCoefs(coefs);
        }
        
        void setCoefs(const std::initializer_list<float>& coefs)
        {
            std::copy(coefs.begin(), coefs.end(), _coefs.begin());
            setSimdCoefs(coefs.begin());
        }

        void setCoefs(float* coefs)
        {
            std::copy(coefs, coefs + TapSize, _coefs.begin());
            setSimdCoefs(coefs);
        }
        
        template <typename ContainerT>
        void setSimdCoefs(const ContainerT& coefs)
        {
            for (auto i = 0; i < TapSize; ++i)
            {
                for (auto j = 0; j < simd::stride; ++j)
                {
                    _scoefs[i * simd::stride + j] = coefs[(i + j) % TapSize];
                }
            }
        }
        
        void reset()
        {
            _index = 0;
            _coefs.zero_mem();
            _scoefs.zero_mem();
            _buffer.zero_mem();
        }
        
        float filter(float sample)
        {
            _buffer[_index++] = sample;
            if (_index == TapSize)
                _index = 0;
            
            float acc = 0;
            uint index = _index;
            for (uint i = 0; i < TapSize; ++i) {
                index = index != 0 ? index - 1 : TapSize - 1;
                acc += _buffer[index] * _coefs[i];
            };
            
            return acc;
        }
        
        float filter2(float sample)
        {
            _buffer[_index] = sample;
            
            float out = 0;
            size_t k;
            for (k = 0; k < TapSize - _index; ++k)
                out += _buffer[(_index + k)] * _coefs[k];

            for (size_t j = 0; j < _index; ++j)
                out += _buffer[j] * _coefs[j + k];

            _index = (_index == 0 ? TapSize - 1 : _index - 1);

            return out;
        }
        
        float simdFilter(float sample)
        {
            using namespace simd;
            
            _buffer[_index] = sample;
            Vec partial(0.0f);
            for (auto i = 0; i < TapSize; i += stride)
            {
                auto coefIdx = (TapSize - _index + i) % TapSize;
                
                Vec coefs(_scoefs.data() + coefIdx * simd::stride);
                Vec samples(_buffer.data() + i);
                
                partial += coefs * samples;
            }
            
            _index = (_index == 0 ? TapSize - 1 : _index - 1);
            return partial.sum();
        }
        
    private:
        uint _index;
        simd::AlignedMemory _coefs;
        simd::AlignedMemory _scoefs;
        simd::AlignedMemory _buffer;
    };
}
