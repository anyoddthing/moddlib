//
//  SineOscillator.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 04/10/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#pragma once

#include "aot_simdlib/aot_simdlib.hpp"
#include "../core/core.hpp"

namespace moddlib
{
    struct SineOscillator :
        Generator<SineOscillator>,
        InputBank<Inputs<1, StreamInput>>,
        SingleStreamOutput
    {
        using phaseIn = BIn_<0, 0>;
        
        void doGenerate()
        {
            auto& phase = input<phaseIn>().buffer();
            auto& out   = output<mainOut>().buffer();
            
            simdv::fastSin(out.data(), phase.data());
        }
    };
}


