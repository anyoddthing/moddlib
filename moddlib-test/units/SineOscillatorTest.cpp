//
//  MidiTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 14/11/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"

#include "SineOscillator.hpp"
#include "TestSynth.h"

#ifndef DISABLE_TEST

using namespace moddlib;

TEST_CASE("SineOscillator Test")
{
    
    SECTION("")
    {
        TestSynth<SineOscillator> testSynth;
        
        
        
    }
}

#endif // DISABLE_TEST
