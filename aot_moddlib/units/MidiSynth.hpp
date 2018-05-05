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
    using MidiOutputPortBank = std::array<MidiOutputPort, 128>;
    
    template <typename CircuitT>
    using MidiPortMapperAction = std::function<void(MidiOutputPortBank&, CircuitT&)>;
    
    template <typename CircuitT, typename SelT>
    struct MidiPortMapper
    {
        using selector = SelT;
        
        MidiPortMapper(uint8_t  controller) : _controller(controller)
        {
        }
        
        uint8_t getController() { return _controller; }
        
        MidiPortMapperAction<CircuitT> bind()
        {
            auto controller = _controller;
            return [controller](MidiOutputPortBank& portBank, CircuitT& circuit)
            {
                connect(portBank[controller], port<SelT>(circuit));
            };
        }
        
        MidiPortMapperAction<CircuitT> unbind()
        {
            return [](MidiOutputPortBank& portBank, CircuitT& circuit)
            {
                port<SelT>(circuit).disconnect();
            };
        }
        
    private:
        uint8_t _controller;
    };
    
    template <typename SynthT, typename VoiceT>
    struct MidiSynth
    {
        using circuit = typename VoiceT::circuit;
        
        MidiSynth() :
            _nextVoice(0)
        {
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
            return _midiBuffer.enqueue(message);
        }
        
        template <typename SelT>
        void bindMidiController(MidiPortMapper<circuit, SelT>& mapper)
        {
            auto& voices = thiz()->getVoices();
            for (auto& voice : voices)
            {
                mapper.bind()(_midiPorts, voice);
            }
            
            _mappings.insert({mapper.getController(), mapper.unbind()});
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
            _midiPorts[message.getControl()].setControlValue(message.getControlValue());
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

        size_t                  _nextVoice;
        MidiBuffer              _midiBuffer;
        MidiOutputPortBank      _midiPorts;
        std::unordered_map<uint8_t, MidiPortMapperAction<circuit>> _mappings;
    };
}




