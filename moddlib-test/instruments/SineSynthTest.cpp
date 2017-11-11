//
//  SineSynthTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 22/11/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"

#include "SineOscillator.hpp"
#include "TestSynth.h"
#include "SineSynth.hpp"

#ifndef DISABLE_TEST

using namespace moddlib;

SCENARIO("SineSynth")
{    
    GIVEN("a synth")
    {
        Voice voice;
        SineSynthCircuit envelope;
    }
}

#endif

