//
//  PhaseGenerator.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 22/11/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#ifndef PhaseGenerator_hpp
#define PhaseGenerator_hpp

#include "../core/core.hpp"
#include "../debug/debug.hpp"

namespace moddlib
{
    struct PhaseGenerator :
        Generator<PhaseGenerator>,
        InputBank<
             Inputs<1, TriggerInput>,
             Inputs<1, FloatInput>>,
        OutputBank<
            Outputs<1, StreamOutput>>
    {
        using triggerIn = BIn_<0, 0>;
        using freqIn    = BIn_<1, 0>;

        PhaseGenerator() : _lastIncrement(0), _phaseIncrement(0)
        {}

        void doInit()
        {
            _lastIncrement  = 0;
            _phaseIncrement = 0;
        }

        void doGenerate()
        {
            fref trigger   = input<triggerIn>();
            auto triggered = trigger.isTriggered();

            if (triggered)
            {
                auto frequency  = input<freqIn>().getValue();
                _phaseIncrement = frequency2Phase(frequency);
                if (_lastIncrement > 0)
                {
                    auto deltaIncr = _phaseIncrement - _lastIncrement;
                    simd::Vec deltaV = simd::Vec(1, 2, 3, 4) * deltaIncr;

                    // not resetting phase here
                    simdv::initPhase(
                        output<mainOut>().ptr(),
                        output<mainOut>().vec() + deltaV + 8 * _lastIncrement,
                        _phaseIncrement);

                    _lastIncrement = _phaseIncrement;
                }
                else
                {
                    simdv::initPhase(output<mainOut>().ptr(), _phaseIncrement);
                    _lastIncrement = _phaseIncrement;
                }
            }
            else
            {
                simdv::updatePhase(output<mainOut>().ptr(), _phaseIncrement);
            }
        }

    private:
        float _lastIncrement;
        float _phaseIncrement;
    };
}

#endif /* PhaseGenerator_hpp */
