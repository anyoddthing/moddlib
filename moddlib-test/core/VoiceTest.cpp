//
//  VoiceTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 24/09/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"

#include "moddlib.hpp"
#include "TestSynth.h"

#ifndef DISABLE_TEST

using namespace moddlib;

TEST_CASE("Voice Test")
{
    
    SECTION("Test Circuit")
    {        
        TestSynth<TestCircuit> testSynth;
        
        testSynth.generate();
        auto& outBuffer = testSynth.circuit.output<TestCircuit::mainOut>().buffer();
        for (size_t i = 0; i < outBuffer.size(); ++i)
        {
            CHECK(outBuffer[i] == 0.5f);
        }
    }
}

#endif
