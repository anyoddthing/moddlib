//
//  bench.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 1/10/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#ifndef bench_h
#define bench_h

#define BENCH_ITER for (size_t i = 0; i < ctx->num_iterations(); ++i)

#include "benchpress/benchpress.hpp"

#include <chrono>
#include <iostream>
#include <array>

#include "aot_simdlib/aot_simdlib.hpp"

#define NOINLINE __attribute__ ((noinline))

#include "aot_moddlib.hpp"

#endif /* bench_h */
