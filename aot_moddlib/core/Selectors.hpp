//
//  Ports.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 29/07/15.
//  Copyright (c) 2015 Daniel Doubleday. All rights reserved.
//

#pragma once

#include "Memory.hpp"

namespace moddlib
{
    template <size_t Pos>
    struct Out_
    {
        enum { pos = Pos };
    };
    
    template <size_t Pos>
    struct In_
    {
        enum { pos = Pos };
    };

    template <int Pos>
    struct Mod_
    {
        enum { pos = Pos };
    };

    template <size_t Bank, size_t Pos>
    struct BIn_
    {
        enum { bank = Bank };
        enum { pos = Pos };
    };

    template <size_t Bank, size_t Pos>
    struct BOut_
    {
        enum { bank = Bank };
        enum { pos = Pos };
    };
}
