//
// Created by Daniel Doubleday on 2/27/16.
// Copyright (c) 2016 Daniel Doubleday. All rights reserved.
//

#pragma once

#include <memory>
#include <unordered_map>

#include "../core/core.hpp"
#include "../midi/midi.hpp"

#include "../units/PhaseGenerator.hpp"
#include "../units/ADSREnvelopeGenerator.hpp"
#include "../units/SineOscillator.hpp"
#include "../units/MidiSynthVoice.hpp"

#include "../debug/debug.hpp"

namespace moddlib
{
    template <typename CircuitT>
    using MidiPortMapperAction = std::function<void(MidiPortBank&, CircuitT&)>;
    
    template <typename CircuitT, typename SelT>
    struct MidiPortMapper
    {
        using selector = SelT;
        
        MidiPortMapper(uint8_t controller, bool isRelative = false) :
            _controller(controller), _isRelative(isRelative)
        {
        }
        
        uint8_t getController() { return _controller; }
        
        MidiPortMapperAction<CircuitT> bind()
        {
            auto controller = _controller;
            auto relative = _isRelative;
            return [controller, relative](MidiPortBank& portBank, CircuitT& circuit)
            {
                portBank.setRelative(controller, relative);
                auto& input = port<SelT>(circuit);
                auto& port = portBank[controller];
                port.setValue(input.getValue());
                connect(port, input);
            };
        }
        
        MidiPortMapperAction<CircuitT> unbind()
        {
            return [](MidiPortBank& portBank, CircuitT& circuit)
            {
                port<SelT>(circuit).disconnect();
            };
        }
        
    private:
        uint8_t _controller;
        bool _isRelative;
    };
    
    template <typename SynthT, typename VoiceT, uint NumVoices = 12>
    struct MidiSynth
    {
        using CircuitType = typename VoiceT::CircuitType;
        
        MidiSynth() : _nextVoice(0)
        {
        }

        void configureVoices(std::function<void(uint, VoiceT&)> configurator)
        {
            auto& voices = getVoices();
            for (uint i = 0; i < voices.size(); ++i)
            {
                configurator(i, voices[i]);
            }
        }
        
        void generate(SampleBuffer& outputBuffer)
        {
            processMessages();

            outputBuffer.zero_mem();
            auto& voices = thiz()->getVoices();
            for (auto& voice : voices)
            {
                if (voice.isActive())
                {
                    voice.generate();
                    auto& voiceOut = voice.mainOut();
                    outputBuffer.add(voiceOut);
                }
            }

            simdv::loop([&](auto i)
            {
                simd::Vec out(outputBuffer.data() + i);
                out *= 0.3f;
                out.store(outputBuffer.data() + i);
            });
        }

        bool queueMessage(const MidiMessage message)
        {
            std::cout << "queueMessage: " << message.getType() << std::endl;
            return _midiBuffer.enqueue(message);
        }
        
        template <typename SelT>
        void bindMidiController(MidiPortMapper<CircuitType, SelT> mapper)
        {
            auto& voices = thiz()->getVoices();
            for (auto& voice : voices)
            {
                mapper.bind()(_midiPorts, voice.getCircuit());
            }
            
            _mappings.insert({mapper.getController(), mapper.unbind()});
        }

        std::array<VoiceT, NumVoices>& getVoices()
        {
            return _voices;
        }
        
        MidiOutputPort& getMidiPort(uint index)
        {
            return _midiPorts[index];
        }

    private:

        SynthT* thiz()
        {
            return static_cast<SynthT*>(this);
        }
        
        void processMessages()
        {
            MidiMessage message;
            while (_midiBuffer.dequeue(message))
            {
                std::cout << "processMessage: " << message.getType() << std::endl;
                processMessage(message);
            }
        }

        void processMessage(const MidiMessage message)
        {
            switch (message.getType())
            {
                case MIDI_NOTE_ON:
                    processNoteOn(message);
                    break;
                case MIDI_NOTE_OFF:
                    processNoteOff(message);
                    break;
                case MIDI_CONTROL_CHANGE:
                    processControlChange(message);
                    break;
                default:
                    break;
            }
        }

        void processNoteOn(MidiMessage message)
        {
            LOG("Active voices: {}", numActiveVoices());
            auto midiNote = message.getMidiNote();
            auto& voices      = thiz()->getVoices();
            auto oldVoice = std::find_if(voices.begin(), voices.end(),
                    [=](fref voice) { return voice.getLastNote() == midiNote
                            && voice.getState()    >  Voice::State::RAMP_DOWN; });

            if (oldVoice != voices.end())
            {
                oldVoice->killVoice();
            }

            auto& newVoice = allocateVoiceForNote(midiNote);
            newVoice.noteOn(message);
        }

        void processNoteOff(MidiMessage message)
        {
            auto midiNote = message.getMidiNote();
            auto& voices = thiz()->getVoices();
            auto voice    = std::find_if(voices.begin(), voices.end(),
                    [=](fref voice) { return voice.getLastNote() == midiNote
                            && voice.getState()    == Voice::State::ON; });

            if (voice != voices.end())
            {
                voice->noteOff(message);
            }
        }

        void processControlChange(MidiMessage message)
        {
            _midiPorts.processMessage(message);
        }

        uint numActiveVoices()
        {
            auto& voices = thiz()->getVoices();
            auto count   = std::count_if(voices.begin(), voices.end(),
                [](fref voice) { return voice.getState() != Voice::IDLE; }
            );
            
            return static_cast<uint>(count);
        }

        VoiceT& allocateVoiceForNote(uint8_t note)
        {
            auto currentMatch = std::numeric_limits<uint8_t>::max();
            auto& voices      = thiz()->getVoices();
            VoiceT* voice  = &voices[0];

            for (size_t i = 0; i < voices.size(); ++i)
            {
                auto voiceIdx  = (_nextVoice + i) % voices.size();
                auto& candidate = voices[voiceIdx];
                if (candidate.getState() == Voice::IDLE)
                {
                    voice      = &candidate;
                    _nextVoice = voiceIdx;
                    break;
                }
                else if (currentMatch > candidate.getState())
                {
                    voice        = &candidate;
                    _nextVoice   = voiceIdx;
                    currentMatch = candidate.getState();
                }
            }

            _nextVoice = (_nextVoice + 1) % voices.size();
            return *voice;
        }

    private:
        std::array<VoiceT, NumVoices>   _voices;
        size_t                          _nextVoice;
        MidiBuffer                      _midiBuffer;
        MidiPortBank                    _midiPorts;
        std::unordered_map<uint8_t, MidiPortMapperAction<CircuitType>> _mappings;
    };
}




