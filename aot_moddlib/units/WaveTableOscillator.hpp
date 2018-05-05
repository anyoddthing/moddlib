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

        void setupTables(std::shared_ptr<WaveTable2D> waveTable2d)
        {
            _waveTable2d = waveTable2d;
        }

        void doGenerate()
        {
            if (_waveTable2d)
            {
                auto frequency = input<freqIn>().getValue();
                
                auto& phase = input<phaseIn>().buffer();
                auto& out   = output<mainOut>().buffer();
                
                out.forEach([&](auto i, auto& val)
                {
                    val = _waveTable2d->lookup(phase[i], frequency);
                });
            }
        }

    private:
        std::shared_ptr<WaveTable2D> _waveTable2d;
    };
}

