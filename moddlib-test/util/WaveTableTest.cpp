//
//  WaveTableTest.cpp
//  moddlib-test
//
//  Created by Daniel Doubleday on 11/25/17.
//  Copyright © 2017 Daniel Doubleday. All rights reserved.
//

#include <cassert>
#include "catch.hpp"

#include "aot_moddlib.hpp"
#include "util/WaveTable.hpp"
#include "units/WaveTableOscillator.hpp"

using namespace moddlib;

#ifndef DISABLE_TEST

TEST_CASE("WaveTable")
{
    WaveTable2D waveTable2d;
    
    WaveTableOscillator oscillator;
}

#endif
