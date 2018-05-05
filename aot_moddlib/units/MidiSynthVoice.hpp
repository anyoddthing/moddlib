//
//  MidiSynthVoice.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 12/21/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#ifndef MidiSynthVoice_h
#define MidiSynthVoice_h

#include <memory>

#include "../core/core.hpp"
#include "../debug/debug.hpp"

namespace moddlib
{
    template <typename VoiceT, typename CircuitT>
    struct MidiSynthVoice : Voice
    {
        using circuit = CircuitT;
        
        MidiSynthVoice() :
            _rampSamples(0),
            _circuit(std::make_unique<CircuitT>())
        {}

        void generate()
        {
            _circuit->generate();
            
            if (thiz()->isSilent())
            {
                setState(Voice::IDLE);
            }

            if (_rampSamples > 0)
            {
                auto fact = static_cast<float>(_rampSamples) / RAMP_SAMPLES;
                simdv::ramp(thiz()->mainOut(), thiz()->mainOut(), fact, -1.f / RAMP_SAMPLES );

                _rampSamples -= 8;
                if (_rampSamples <= 0)
                {
                    setState(Voice::IDLE);
                }
            }
        }

        void noteOn(MidiMessage message)
        {
            _circuit->init();
            _rampSamples = 0;

            setNote(message.getMidiNote());
            setState(Voice::ON);

            thiz()->triggerNote(message);
        }

        void noteOff(MidiMessage message)
        {
            thiz()->triggerDown();
            setState(Voice::FADE_OUT);
        }

        void killVoice()
        {
            if (getState() > Voice::State::RAMP_DOWN)
            {
                thiz()->triggerDown();
                setState(Voice::RAMP_DOWN);
                _rampSamples = RAMP_SAMPLES;
            }
        }

        CircuitT& getCircuit()
        {
            return *_circuit;
        }

    private:

        VoiceT* thiz()
        {
            return static_cast<VoiceT*>(this);
        }

    private:
        static constexpr uint RAMP_SAMPLES = 40;
        uint _rampSamples;

        std::unique_ptr<CircuitT> _circuit;
    };
}

#endif /* MidiSynthVoice_h */
