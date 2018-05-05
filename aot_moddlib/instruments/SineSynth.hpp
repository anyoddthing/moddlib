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
#include "../units/SineOscillator.hpp"
#include "../units/SineWTOscillator.hpp"
#include "../units/TriggerModule.hpp"
#include "../debug/debug.hpp"
#include "BasicMidiSynth.hpp"

namespace moddlib
{
    using OscillatorT = SineWTOscillator;
    
    struct SineSynthCircuit :
        Circuit<SineSynthCircuit, PhaseGenerator, OscillatorT, ADSREnvelopeGenerator>,
        InputBank<
            Inputs<1, TriggerPort>,
            Inputs<1, PortLink<FloatInput>>>
    {
        using phaseModule = Mod_<0>;
        using oscModule   = Mod_<1>;
        using envModule   = Mod_<2>;

        using triggerIn   = BIn_<0, 0>;
        using frequencyIn = BIn_<1, 0>;

        SineSynthCircuit()
        {
            //==============================================================================
            // export ports
            
            connect(input<triggerIn>(), port<phaseModule, PhaseGenerator::triggerIn>());
            connect(input<triggerIn>(), port<envModule, ADSREnvelopeGenerator::triggerIn>());
            
            input<frequencyIn>().linkTo(port<phaseModule, PhaseGenerator::freqIn>());

            //==============================================================================
            // connect modules
            
            connect(port<phaseModule, PhaseGenerator::mainOut>(), port<oscModule, OscillatorT::phaseIn>());
            connect(port<oscModule, OscillatorT::mainOut>(), port<envModule, ADSREnvelopeGenerator::mainIn>());
        }
    };
    
    using SineSynth = BasicMidiSynth<SineSynthCircuit>;
}


