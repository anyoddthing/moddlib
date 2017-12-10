//
//  ScratchTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 27/06/15.
//  Copyright (c) 2015 Daniel Doubleday. All rights reserved.
//

#include <cassert>
#include "catch.hpp"

#include "aot_moddlib.hpp"
#include "TestSynth.h"


#ifndef DISABLE_TEST

using namespace moddlib;

TEST_CASE("Scratch Test")
{
//    SECTION("Scratch Pad Test")
//    {
//        TestGenerator testGenerator;
//        auto& input = testGenerator.getInput(0);
//        auto buffer = &input.buffer();
//        
//        testGenerator.generatorGenerate();
//        
//        for (size_t i = 0; i < SampleBuffer::size; ++i)
//        {
//            CHECK((*buffer)[i] == 0);
//        }
//        
//        ConnectableOutput output;
//        output.buffer().fill(0.5f);
//        input.connect(&output);
//        
//        testGenerator.generatorGenerate();
//        
//        buffer = &input.buffer();
//        for (size_t i = 0; i < SampleBuffer::size; ++i)
//        {
//            CHECK((*buffer)[i] == 0.5f);
//        }
//    }
    
}

#endif
