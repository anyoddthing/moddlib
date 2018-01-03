//
//  main.cpp
//  moddlib-dbg
//
//  Created by Daniel Doubleday on 12/17/17.
//  Copyright © 2017 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>
#include <iostream>

#include "aot_moddlib.hpp"
#include "util/DebugDumpHost.hpp"
#include "units/WaveTableOscillator.hpp"
#include "concepts/GeneratorRef.hpp"
#include "util/WaveTable.hpp"

using namespace moddlib;

void testWaveTable()
{
    WaveTable<> waveTable(1024);
    std::vector<float> partials = {1, 0.5, 0.3};
    waveTable.fillSinesum(partials, 3);
    
    uint32_t numSamples = 512;
    AlignedMemory memory(numSamples);
    
    float deltaPhase = 2.0f / numSamples;
    float phase = -1;
    for (auto i = 0; i < numSamples; ++i)
    {
        memory[i] = waveTable.lookup(phase);
        phase += deltaPhase;
    }
    
    dbg::dumpBuffer("test.out", memory.data(), numSamples);
}

void testWaveTable2D()
{
    WaveTable2D<> waveTable;
    std::vector<float> partials = {1};
    waveTable.setupTables(440, partials);
    
    uint32_t numSamples = Engine::instance().getFrameRate();
    std::cout << "fr = " << numSamples << "\n";
    AlignedMemory memory(numSamples);
    
    float deltaPhase = Engine::instance().convertFrequencyToPhaseIncrement(440);
    float phase = -1;
    for (auto i = 0; i < numSamples; ++i)
    {
        memory[i] = waveTable.lookup(phase, 440);
        phase += deltaPhase;
        if (phase >= 1)
        {
            phase -= 2;
        }
    }
    
    dbg::dumpBuffer("test.out", memory.data(), numSamples);
}

int main()
{
    testWaveTable2D();
    return 0;
}
