//
//  FFTTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 1/6/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//


#include <stdio.h>
#include <cmath>

#include "catch.hpp"

#include "core.hpp"
#include "FFT.hpp"

#ifdef DISABLE_TEST

using namespace usyn;

#define N 1024

void generateSin(float* dest, uint numPeriods)
{
    for (size_t n = 0; n < N; ++n)
    {
        dest[n] =
            0.5f * std::cos(2 * simd::pi<float> * numPeriods * static_cast<float>(n)/N)
//            ;
             +
            0.5f * std::cos(2 * simd::pi<float> * 2 * numPeriods * static_cast<float>(n)/N);
    }
}


TEST_CASE("FFT Test")
{
    SECTION("Can parse message")
    {
    
        AlignedMemory audioIn(1024);
        AlignedBuffer<N / 2> freqOut;
        
        generateSin(audioIn, 5);
        
        FFT<N> fft;
        fft.computeFFT(audioIn, freqOut);
        
        freqOut.forEach([](int n, fref sample)
        {
            std::cout << n << " " << sample << "\n";
        });
    }
}

#endif // DISABLE_TEST
