//
//  WaveTableOscillator.hpp
//  moddlib
//
//  Created by Daniel Doubleday on 12/13/17.
//  Copyright © 2017 Daniel Doubleday. All rights reserved.
//

#pragma once

#include "../core/core.hpp"
#include "../util/WaveTable.hpp"

namespace moddlib
{
    struct WaveTableOscillator :
        Generator<WaveTableOscillator>,
        InputBank<
            Inputs<1, FloatInput>,
            Inputs<1, StreamInput>>,
        SingleStreamOutput
    {
        using freqIn  = BIn_<0, 0>;
        using phaseIn = BIn_<1, 0>;

        using WaveTable2DType = WaveTable2D<>;            
    
        void setupTables(const std::vector<float> &partials)
        {
            _waveTable2d.setupTables(20, partials);
        }

        void doGenerate()
        {
            auto frequency = input<freqIn>().getValue();
            
            auto& phase = input<phaseIn>().buffer();
            auto& out   = output<mainOut>().buffer();
            
            out.forEach([&](auto i, auto& val)
            {
                val = _waveTable2d.lookup(phase[i], frequency);
            });
        }

    private:
    
        WaveTable2DType _waveTable2d;
    };
}

