//
//  SawtoothTable.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 2/20/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#ifndef SawtoothTable_hpp
#define SawtoothTable_hpp

#include <stdio.h>
#include <array>

#include "../core/core.hpp"

namespace moddlib
{
    struct SawtoothTable
    {
        static SawtoothTable& instance()
        {
            static SawtoothTable table;
            return table;
        }
        
        static float convertPhaseIncrementToLevel(float phaseIncrement)
        {
//            return -1.0f - approxLog2(std::max(phaseIncrement, 1.0e-30f));
            return -1.0f - std::log2(std::max(phaseIncrement, 1.0e-30f));
        }
        
        SawtoothTable();
        
        float calculateSawtooth(float currentPhase, float positivePhaseIncrement, float freqLevel)
        {
            /* Use Phase to determine sampleIndex into table. */
            auto findex = ((PHASE_SCALAR * currentPhase) + PHASE_SCALAR);
            
            // findex is > 0 so we do not need to call floor().
            auto sampleIndex = static_cast<int>(findex);
            auto tableIndex = static_cast<int>(freqLevel);
            
            auto horizontalFraction = findex - sampleIndex;

            float val;
            if (tableIndex > (NUM_TABLES - 2))
            {
                /*
                 * Just use top table and mix with arithmetic sawtooth if below lowest frequency.
                 * Generate new fraction for interpolating between 0.0 and lowest table frequency.
                 */
                auto fraction = positivePhaseIncrement * LOWEST_PHASE_INC_INV;
                fref tableBase = _tables[(NUM_TABLES - 1)];

                /* Get adjacent samples. Assume guard point present. */
                auto sam1 = tableBase[sampleIndex];
                auto sam2 = tableBase[sampleIndex + 1];
                /* Interpolate between adjacent samples. */
                auto loSam = sam1 + (horizontalFraction * (sam2 - sam1));

                /* Use arithmetic version for low frequencies. */
                /* fraction is 0.0 at 0 Hz */
                val = currentPhase + (fraction * (loSam - currentPhase));
            }
            else
            {
                auto verticalFraction = freqLevel - tableIndex;
                if (tableIndex < 0)
                {
                    if (tableIndex < -1) // above Nyquist!
                    {
                        val = 0.0;
                    }
                    else
                    {
                        /*
                         * At top of supported range, interpolate between 0.0 and first partial.
                         */
                        fref tableBase = _tables[0]; /* Sine wave table. */

                        /* Get adjacent samples. Assume guard point present. */
                        auto sam1 = tableBase[sampleIndex];
                        auto sam2 = tableBase[sampleIndex + 1];

                        /* Interpolate between adjacent samples. */
                        auto hiSam = sam1 + (horizontalFraction * (sam2 - sam1));
                        /* loSam = 0.0 */
                        // verticalFraction is 0.0 at Nyquist
                        val = verticalFraction * hiSam;
                    }
                }
                else
                {
                    // Interpolate between adjacent levels to prevent harmonics from popping.
                     
                    /* Get adjacent samples. Assume guard point present. */
                    auto sam1High = _tables[tableIndex + 1][sampleIndex];
                    auto sam2High = _tables[tableIndex + 1][sampleIndex + 1];

                    /* Interpolate between adjacent samples. */
                    auto hiSam = sam1High + (horizontalFraction * (sam2High - sam1High));

                    /* Get adjacent samples. Assume guard point present. */
                    auto sam1Low = _tables[tableIndex][sampleIndex];
                    auto sam2Low = _tables[tableIndex][sampleIndex + 1];

                    /* Interpolate between adjacent samples. */
                    auto loSam = sam1Low + (horizontalFraction * (sam2Low - sam1Low));

                    val = loSam + (verticalFraction * (hiSam - loSam));
                }
            }
            return val;
        }

        static constexpr int NUM_TABLES = 8;
        static constexpr int CYCLE_SIZE = (1 << 10);
        static constexpr int TABLE_SIZE = CYCLE_SIZE + 1;
        
        /*****************************************************************************
         * When the phaseInc maps to the highest level table, then we start interpolating between the
         * highest table and the raw sawtooth value (phase). When phaseInc points to highest table:
         * flevel = NUM_TABLES - 1 = -1 - log2(pInc); log2(pInc) = - NUM_TABLES pInc = 2**(-NUM_TABLES)
         */
        static constexpr float LOWEST_PHASE_INC_INV = (1 << NUM_TABLES);
        
        static constexpr float PHASE_SCALAR = CYCLE_SIZE * 0.5f;
        
    private:
        std::array<simd::AlignedBuffer<TABLE_SIZE>, NUM_TABLES> _tables;
    };
}
#endif /* SawtoothTable_hpp */
