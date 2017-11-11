//
//  UtilitiesTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 12/23/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"

#include "aot_moddlib.hpp"
#include "TestSynth.h"

#ifndef DISABLE_TEST

using namespace moddlib;

TEST_CASE("Function Port Tests")
{
    SECTION("A function port calculates output")
    {
        struct Adder
        {
            float operator()(float val)
            {
                return val + toAdd;
            }

            float toAdd;
        };

        FloatOutput outputPort;
        FloatInput inputPort;
        
        connect(outputPort, inputPort);
        
        auto functionPort = makeFunctionPort(inputPort, Adder { 2 });
        
        CHECK(functionPort.getValue() == Approx(2 + 0));
        
        outputPort.setValue(0.3f);
        CHECK(functionPort.getValue() == Approx(2 + 0.3f));
    }
}

#endif
