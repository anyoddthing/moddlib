//
//  DbgCreateSine.h
//  moddlib
//
//  Created by Daniel Doubleday on 30.04.18.
//  Copyright © 2018 Daniel Doubleday. All rights reserved.
//

#pragma once

#include "units/SineOscillator.hpp"
#include "instruments/SubtractiveSynth.hpp"


void createSine()
{
    simd::AlignedMemory memory(64000);
    SubtractiveSynth<SineOscillator> synth;
    SampleBuffer sampleBuffer;
    
    synth.queueMessage(moddlib::midi::noteOnMessage(60, 128));
    for (auto i = 0; i < memory.size(); i += sampleBuffer.size())
    {
        synth.generate(sampleBuffer);
        simdv::memcopy(&memory[i], sampleBuffer.data());
    }
    
    dbg::dumpBuffer("sine.out", memory.data(), memory.size());
}
