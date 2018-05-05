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
        using freqIn = BIn_<1, 0>;

        WaveTableSynthCircuit()
        {
            //==============================================================================
            // export ports
            
            connect(input<triggerIn>(), port<phaseModule, PhaseGenerator::triggerIn>());
            connect(input<freqIn>(), port<phaseModule, FreqInT<PhaseGenerator>>());
            connect(input<freqIn>(), port<oscModule, WaveTableOscillator::freqIn>());
            
            connect(input<triggerIn>(), port<envModule, ADSREnvelopeGenerator::triggerIn>());

            //==============================================================================
            // connect modules
            
            connect(portOut<phaseModule>(), port<oscModule, WaveTableOscillator::phaseIn>());
            connect(portOut<oscModule>(), portIn<envModule>());
            
            auto& osc = module<oscModule>();
            auto waveTable = std::make_shared<WaveTable2D>();
            waveTable->setupTables(40, waveTable::sawPartials());
            osc.setupTables(waveTable);
        }
    };
    
    using WaveTableSynth = BasicMidiSynth<WaveTableSynthCircuit>;
}



