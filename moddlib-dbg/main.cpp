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
#include "units/FourPoleFilter.hpp"
#include "units/VCA.hpp"
#include "concepts/GeneratorRef.hpp"
#include "util/WaveTable.hpp"
#include "util/FIR.hpp"

//==============================================================================
/******************************* SOURCE LICENSE *********************************
Copyright (c) 2015 MicroModeler.

A non-exclusive, nontransferable, perpetual, royalty-free license is granted to the Licensee to
use the following Information for academic, non-profit, or government-sponsored research purposes.
Use of the following Information under this License is restricted to NON-COMMERCIAL PURPOSES ONLY.
Commercial use of the following Information requires a separately executed written license agreement.

This Information is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

******************************* END OF LICENSE *********************************/

// A commercial license for MicroModeler DSP can be obtained at http://www.micromodeler.com/launch.jsp

// Begin header file, filter1.h

#ifndef FILTER1_H_ // Include guards
#define FILTER1_H_

static const int filter1_length = 20;
extern float filter1_coefficients[20];

typedef struct
{
    float * pointer;
    float state[40];
    float output;
} filter1Type;


filter1Type *filter1_create( void );
void filter1_destroy( filter1Type *pObject );
 void filter1_init( filter1Type * pThis );
 void filter1_reset( filter1Type * pThis );
#define filter1_writeInput( pThis, input )  \
    filter1_filterBlock( pThis, &input, &pThis->output, 1 );

#define filter1_readOutput( pThis )  \
    pThis->output

 int filter1_filterBlock( filter1Type * pThis, float * pInput, float * pOutput, unsigned int count );
#define filter1_outputToFloat( output )  \
    (output)

#define filter1_inputFromFloat( input )  \
    (input)

 void filter1_dotProduct( float * pInput, float * pKernel, float * pAccumulator, short count );
#endif // FILTER1_H_

/******************************* SOURCE LICENSE *********************************
Copyright (c) 2015 MicroModeler.

A non-exclusive, nontransferable, perpetual, royalty-free license is granted to the Licensee to
use the following Information for academic, non-profit, or government-sponsored research purposes.
Use of the following Information under this License is restricted to NON-COMMERCIAL PURPOSES ONLY.
Commercial use of the following Information requires a separately executed written license agreement.

This Information is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

******************************* END OF LICENSE *********************************/

// A commercial license for MicroModeler DSP can be obtained at http://www.micromodeler.com/launch.jsp

#include <stdlib.h> // For malloc/free
#include <string.h> // For memset




//==============================================================================

using namespace moddlib;

void testWaveTable()
{
    WaveTable waveTable(1024);
    std::vector<float> partials = {1, 0.5, 0.3};
    waveTable.fillSinesum(partials, 3);
    
    uint numSamples = 512;
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

template <typename ScalerT>
void testWaveTable2D(std::string const & name, uint oversampling = 1)
{
    WaveTable2D waveTable;
    auto partials = waveTable::sawPartials(20);
    waveTable.setupTables<ScalerT>(40, partials, oversampling);
    
    std::vector<float> memory;
    
    float startF = 20;
    float endF = Engine::instance().getFrameRate() / 2;
    float phase = -1;
    float freq = startF;
    float freqRamp = 1.00001f;
    while (freq < endF)
    {
        auto deltaPhase = Engine::instance().convertFrequencyToPhaseIncrement(freq);
        memory.push_back(waveTable.lookup(phase, freq));
        phase += deltaPhase;
        freq *= freqRamp;
        if (phase >= 1)
        {
            phase -= 2;
        }
    }
    
    dbg::dumpBuffer(name + ".out", memory.data(), memory.size());
}

void oversamplingTest()
{
//    Engine::instance().setFrameRate(Engine::DEFAULT_FRAMERATE * 2);
    
    WaveTable2D waveTable;
    auto partials = waveTable::sawPartials(20);
    waveTable.setupTables<HannPartialScaler>(20, partials);

    std::vector<float> memory;
    
    float startF = 20;
    float endF = Engine::DEFAULT_FRAMERATE / 2;
    float phase = -1;
    float freq = startF;
    float freqRamp = 1.00001f;
    while (freq < endF)
    {
        auto deltaPhase = Engine::instance().convertFrequencyToPhaseIncrement(freq);
        memory.push_back(waveTable.lookup(phase, freq));
        phase += deltaPhase;
        freq *= freqRamp;
        if (phase >= 1)
        {
            phase -= 2;
        }
    }
    
    FIR<2> fir;
    fir.setCoefs({
    0.5,0.5
    });
    
    std::vector<float> downsampleBuffer(memory.size() / 2);
    for (uint i = 0; i < memory.size(); i += 1)
    {
        auto filtered = fir.filter(memory[i]);
        memory[i] = filtered;
//        downsampleBuffer[i] = memory[i];
        if (i % 2 == 0)
        {
            downsampleBuffer[i / 2] = filtered;
        }
    }

    dbg::dumpBuffer("oversampling3.out", downsampleBuffer.data(), downsampleBuffer.size());
    dbg::dumpBuffer("memory.out", memory.data(), memory.size());
}

void testWindows()
{
    auto partials = waveTable::getMaxHarmonic(20);
    std::vector<float> window;
    window.reserve(partials);
    
    HammingPartialScaler hamming;
    for (auto partial = 0; partial < partials; ++partial)
    {
        auto scaledPartial = hamming(partial, partials);
        std::cout << partial << " " << scaledPartial << "\n";
        window.push_back(scaledPartial);
    }
    
    dbg::dumpBuffer("hamming.out", window.data(), partials);
    
    window.clear();
    HannPartialScaler hann;
    for (auto partial = 0; partial < partials; ++partial)
    {
        auto scaledPartial = hann(partial, partials);
        window.push_back(scaledPartial);
    }
    
    dbg::dumpBuffer("hann.out", window.data(), partials);
}

int main()
{
//    Engine::instance().setFrameRate(Engine::DEFAULT_FRAMERATE * 2);
    oversamplingTest();
//    testWaveTable2D<IdentityPartialScaler>("identity1");
//    testWaveTable2D<HammingPartialScaler>("hamming1");
//    testWaveTable2D<HannPartialScaler>("hann1");
    
//    testWaveTable2D<IdentityPartialScaler>("identity2", 2);
//    testWaveTable2D<HammingPartialScaler>("hamming2", 2);
//    testWaveTable2D<HannPartialScaler>("hann2", 2);

    return 0;
}
