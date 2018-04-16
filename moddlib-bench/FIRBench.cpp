//
//  FIRBench.cpp
//  moddlib-bench
//
//  Created by Daniel Doubleday on 4/15/18.
//  Copyright © 2018 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>

#include <chrono>
#include "bench.hpp"
#include "util/FIR.hpp"

#define FILTER_TAP_NUM 40
static float filter_taps[FILTER_TAP_NUM] = {
  0.0019390782882137887,
  0.0032384617106349716,
  -0.0005598729703174639,
  -0.006658539329411668,
  -0.00441065171178432,
  0.005836241148952741,
  0.006321788895877844,
  -0.008621820501626892,
  -0.013638193706077547,
  0.00735534817882479,
  0.02125296166861005,
  -0.005352091275918336,
  -0.03351422027330926,
  -0.0021817362402696363,
  0.05008216981174255,
  0.0177217186563712,
  -0.0799491618225474,
  -0.06008679798019613,
  0.17134096520779674,
  0.4237090613389178,
  0.4237090613389178,
  0.17134096520779674,
  -0.06008679798019613,
  -0.0799491618225474,
  0.0177217186563712,
  0.05008216981174255,
  -0.0021817362402696363,
  -0.03351422027330926,
  -0.005352091275918336,
  0.02125296166861005,
  0.00735534817882479,
  -0.013638193706077547,
  -0.008621820501626892,
  0.006321788895877844,
  0.005836241148952741,
  -0.00441065171178432,
  -0.006658539329411668,
  -0.0005598729703174639,
  0.0032384617106349716,
  0.0019390782882137887
};

struct FIRBench
{
    static FIRBench instance;
    
    FIRBench() : buffer(bufferSize)
    {
        using namespace moddlib;
        
        fir1.setCoefs(filter_taps);
        fir2.setCoefs(filter_taps);
        
        std::generate(buffer.begin(), buffer.end(), [] ()
        {
            return std::rand() * 2.0f / RAND_MAX - 1;
        });
    }
    
    void test1(benchpress::context* ctx)
    {
        for (float val : buffer)
        {
            fir1.filter2(val);
        }
    }

    void test2(benchpress::context* ctx)
    {
        for (float val : buffer)
        {
            fir2.filter(val);
        }
    }

private:
    uint bufferSize = 1024;
    moddlib::AlignedMemory buffer;
    moddlib::FIR<FILTER_TAP_NUM> fir1;
    moddlib::FIR<FILTER_TAP_NUM> fir2;
};

FIRBench FIRBench::instance;

BENCHMARK("FIR Test1", [](benchpress::context* ctx)
{
    BENCH_ITER FIRBench::instance.test1(ctx);
});

BENCHMARK("FIR Test2", [](benchpress::context* ctx)
{
    BENCH_ITER FIRBench::instance.test2(ctx);
});
