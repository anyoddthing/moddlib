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
#include "../units/MidiSynthVoice.hpp"
#include "../units/MidiSynth.hpp"
#include "../units/TriggerModule.hpp"
#include "../debug/debug.hpp"

namespace moddlib
{
    template <typename CircuitT>
    struct BasicMidiSynthVoice : MidiSynthVoice<BasicMidiSynthVoice<CircuitT>, CircuitT>, TriggerModule
    {
        using CircuitType = CircuitT;
        
        using envModule = typename CircuitT::envModule;
        
        BasicMidiSynthVoice()
        {
            auto& circuit = this->template getCircuit();
            connect(getTrigger(), input<TriggerInT<CircuitT>>(circuit));
            connect(getFrequency(), input<FreqInT<CircuitT>>(circuit));
        }

        constexpr StreamOutput& mainOut()
        {
            auto& circuit = this->template getCircuit();
            return portOut<envModule>(circuit);
        }

        bool isSilent()
        {
            auto& circuit = this->template getCircuit();
            auto isPlaying = circuit.template module<typename CircuitT::envModule>().isPlaying();
            return ! isPlaying;
        }
    };

    template <typename CircuitT>
    struct BasicMidiSynth : MidiSynth<BasicMidiSynth<CircuitT>, BasicMidiSynthVoice<CircuitT>>
    {
        using CircuitType = CircuitT;
        using VoiceType = BasicMidiSynthVoice<CircuitT>;
    };
}


