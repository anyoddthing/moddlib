//
//  SineWTOscillator.hpp
//  moddlib
//
//  Created by Daniel Doubleday on 11/19/17.
//  Copyright © 2017 Daniel Doubleday. All rights reserved.
//

#pragma once

#include <cmath>

#include "aot_simdlib/aot_simdlib.hpp"

#include "../core/core.hpp"

namespace moddlib
{
    struct SineWaveTable
    {
        static SineWaveTable& instance()
        {
            static SineWaveTable table;
            return table;
        }
        
        float getSample(float phase)
        {
            size_t tableSize = _table.size();
            float lookup = 0.5f * (phase + 1) * tableSize;
            size_t index1 = static_cast<size_t>(lookup);
            size_t index2 = ( index1 + 1 ) & ( tableSize - 1 ); // faster mod that only works if tableSize is a power of 2
            float val1 = _table[index1];
            float val2 = _table[index2];
            float frac = lookup - (float)index1;

            return val2 + frac * ( val2 - val1 );
        }
        
        SineWaveTable()
        {
            float phaseIncr = 2.0f / (_table.size());
            for (auto i = 0; i < _table.size(); ++i)
            {
                float phase = -1 + i * phaseIncr;
                _table[i] = std::sin(phase * simd::pi<float>);
            }
        }
        
    private:
        AlignedBuffer<64> _table;
    };
    
    struct SineWTOscillator :
        Generator<SineWTOscillator>,
        InputBank<Inputs<1, StreamInput>>,
        SingleStreamOutput
    {
        using phaseIn = BIn_<0, 0>;
        
        void doGenerate()
        {
            auto& phase = input<phaseIn>().buffer();
            auto& out   = output<mainOut>().buffer();
            
            phase.forEach([&](auto i, auto& p)
            {
                out[i] = SineWaveTable::instance().getSample(p);
            });
        }
    };
}
