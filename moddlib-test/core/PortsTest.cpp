//
//  PortsTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 19/09/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"

#include "aot_moddlib.hpp"
#include "TestSynth.h"

#ifndef DISABLE_TEST

using namespace moddlib;

struct GatedGenerator :
    public Generator<GatedGenerator>,
    public InputBank<Inputs<1, FloatInput>>,
    public SingleStreamOutput
{
    using gateIn = BIn_<0, 0>;
    
    void doGenerate()
    {
        if (input<gateIn>().isUp())
        {
            output<mainOut>().buffer().fill(0.5f);
        }
        else
        {
            output<mainOut>().buffer().zero_mem();
        }
    }
};

struct GatedSynth :
    public TestSynth<GatedGenerator>
{
    
};

TEST_CASE("Ports Test")
{
    SECTION("Trigger")
    {
        GatedSynth synth;
        FloatOutput output;
        
        connect(output, synth.circuit.input<GatedGenerator::gateIn>());
        
        auto& out = synth.circuit.output<GatedGenerator::mainOut>();
        synth.generate();
        
        out.buffer().forEach([](int i, float& val) {
            CHECK(val == 0);
        });
        
        output.up();
        synth.generate();
        
        out.buffer().forEach([](uint32_t i, float& val) {
            CHECK(val == Approx(0.5));
        });
    
        output.down();
        synth.generate();
        
        out.buffer().forEach([](int i, float& val) {
            CHECK(val == 0);
        });
    }
}

#endif
