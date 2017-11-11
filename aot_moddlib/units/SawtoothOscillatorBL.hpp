//
//  SawtoothOscillatorBL.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 2/27/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#ifndef SawtoothOscillatorBL_h
#define SawtoothOscillatorBL_h

#include "../core/core.hpp"
#include "../util/SawtoothTable.hpp"

namespace moddlib
{
    struct SawtoothOscillatorBL :
        Generator<SawtoothOscillatorBL>,
        InputBank<
            Inputs<1, TriggerInput>,
            Inputs<1, FloatInput>>,
        SingleStreamOutput
    {
        using triggerIn = BIn_<0, 0>;
        using freqIn    = BIn_<1, 0>;
        
        SawtoothOscillatorBL() : _phase(0)
        {}
        
        void doGenerate()
        {
            if (input<triggerIn>().isTriggered())
            {
                _phase = 0;
            }
            
            fref st = SawtoothTable::instance();

            auto currentPhase   = _phase;
            auto phaseIncrement = frequency2Phase(input<freqIn>());

            auto freqLevel = SawtoothTable::convertPhaseIncrementToLevel(phaseIncrement);
            
            for (auto i = 0; i < Engine::FRAMES_PER_BLOCK; ++i)
            {
                output<mainOut>()[i] = st.calculateSawtooth(currentPhase, phaseIncrement, freqLevel);
                currentPhase = incrementWrapPhase(currentPhase, phaseIncrement);
            }
            
            _phase = currentPhase;
        }

    private:
        float _phase;
    };
    
}

#endif /* SawtoothOscillatorBL_h */
