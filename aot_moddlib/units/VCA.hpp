//
//  VCA.hpp
//  moddlib
//
//  Created by Daniel Doubleday on 1/13/18.
//  Copyright © 2018 Daniel Doubleday. All rights reserved.
//

#pragma once

#include "../core/core.hpp"

namespace moddlib
{
    template <uint Channels>
    struct VCA :
        Generator<VCA<Channels>>,
        InputBank<
            Inputs<Channels, StreamInput>,
            Inputs<Channels, DefaultInput>,
            Inputs<Channels, StreamInput>>,
        OutputBank<
            Outputs<2, StreamOutput>>
    {
        using signalIn = Bank_<0>;
        using ampIn = Bank_<1>;
        using envIn = Bank_<2>;

        void init()
        {
            for (auto i = 0; i < Channels; ++i)
            {
                this->template inputs<ampIn>()[i].setup(1);
            }
        }
        
        void doGenerate()
        {
            
        }
        
    };

}
