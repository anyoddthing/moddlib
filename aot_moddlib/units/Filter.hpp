//
//  Filter.hpp
//  moddlib
//
//  Created by Daniel Doubleday on 23.04.18.
//  Copyright © 2018 Daniel Doubleday. All rights reserved.
//

#pragma once

#include "aot_simdlib/aot_simdlib.hpp"
#include "../core/core.hpp"

namespace moddlib
{
    template <typename FilterT, typename InputT>
    struct Filter :
        Generator<Filter<FilterT>>,
        InputBank<
            Inputs<2, InputT>>,
        OutputBank<
            Outputs<1, StreamOutput>>
    {
        using freqIn = BIn_<0, 0>;
        using qIn = BIn_<0, 1>;
        
        void doGenerate()
        {
            doGenerate<InputT>();
        }
        
        template <typename T>
        void doGenerate(std::enable_if_t<std::is_same<T, StreamInput>::value>* = 0)
        {
        
        }
        
        template <typename T>
        void doGenerate(std::enable_if_t<std::is_same<T, FloatInput>::value>* = 0)
        {
        
        }
        
    private:
        FilterT _filter;
    };
}
