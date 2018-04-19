//
// Created by Daniel Doubleday on 04/07/15.
// Copyright (c) 2015 Daniel Doubleday. All rights reserved.
//

#ifndef UNITSYNTH_ALIGNEDBUFFER_HPP
#define UNITSYNTH_ALIGNEDBUFFER_HPP

#include <array>

#include "aot_simdlib/aot_simdlib.hpp"

#include "Types.hpp"

namespace moddlib
{
    using SampleBuffer = simd::AlignedBuffer<8>;
    
    struct SampleBuffers
    {
        static SampleBuffers& sharedBuffers()
        {
            return _instance;
        }

        const SampleBuffer& flatLine()
        {
            return _zeroBuffer;
        }

    private:
    
        SampleBuffers()
        {
            _zeroBuffer.zero_mem();
        }

        static const SampleBuffers& sampleBuffers()
        {
            static SampleBuffers buffers;
            return buffers;
        }

    private:
        static SampleBuffers _instance;

        SampleBuffer _zeroBuffer;

    };

}

#endif //UNITSYNTH_ALIGNEDBUFFER_HPP
