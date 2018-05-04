//
//  SelectorsTest.cpp
//  moddlib-test
//
//  Created by Daniel Doubleday on 04.05.18.
//  Copyright © 2018 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"

#include "aot_moddlib.hpp"
#include "TestSynth.h"

#ifndef DISABLE_TEST

using namespace moddlib;

TEST_CASE("Selectors")
{
    SECTION("Append selectors to list")
    {
        CHECK(std::is_same<
            Sel_<Sel_<Mod_<0>, Mod_<1>>, Mod_<2>>,
            Sel_<Mod_<0>, Mod_<1>, Mod_<2>>>::value);
    }

    SECTION("Prepend selectors to list")
    {
        CHECK(std::is_same<
            Sel_<Mod_<0>, Sel_<Mod_<1>, Mod_<2>>>,
            Sel_<Mod_<0>, Mod_<1>, Mod_<2>>>::value);
    }

    SECTION("Concatenate 2 selector lists")
    {
        CHECK(std::is_same<
            Sel_<Sel_<Mod_<0>, Mod_<1>>, Sel_<Mod_<2>, Mod_<3>>>,
            Sel_<Mod_<0>, Mod_<1>, Mod_<2>, Mod_<3>>>::value);
    }
    
    SECTION("Create list from mixed elements")
    {
        CHECK(std::is_same<
            Sel_<Mod_<0>, Sel_<Mod_<1>>, Mod_<2>, Sel_<Mod_<3>, Mod_<4>>, Mod_<5>>,
            Sel_<Mod_<0>, Mod_<1>, Mod_<2>, Mod_<3>, Mod_<4>, Mod_<5>>>::value);
    }

}

#endif
