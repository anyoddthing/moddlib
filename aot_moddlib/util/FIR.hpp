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
        FIR()
        {
            reset();
        }
        
        template <typename ContainerT>
        void setCoefs(ContainerT const & coefs)
        {
            std::copy(coefs.begin(), coefs.end(), _coefs.begin());
        }
        
        void setCoefs(std::initializer_list<float> const & coefs)
        {
            std::copy(coefs.begin(), coefs.end(), _coefs.begin());
        }

        void setCoefs(float* coefs)
        {
            std::copy(coefs, coefs + TapSize, _coefs.begin());
        }
        
        void reset()
        {
            _index = 0;
            _coefs.fill(0);
            _buffer.fill(0);
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
        
    private:
        uint _index;
        std::array<float, TapSize> _coefs;
        std::array<float, TapSize> _buffer;
    };
}
