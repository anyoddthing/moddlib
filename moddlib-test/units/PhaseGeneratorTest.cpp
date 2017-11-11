//
//  PhaseGeneratorTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 22/11/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"

#include "SineOscillator.hpp"
#include "TestSynth.h"
#include "PhaseGenerator.hpp"

#ifndef DISABLE_TEST

using namespace moddlib;

TEST_CASE("PhaseGenerator Test")
{
    
    SECTION("")
    {
        PhaseGenerator generator;
    }
}

#endif // DISABLE_TEST

