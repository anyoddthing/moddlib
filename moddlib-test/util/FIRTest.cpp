//
//  WaveTableTest.cpp
//  moddlib-test
//
//  Created by Daniel Doubleday on 04/15/18.
//  Copyright © 2017 Daniel Doubleday. All rights reserved.
//

#include <cassert>
#include <random>

#include "catch.hpp"

#include "aot_moddlib.hpp"
#include "util/FIR.hpp"

using namespace moddlib;

#ifdef DISABLE_TEST

/*

FIR filter designed with
http://t-filter.appspot.com

sampling frequency: 88000 Hz

* 0 Hz - 18000 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = 0.21456249116052417 dB

* 24000 Hz - 44000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -65.61939877550482 dB

*/

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

TEST_CASE("FIR")
{
    SECTION("Compare versions")
    {
        uint bufferSize = 1024;
        AlignedMemory buffer(bufferSize);
        std::generate(buffer.begin(), buffer.end(), [] ()
        {
            return std::rand() * 2.0f / RAND_MAX - 1;
        });
        
        FIR<FILTER_TAP_NUM> fir1;
        fir1.setCoefs(filter_taps);
        
        FIR<FILTER_TAP_NUM> fir2;
        fir2.setCoefs(filter_taps);
        
        for (float val : buffer)
        {
            CHECK(val >= -1.0f);
            CHECK(val <= 1.0f);
            
            auto firval1 = fir1.filter(val);
            auto firval2 = fir2.filter(val);
            
            CHECK(firval1 == firval2);
        }
    }
}

#endif
