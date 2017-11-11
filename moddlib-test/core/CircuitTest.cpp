//
//  CircuitTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 22/11/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>

#include <stdio.h>
#include "catch.hpp"

#include "moddlib.hpp"
#include "TestSynth.h"

#ifndef DISABLE_TEST

using namespace moddlib;

struct CircuitWithInputs :
    public Circuit<CircuitWithInputs, DCOffsetGenerator>,
    public InputBank<Inputs<1, FloatPort>>,
    public OutputBank<Outputs<1, StreamOutputAlias>>
{
    using offsetIn = BIn_<0, 0>;
    
    CircuitWithInputs()
    {
//        connect(input<offsetIn>(), module<Mod_<0>>().input<DCOffsetGenerator::offsetIn>());
        connect(input<offsetIn>(), moduleIn<Mod_<0>, DCOffsetGenerator::offsetIn>());
        
        output<mainOut>().setOutput(module<Mod_<0>>().output<DCOffsetGenerator::mainOut>());
    }
};

TEST_CASE("Circuit Test")
{
    SECTION("Inputs")
    {
        Engine engine;
        FloatOutput output;
        output.setValue(0.1f);
        CircuitWithInputs circuit;
        
        connect(output, circuit.input<CircuitWithInputs::offsetIn>());
        circuit.generate();
        
        circuit.output<CircuitWithInputs::mainOut>().buffer().forEach([](auto i, auto& val)
        {
            CHECK(val == Approx(0.1f));
        });
    }
}

#endif
