//
//  moddlib-test.h
//  moddlib
//
//  Created by Daniel Doubleday on 22.04.18.
//  Copyright © 2018 Daniel Doubleday. All rights reserved.
//

#pragma once

#include <cassert>
#include <random>

#include "catch.hpp"

#include "aot_moddlib.hpp"
#include "util/FIR.hpp"

Approx Approx20bit(double value)
{
    return Approx(value).margin(1.0f/(1 << 20));
}

