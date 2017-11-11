//
//  Utilities.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 27/11/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#ifndef Utilities_hpp
#define Utilities_hpp

#include <cassert>
#include "Memory.hpp"

namespace moddlib
{
    template<std::size_t I = 0, typename FuncT, typename... Tp>
    std::enable_if_t<I == sizeof...(Tp), void>
    tupleForEach(std::tuple<Tp...>&, FuncT)
    { }

    template<std::size_t I = 0, typename FuncT, typename... Tp>
    inline typename std::enable_if_t<I < sizeof...(Tp), void>
    tupleForEach(std::tuple<Tp...>& t, FuncT f)
    {
        f(std::get<I>(t));
        tupleForEach<I + 1, FuncT, Tp...>(t, f);
    }
    
    template <typename T = float>
    class VariableBuffer : public NonCopyable
    {
    public:
        VariableBuffer() : _value()
        {}
        
        T getValue() const
        {
            return _value;
        }
        
        operator T() const
        {
            return _value;
        }
        
    protected:
        T _value;
    };
    
    class FloatBuffer : public VariableBuffer<float>
    {
    };

    class StreamBuffer : public NonCopyable
    {
    public:
        void ensureBuffer()
        {
            if (!_buffer)
            {
                _buffer = std::make_shared<SampleBuffer>();
            }
        }

        SampleBuffer& buffer()
        {
            return *_buffer.get();
        }
        
        bool isConnected()
        {
            return _buffer.use_count() > 1;
        }
        
        void connect(StreamBuffer& port)
        {
            _buffer = port._buffer;
        }
        
        simd::Vec vec(size_t offset = 0)
        {
            return simd::Vec(ptr(offset));
        }
        
        float* ptr(size_t offset = 0)
        {
            return buffer().data() + offset;
        }
        
    protected:
        std::shared_ptr<SampleBuffer> _buffer;
    };
    
    class Trigger
    {
    public:

        Trigger() : _state(false)
        {}

        bool update(bool state)
        {
            auto oldState = _state;
            _state = state;
            return state && !oldState;
        }

        void reset()
        {
            _state = false;
        }

    private:
        bool _state;
    };


    template <typename PortT, typename FunctionT>
    class FunctionPort
    {
    public:

        FunctionPort(PortT& port, FunctionT func) :
            _port(port),
            _func(func),
            _lastVal(port.getValue()),
            _currentResult(func(port.getValue()))
        {
        }

        float getValue()
        {
            auto aVal = _port.getValue();
            if (_lastVal != aVal)
            {
                _currentResult = _func(aVal);
                _lastVal = aVal;
            }
            return _currentResult;
        }

    private:

        PortT& _port;
        FunctionT _func;
        float _lastVal;
        float _currentResult;
    };

    template <typename PortT, typename FunctionT>
    FunctionPort<PortT, FunctionT> makeFunctionPort(PortT& aPort, FunctionT aFunc)
    {
        return FunctionPort<PortT, FunctionT>(aPort, aFunc);
    }
    
    class TestToneBuffer
    {
    public:
        TestToneBuffer(size_t numSamples) : _buffer(numSamples)
        {
            assert(numSamples % 8 == 0);
            for (size_t n = 0; n < numSamples; ++n)
            {
                _buffer[n] = std::sin(2 * simd::pi<float> * static_cast<float>(n) / numSamples);
            }
        }
        
        size_t size()
        {
            return _buffer.size();
        }
        
        float* data(size_t offset)
        {
            return _buffer.data() + offset;
        }
        
    private:
        AlignedMemory _buffer;
    };

}
























#endif /* Utilities_hpp */
