//
//  ADSREnvelopeTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 1/25/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>

#include <stdio.h>

#include <stdio.h>
#include "catch.hpp"

#include "SineOscillator.hpp"
#include "TestSynth.h"
#include "ADSREnvelopeGenerator.hpp"

#ifndef DISABLE_TEST

using namespace moddlib;

SCENARIO("ADSREnvelopeTest")
{    
    GIVEN("an envelope")
    {
        ADSREnvelope envelope;
        
        WHEN("it was not reset")
        {
            THEN("it produces zero output")
            {
                AlignedMemory buffer(8);
                CHECK(envelope.fill(8, buffer) == 0);
            }
            
            THEN("zeroes destination")
            {
                AlignedMemory buffer(8);
                memset(buffer, 13, 8);
                envelope.generate(8, buffer);
                buffer.forEach([](auto i, float& v) {
                    CHECK(v == 0);
                });
            }
        }
        
        WHEN("it was reset")
        {
            THEN("it generates output")
            {
                AlignedMemory buffer(8);
                envelope.reset();
                CHECK(envelope.isPlaying());
                CHECK(envelope.fill(8, buffer) > 0);
            }
        }
    }
}

#endif
