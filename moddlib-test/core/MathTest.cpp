//
//  MathTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 05/10/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//


#include <stdio.h>
#include "catch.hpp"

#include "Memory.hpp"
#include "Math.hpp"

#ifndef DISABLE_TEST

TEST_CASE("Normalze Array")
{
    SECTION("Normalize Array with positive max")
    {
        std::array<float, 5> floats = {0, 0.1, 0.2, 0.5, 1.2};
        std::array<float, 5> normalized;
        
        std::transform(floats.begin(), floats.end(), normalized.begin(),
            [](fref val) { return val / 1.2f; });
        
        moddlib::normalizeArray(floats.data(), 5);
        
        for (auto i = 0; i < 5; ++i)
        {
            CHECK(floats[i] == Approx(normalized[i]));
        }
    }
    
    SECTION("Normalize Array with negative max")
    {
        std::array<float, 5> floats = {0, 0.1, 0.2, -1.2, 0.5};
        std::array<float, 5> normalized;
        
        std::transform(floats.begin(), floats.end(), normalized.begin(),
            [](fref val) { return val / 1.2f; });
        
        moddlib::normalizeArray(floats.data(), 5);
        
        for (auto i = 0; i < 5; ++i)
        {
            CHECK(floats[i] == Approx(normalized[i]));
        }
    }
    
    SECTION("Normalize Array with sub 1.0")
    {
        std::array<float, 5> floats = {0.8, 0, 0.1, 0.2, 0.5};
        std::array<float, 5> normalized;
        
        std::transform(floats.begin(), floats.end(), normalized.begin(),
            [](fref val) { return val / 0.8f; });
        
        moddlib::normalizeArray(floats.data(), 5);
        
        for (auto i = 0; i < 5; ++i)
        {
            CHECK(floats[i] == Approx(normalized[i]));
        }
    }


    SECTION("Can round to next power of 2")
    {
        CHECK(moddlib::roundUpToPower2(3) == 4);
        
        // check usage as constexpr
        std::integral_constant<uint, moddlib::roundUpToPower2(7)> tc;
        CHECK(tc.value == 8);
    }
}

#endif
