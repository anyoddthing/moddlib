//
//  TestSynth.h
//  unitsynth
//
//  Created by Daniel Doubleday on 15/07/15.
//  Copyright (c) 2015 Daniel Doubleday. All rights reserved.
//

#ifndef __unitsynth__TestSynth__
#define __unitsynth__TestSynth__

#include <stdio.h>
#include "aot_moddlib.hpp"

namespace moddlib
{

    class TestGenerator :
        public Generator<TestGenerator>,
        public InputBank<Inputs<2, StreamInput>>,
        public SingleStreamOutput
    {
    public:
        using freqIn = BIn_<0, 0>;

        void doGenerate()
        {
            output<mainOut>().buffer().copy(input<freqIn>().buffer().data());
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Oscillators

    /**
     * Generate
     */
    struct DCOffsetGenerator :
        public Generator<DCOffsetGenerator>,
        public InputBank<Inputs<1, DefaultInput>>,
        public SingleStreamOutput
    {
        using offsetIn = BIn_<0, 0>;

        void doGenerate()
        {
            auto val = input<offsetIn>().getValue();
            output<mainOut>().buffer().fill(val);
        }
    };

    struct TestOscillator : DCOffsetGenerator
    {
        static const float dcOffset;

        TestOscillator()
        {
            input<offsetIn>().setup(dcOffset);
        }
    };
    
    struct TestCircuit :
        public Circuit<TestCircuit, TestOscillator>,
        public InputBank<
            Inputs<1, DefaultInput>>,
        public OutputBank<
            Outputs<1, StreamOutputAlias>
        >
    {
        TestCircuit()
        {
            output<mainOut>().setOutput(moduleOut<Mod_<0>, TestOscillator::mainOut>());
        }
    };

    template <typename CircuitT>
    struct TestSynth
    {
        void generate()
        {
            circuit.generate();
        }

        Engine engine;
        CircuitT circuit;
        Voice voice;
    };

}



#endif /* defined(__unitsynth__TestSynth__) */
