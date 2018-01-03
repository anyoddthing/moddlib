//
//  SineSynth.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 04/10/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//
#pragma once

#include <memory>

#include "../core/core.hpp"
#include "../midi/midi.hpp"

#include "../units/PhaseGenerator.hpp"
#include "../units/ADSREnvelopeGenerator.hpp"
#include "../units/WaveTableOscillator.hpp"
#include "../units/TriggerModule.hpp"
#include "../debug/debug.hpp"
#include "BasicMidiSynth.hpp"

namespace moddlib
{
    struct WaveTableSynthCircuit :
        Circuit<WaveTableSynthCircuit, PhaseGenerator, WaveTableOscillator, ADSREnvelopeGenerator>,
        InputBank<
            Inputs<1, TriggerPort>,
            Inputs<1, FloatPort>>
    {
        using phaseModule = Mod_<0>;
        using oscModule   = Mod_<1>;
        using envModule   = Mod_<2>;

        using triggerIn   = BIn_<0, 0>;
        using frequencyIn = BIn_<1, 0>;

        WaveTableSynthCircuit()
        {
            //==============================================================================
            // export ports
            
            connect(input<triggerIn>(), moduleIn<phaseModule, PhaseGenerator::triggerIn>());
            connect(input<frequencyIn>(), moduleIn<phaseModule, PhaseGenerator::freqIn>());
            connect(input<frequencyIn>(), moduleIn<oscModule, WaveTableOscillator::freqIn>());
            
            connect(input<triggerIn>(), moduleIn<envModule, ADSREnvelopeGenerator::triggerIn>());

            //==============================================================================
            // connect modules
            
            connect(moduleOut<phaseModule, PhaseGenerator::mainOut>(), moduleIn<oscModule, WaveTableOscillator::phaseIn>());
            connect(moduleOut<oscModule, WaveTableOscillator::mainOut>(), moduleIn<envModule, ADSREnvelopeGenerator::mainIn>());
            
            
            auto& osc = module<oscModule>();
            osc.setupTables({1});
        }
    };
    
    using WaveTableSynth = BasicMidiSynth<WaveTableSynthCircuit>;
}



