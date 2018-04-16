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
#include "../units/SawtoothOscillatorDPW.hpp"
#include "../units/MidiSynthVoice.hpp"
#include "../units/MidiSynth.hpp"
#include "../units/TriggerModule.hpp"
#include "../debug/debug.hpp"


namespace moddlib
{
    struct SubtractiveSynthCircuit :
        Circuit<SubtractiveSynthCircuit, SawtoothOscillatorDPW, ADSREnvelopeGenerator>,
        InputBank<
            Inputs<1, TriggerPort>,
            Inputs<1, PortLink<FloatInput>>>
    {
        using oscModule   = Mod_<0>;
        using envModule   = Mod_<1>;

        using triggerIn   = BIn_<0, 0>;
        using frequencyIn = BIn_<1, 0>;

        SubtractiveSynthCircuit()
        {
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // export ports
            
            connect(input<triggerIn>(), moduleIn<oscModule, SawtoothOscillatorDPW::triggerIn>());
            connect(input<triggerIn>(), moduleIn<envModule, ADSREnvelopeGenerator::triggerIn>());
            
            input<frequencyIn>().linkTo(moduleIn<oscModule, SawtoothOscillatorDPW::freqIn>());

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // connect modules
            
            connect(moduleOut<oscModule, SawtoothOscillatorDPW::mainOut>(), moduleIn<envModule, ADSREnvelopeGenerator::mainIn>());
        }
    };

    
    struct SubtractiveVoice : MidiSynthVoice<SubtractiveVoice, SubtractiveSynthCircuit>, TriggerModule
    {
        SubtractiveVoice()
        {
            connect(getTrigger(), getCircuit().input<SubtractiveSynthCircuit::triggerIn>());
            connect(getFrequency(), getCircuit().input<SubtractiveSynthCircuit::frequencyIn>());
        }
        
        constexpr StreamOutput& mainOut()
        {
            return getCircuit().moduleOut<SubtractiveSynthCircuit::envModule, ADSREnvelopeGenerator::mainOut>();
        }

        bool isSilent()
        {
            auto isPlaying = getCircuit().module<SubtractiveSynthCircuit::envModule>().isPlaying();
            return ! isPlaying;
        }
    };

    struct SubtractiveSynth : MidiSynth<SubtractiveSynth, SubtractiveVoice>
    {
        SubtractiveSynth() :
            _releaseInput(0.1f, 4.0f)
        {
            for (fref voice : _voices)
            {
                using namespace std::placeholders;
                auto& envModule = voice.getCircuit().module<SubtractiveSynthCircuit::envModule>();
                
                connect(_releaseInput, envModule.input<ADSREnvelopeGenerator::releaseIn>());
            }
        }

        std::array<SubtractiveVoice, 12>& getVoices()
        {
            return _voices;
        }

    private:
    
        std::array<SubtractiveVoice, 12> _voices;
        MidiOutputPort _releaseInput;
    };
}



#endif /* SubtractiveSynth_h */
