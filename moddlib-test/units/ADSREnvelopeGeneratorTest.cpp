//
//  ADSREnvelopeGenerator.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 1/23/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>

#include <stdio.h>
#include "catch.hpp"

#include "SineOscillator.hpp"
#include "TestSynth.h"
#include "ADSREnvelopeGenerator.hpp"

#ifndef DISABLE_TEST

using namespace moddlib;

SCENARIO("ADSREnvelopeGeneratorTest")
{    
//    GIVEN("an envelope without inputs")
//    {
//        Voice voice;
//        ADSREnvelopeGenerator envelope;
//        
//        WHEN("it was not triggered")
//        {
//            THEN("it produces zero output")
//            {
//                envelope.generate();
//                envelope.output<ADSREnvelopeGenerator::envOut>().buffer().forEach([](int i, float val)
//                {
//                    CHECK( val == 0);
//                });
//            }
//        }
//        
//        WHEN("it was triggered")
//        {
//            FloatOutput output;
//            connect(output, envelope.input<ADSREnvelopeGenerator::triggerIn>());
//            
//            output.up();
//            THEN("it produces value > 0 after 2nd frame")
//            {
//                envelope.generate();
//                envelope.generate();
//                
//                envelope.output<ADSREnvelopeGenerator::envOut>().buffer().forEach([](int i, float val)
//                {
//                    CHECK( val > 0);
//                });
//            }
//        }
//    }
}

#endif
