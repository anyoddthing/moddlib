//
//  SawtoothOscillatorDPW.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 2/28/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#ifndef SawtoothOscillatorDPW_h
#define SawtoothOscillatorDPW_h

#include "../core/core.hpp"
#include "../util/SawtoothTable.hpp"

namespace moddlib
{
    struct SawtoothOscillatorDPW :
        Generator<SawtoothOscillatorDPW>,
        InputBank<
            Inputs<1, TriggerInput>,
            Inputs<1, FloatInput>>,
        SingleStreamOutput
    {
        const float VERY_LOW_FREQUENCY = 2.0 * 0.1 / 44100.0;
        
        using triggerIn = BIn_<0, 0>;
        using freqIn    = BIn_<1, 0>;
        
        SawtoothOscillatorDPW() : _phase(0)
        {}
        
        void doGenerate()
        {
            if (input<triggerIn>().isTriggered())
            {
                _phase = 0;
            }
            
            auto currentPhase   = _phase;
            auto phaseIncrement = frequency2Phase(input<freqIn>());

            auto freqLevel = SawtoothTable::convertPhaseIncrementToLevel(phaseIncrement);
            
            for (auto i = 0; i < Engine::FRAMES_PER_BLOCK; ++i)
            {
                /* Square the raw sawtooth. */
                auto squared = currentPhase * currentPhase;
                // Differentiate using a delayed value.
                auto diffed = squared - _z2;
                _z2 = _z1;
                _z1 = squared;

                /* Calculate scaling based on phaseIncrement */
                auto pinc = phaseIncrement;
                // Absolute value.
                if (pinc < 0.0f)
                {
                    pinc = 0.0f - pinc;
                }

                // If the frequency is very low then just use the raw sawtooth.
                // This avoids divide by zero problems and scaling problems.
                if (pinc < VERY_LOW_FREQUENCY)
                {
                    output<mainOut>()[i] = currentPhase;
                }
                else
                {
                    output<mainOut>()[i] = diffed * 0.25f / pinc;
                }
                
                currentPhase = incrementWrapPhase(currentPhase, phaseIncrement);
            }
            
            _phase = currentPhase;
        }

    private:
        float _phase;
        float _z1, _z2;
    };
    
}

#endif /* SawtoothOscillatorDPW_h */
