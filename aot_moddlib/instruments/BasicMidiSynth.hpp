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
        using envModule = typename CircuitT::envModule;
        
        BasicMidiSynthVoice()
        {
            auto& circuit = this->template getCircuit();
            connect(getTrigger(), circuit.template input<typename CircuitT::triggerIn>());
            connect(getFrequency(), circuit.template input<typename CircuitT::frequencyIn>());
        }

        constexpr StreamOutput& mainOut()
        {
            auto& circuit = this->template getCircuit();
            return circuit.template moduleOut<envModule, ADSREnvelopeGenerator::mainOut>();
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

        BasicMidiSynth() :
            _releaseInput(0.1f, 4.0f)
        {
            for (auto& voice : _voices)
            {
                auto& envModule = voice.getCircuit().template module<typename CircuitT::envModule>();

                connect(_releaseInput, envModule.template input<ADSREnvelopeGenerator::releaseIn>());
            }
        }

        std::array<VoiceType, 12>& getVoices()
        {
            return _voices;
        }

    private:

        std::array<VoiceType, 12>  _voices;
        MidiOutputPort             _releaseInput;
    };
}


