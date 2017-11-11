//
//  SawtoothTable.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 2/20/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#include "SawtoothTable.hpp"

namespace moddlib
{
    SawtoothTable::SawtoothTable()
    {
        fref sineTable = _tables[0];
        /* Fill initial sine table with values for -PI to PI. */
        for (int j = 0; j < TABLE_SIZE; ++j)
        {
            auto rad = 2 * moddlib::pi<float> * static_cast<float>(j) / static_cast<float>(CYCLE_SIZE);
            sineTable[j] = std::sin(rad - moddlib::pi<float>);
        }
        
        /*
         * Build each table from scratch and scale partials by raised cosine* to eliminate Gibbs
         * effect.
         */
        for (int i = 1; i < NUM_TABLES; ++i)
        {
            fref table = _tables[i];
            /* Add together partials for this table. */
            auto numPartials = 1 << (i + 1);
            auto kGibbs = moddlib::pi<float> / (2 * numPartials);
            for (int k = 0; k < numPartials; k++)
            {
                auto sineIndex = 0;
                auto partial = k + 1;
                auto cGibbs = std::cos(k * kGibbs);
                /* Calculate amplitude for Nth partial */
                auto ampl = cGibbs / partial;

                for (int j = 0; j < TABLE_SIZE; j++)
                {
                    table[j] += ampl * sineTable[sineIndex];
                    sineIndex += partial;
                    /* Wrap index at end of table.. */
                    if (sineIndex >= CYCLE_SIZE)
                    {
                        sineIndex -= CYCLE_SIZE;
                    }
                }
            }
        }
        
        for (int i = 1; i < NUM_TABLES; ++i)
        {
            normalizeArray(_tables[i], TABLE_SIZE);
        }
    }
}
