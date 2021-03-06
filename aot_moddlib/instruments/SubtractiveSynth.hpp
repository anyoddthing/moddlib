//
//  SubtractiveSynth.h
//  unitsynth
//
//  Created by Daniel Doubleday on 2/28/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#ifndef SubtractiveSynth_h
#define SubtractiveSynth_h

#include <memory>

#include "../core/core.hpp"
#include "../midi/midi.hpp"

#include "../units/PhaseGenerator.hpp"
#include "../units/ADSREnvelopeGenerator.hpp"
#include "../units/WaveTableOscillator.hpp"
#include "../units/TriggerModule.hpp"
#include "../units/FourPoleFilter.hpp"
#include "../debug/debug.hpp"
#include "BasicMidiSynth.hpp"


namespace moddlib
{
    template <typename OscillatorT>
    struct SubtractiveSynthCircuit :
        Circuit<SubtractiveSynthCircuit<OscillatorT>,
            PhaseGenerator,
            OscillatorT,
            FourPoleFilter<double>,
            ADSREnvelopeGenerator
            >,
        InputBank<
            Inputs<1, TriggerPort>,
            Inputs<1, FloatPort>,
            Inputs<2, StreamPort>>
    {
        using phaseModule   = Mod_<0>;
        using oscModule     = Mod_<1>;
        using filterModule  = Mod_<2>;
        using envModule     = Mod_<3>;

        using triggerIn     = BIn_<0, 0>;
        using freqIn        = BIn_<1, 0>;
        using filterFreqIn  = BIn_<2, 0>;
        using filterQIn     = BIn_<2, 1>;

        SubtractiveSynthCircuit()
        {
            //==============================================================================
            // connect inputs

            connect(
                input<triggerIn>(),
                port<phaseModule, TriggerInT<PhaseGenerator>>(this));
            connect(
                input<freqIn>(),
                port<phaseModule, FreqInT<PhaseGenerator>>(this));
            tryconnect(
                input<freqIn>(),
                port<oscModule, FreqInT<OscillatorT>>(this));

            connect(
                input<triggerIn>(),
                port<envModule, TriggerInT<ADSREnvelopeGenerator>>(this));
            connect(
                port<envModule, ADSREnvelopeGenerator::envOut>(this),
                port<filterModule, FourPoleFilter<>::freqIn>(this));
            connect(
                input<filterQIn>(),
                port<filterModule, FourPoleFilter<>::qIn>(this));

            
            //==============================================================================
            // connect modules

            connect(
                portOut<phaseModule>(this),
                port<oscModule, PhaseInT<OscillatorT>>(this));
            connect(
                portOut<oscModule>(this),
                portIn<filterModule>(this));
            connect(
                portOut<filterModule>(this),
                portIn<envModule>(this));
        }
    };
    
    template <typename OscillatorT>
    using SubtractiveSynth = BasicMidiSynth<SubtractiveSynthCircuit<OscillatorT>>;

}



#endif /* SubtractiveSynth_h */
