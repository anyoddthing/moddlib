//
//  SineSynth.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 04/10/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//
#ifndef SineSynth_hpp
#define SineSynth_hpp

#include <memory>

#include "../core/core.hpp"
#include "../midi/midi.hpp"

#include "../units/PhaseGenerator.hpp"
#include "../units/ADSREnvelopeGenerator.hpp"
#include "../units/SineOscillator.hpp"
#include "../units/SineWTOscillator.hpp"
#include "../units/MidiSynthVoice.hpp"
#include "../units/MidiSynth.hpp"
#include "../units/TriggerModule.hpp"
#include "../debug/debug.hpp"


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
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // export ports
            
            connect(input<triggerIn>(), moduleIn<phaseModule, PhaseGenerator::triggerIn>());
            connect(input<triggerIn>(), moduleIn<envModule, ADSREnvelopeGenerator::triggerIn>());
            
            input<frequencyIn>().linkTo(moduleIn<phaseModule, PhaseGenerator::freqIn>());

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // connect modules
            
            connect(moduleOut<phaseModule, PhaseGenerator::mainOut>(), moduleIn<oscModule, OscillatorT::phaseIn>());
            connect(moduleOut<oscModule, OscillatorT::mainOut>(), moduleIn<envModule, ADSREnvelopeGenerator::mainIn>());
        }
    };

    
    struct SineVoice : MidiSynthVoice<SineVoice, SineSynthCircuit>, TriggerModule
    {
        SineVoice()
        {
            connect(getTrigger(), getCircuit().input<SineSynthCircuit::triggerIn>());
            connect(getFrequency(), getCircuit().input<SineSynthCircuit::frequencyIn>());
        }
        
        constexpr StreamOutput& mainOut()
        {
            return getCircuit().moduleOut<SineSynthCircuit::envModule, ADSREnvelopeGenerator::mainOut>();
        }

        bool isSilent()
        {
            auto isPlaying = getCircuit().module<SineSynthCircuit::envModule>().isPlaying();
            return ! isPlaying;
        }
    };

    struct SineSynth : MidiSynth<SineSynth, SineVoice>
    {
        SineSynth() :
            _releaseInput(0.1f, 4.0f)
        {
            for (auto& voice : _voices)
            {
                auto& envModule = voice.getCircuit().module<SineSynthCircuit::envModule>();
                
                connect(_releaseInput, envModule.input<ADSREnvelopeGenerator::releaseIn>());
            }
        }

        std::array<SineVoice, 12>& getVoices()
        {
            return _voices;
        }

    private:
    
        std::array<SineVoice, 12>  _voices;
        MidiOutputPort             _releaseInput;
    };
}


#endif /* SineSynth_hpp */
