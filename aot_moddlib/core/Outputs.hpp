//
//  Outputs.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 18/07/15.
//  Copyright (c) 2015 Daniel Doubleday. All rights reserved.
//

#ifndef unitsynth_Outputs_hpp
#define unitsynth_Outputs_hpp

#include <iostream>

#include "aot_simdlib/aot_simdlib.hpp"

namespace moddlib
{
    template <int Size, typename OutputT>
    struct Outputs
    {
        using type = OutputT;
        
        OutputT& get(const size_t index)
        {
            return _outputs[index];
        }
        
        OutputT& operator[](const size_t index)
        {
            return _outputs[index];
        }
        
    private:
        std::array<OutputT, Size> _outputs;
    };
    

    template <typename... OutputsT>
    struct OutputBank
    {
        using mainOut  = BOut_<0, 0>;
        using mainOutL = BOut_<0, 0>;
        using mainOutR = BOut_<0, 1>;
        
        template <std::size_t I>
        /* Outputs<OutputT>& */ constexpr auto& outputs()
        {
            return std::get<I>(_outputs);
        }
        
        template <typename SelectorT>
        /* InputT& */ constexpr auto& output()
        {
            return outputs<SelectorT::bank>()[SelectorT::pos];
        }
        
        template <std::size_t I, std::size_t J>
        /* InputT& */ constexpr auto& output(BIn_<I, J> selector)
        {
            return outputs<selector.bank>()[selector.pos];
        }
        
        constexpr auto& output(uint32_t i, uint32_t j)
        {
            return std::get<i>(_outputs)[j];
        }
        
    private:
        std::tuple<OutputsT...> _outputs;
    };

    struct StreamOutput
    {
        constexpr SampleBuffer& buffer() const
        {
            return _buffer;
        }
        
        simd::Vec vec(size_t offset = 0)
        {
            return simd::Vec(ptr(offset));
        }
        
        operator const float*() const { return _buffer.data(); }
        operator float*() { return _buffer.data(); }

        float* ptr(size_t offset = 0)
        {
            return buffer().data() + offset;
        }
        
    private:
        mutable SampleBuffer _buffer;
    };
    
    struct FloatOutput : FloatBuffer
    {
        void setValue(float value)
        {
            _value = value;
        }
        
        void up()
        {
            _value = 1.f;
        }
        
        void down()
        {
            _value = 0;
        }
        
        operator float() const
        {
            return _value;
        }
        
        const FloatOutput& operator=(float value)
        {
            _value = value;
            return *this;
        }
    };
    
    struct FlatOutput
    {
        static const FloatOutput& sharedOutput() 
        {
            return _output;
        }
        
        static const StreamOutput& sharedStream()
        {
            return _stream;
        }
        
    private:
        static FloatOutput _output;
        static StreamOutput _stream;
    };

    using SingleStreamOutput = OutputBank<Outputs<1, StreamOutput>>;
}


#endif
