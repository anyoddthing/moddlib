//
//  Input.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 12/07/15.
//  Copyright (c) 2015 Daniel Doubleday. All rights reserved.
//

#pragma once

#include <array>
#include <tuple>
#include <utility>
#include <type_traits>
#include <cassert>

#include "aot_simdlib/aot_simdlib.hpp"

#include "Memory.hpp"

namespace moddlib
{
    struct NoInput
    {
        static NoInput instance;
    };

    template <uint Size, typename InputT>
    struct Inputs
    {
        using type = InputT;
        using size = std::integral_constant<int, Size>;

        void init()
        {
            initInputs();
        }
        
        void prepare()
        {
            prepareInputs();
        }

        void cleanup()
        {
            cleanupInputs();
        }

        template <uint Pos>
        constexpr auto& get()
        {
            static_assert(Pos < Size, "No such input");
            return _inputs[Pos];
        }
        
        InputT& operator[](uint pos)
        {
            return _inputs[pos];
        }

    private:
    
        //==============================================================================
        // init loop
        
        template <std::size_t I> std::enable_if_t<
            I == Size,
        void> initInputs()
        {}
        
        template <std::size_t I = 0> std::enable_if_t<
            I < Size,
        void> initInputs()
        {
            _inputs[I].init();
        }
        
        //==============================================================================
        // prepare loop
        
        template <std::size_t I> std::enable_if_t<
            I == Size,
        void> prepareInputs()
        {}
        
        template <std::size_t I = 0> std::enable_if_t<
            I < Size,
        void> prepareInputs()
        {
            _inputs[I].prepare();
        }
        
        //==============================================================================
        // cleanup loop
        
        template <std::size_t I> std::enable_if_t<
            I == Size,
        void> cleanupInputs()
        {}
        
        template <std::size_t I = 0> std::enable_if_t<
            I < Size,
        void> cleanupInputs()
        {
            _inputs[I].cleanup();
        }
        
    private:
        std::array<InputT, Size> _inputs;
    };

    namespace detail
    {
        template <typename SelectorT>
        struct InputHelper
        {
            template <typename... InputsT>
            constexpr static auto& get(std::tuple<InputsT...>& inputs)
            {
                static_assert(SelectorT::bank < sizeof...(InputsT), "No such input bank");
                return std::get<SelectorT::bank>(inputs).template get<SelectorT::pos>();
            }
        };
        
        template <>
        struct InputHelper<NoBIn>
        {
            template <typename... InputsT>
            constexpr static auto& get(std::tuple<InputsT...>& inputs)
            {
                return NoInput::instance;
            }
        };
    }
    
    template <typename... InputsT>
    struct InputBank
    {
        using mainIn  = BIn_<0, 0>;
        using mainInL = BIn_<0, 0>;
        using mainInR = BIn_<0, 1>;

        void inputsInit()
        {
            inputBankInit(_inputs);
        }

        void inputsPrepare()
        {
            inputBankPrepare(_inputs);
        }

        void inputsCleanup()
        {
            inputBankCleanup(_inputs);
        }

        template <uint I>
        /* Inputs<InputT>& */ constexpr auto& inputs()
        {
            static_assert(I < sizeof...(InputsT), "No such input bank");
            return std::get<I>(_inputs);
        }

        template <typename SelectorT>
        /* InputT& */ constexpr auto& input()
        {
            return detail::InputHelper<SelectorT>::get(_inputs);
        }
        
        std::tuple<InputsT...>& getInputs()
        {
            return _inputs;
        }
        
    private:

        //==============================================================================
        // init inputs loop

        template <uint I> std::enable_if_t<
            I == sizeof...(InputsT),
        void> inputBankInit(std::tuple<InputsT...>&)
        {}

        template <uint I = 0> std::enable_if_t<
            I < sizeof...(InputsT),
        void> inputBankInit(std::tuple<InputsT...>& inputs)
        {
            std::get<I>(inputs).init();
            inputBankInit<I + 1>(inputs);
        }
        
        //==============================================================================
        // prepare inputs loop>

        template <std::size_t I> std::enable_if_t<
            I == sizeof...(InputsT),
        void> inputBankPrepare(std::tuple<InputsT...>&)
        {}

        template <std::size_t I = 0> std::enable_if_t<
            I < sizeof...(InputsT),
        void> inputBankPrepare(std::tuple<InputsT...>& inputs)
        {
            std::get<I>(inputs).prepare();
            inputBankPrepare<I + 1>(inputs);
        }

        //==============================================================================
        // cleanup inputs loop

        template <std::size_t I> std::enable_if_t<
            I == sizeof...(InputsT),
        void> inputBankCleanup(std::tuple<InputsT...>&)
        {}

        template <std::size_t I = 0> std::enable_if_t<
            I < sizeof...(InputsT),
        void> inputBankCleanup(std::tuple<InputsT...>& inputs)
        {
            std::get<I>(inputs).cleanup();
            inputBankCleanup<I + 1>(inputs);
        }

    private:
        std::tuple<InputsT...> _inputs;
    };
    
    //==============================================================================
    // Helpers methods to prevent need for this->template lookup

    template <typename SelectorT, typename InputBankT>
    /* InputT& */ constexpr auto& input(InputBankT* inputBank)
    {
        return detail::InputHelper<SelectorT>::get(inputBank->getInputs());
    }
    
    template <typename SelectorT, typename InputBankT>
    /* InputT& */ constexpr auto& input(InputBankT& inputBank)
    {
        return detail::InputHelper<SelectorT>::get(inputBank.getInputs());
    }

    //==============================================================================
    // Concrete inputa
    
    struct FloatInput
    {
        FloatInput() : _source(&FlatOutput::sharedOutput())
        {}
        
        void init() {}
        
        void prepare() {}
        
        void cleanup() {}

        float getValue() const
        {
            return _source->getValue();
        }
        
        operator float() const
        {
            return _source->getValue();
        }
        
        bool isConnected()
        {
            return _source != &FlatOutput::sharedOutput();
        }

        void disconnect()
        {
            _source = &FlatOutput::sharedOutput();
        }
        
        void setSource(const FloatBuffer& source)
        {
            _source = &source;
        }

        bool isUp()
        {
            return getValue() > 0.5f;
        }

        bool isDown()
        {
            return !isUp();
        }
        
    private:
        const FloatBuffer* _source;
    };
    
    struct TrackingInput : FloatInput
    {
        void init()
        {
            _lastValue = 0;
        }
        
        void cleanup()
        {
            _lastValue = getValue();
        }
        
        bool changed() const
        {
            return getValue() != _lastValue;
        }
        
        float getLastValue() const
        {
            return _lastValue;
        }
        
    protected:
        float _lastValue;
    };
    
    struct DefaultInput : TrackingInput
    {
        DefaultInput()
        {
            setSource(_defaultValue);
        }
        
        void setup(float value)
        {
            _defaultValue.setValue(value);
        }
        
    private:
        FloatOutput _defaultValue;
    };

    struct TriggerInput : TrackingInput
    {
        bool isTriggered() const
        {
            return getValue() > getLastValue();
        }
    };

    struct LimitedInput : FloatInput
    {
        LimitedInput() : _min(-1), _max(1) {}

        void setup(float min, float value, float max)
        {
            _min = min;
            _max = max;
            setValue(value);
        }

        void prepare()
        {
            setValue(getValue());
        }
        
        float getValue()
        {
            return _value;
        }

        bool isUp()
        {
            return getValue() > 0.01f;
        }
        
        bool isDown()
        {
            return !isUp();
        }
        
    private:
    
        void setValue(float value)
        {
            _value = std::max(_min, std::min(value, _max));
        }
        
    private:
        float _value;
        float _min;
        float _max;
    };

    struct StreamInput
    {
        StreamInput() : _source(&FlatOutput::sharedStream()) {}
        
        void prepare() {}

        void cleanup() {}
        
        void init() {}
        
        const SampleBuffer& buffer() const
        {
            return _source->buffer();
        }
        
        const simd::Vec vec(uint offset = 0) const
        {
            return simd::Vec(ptr(offset));
        }
        
        operator const float*() const { return ptr(); }

        const float* ptr(uint offset = 0) const
        {
            return buffer().data() + offset;
        }
    
        void setSource(StreamOutput& source)
        {
            _source = &source;
        }

        bool isConnected()
        {
            return _source != &FlatOutput::sharedStream();
        }
        
    private:
        const StreamOutput* _source;
    };

    template <size_t Size>
    struct MultiInput : NonCopyable
    {
        MultiInput()
        {
            _connections.fill(nullptr);
        }

        void pullData() {}

        void connect(size_t index, const StreamOutput& output)
        {
            _connections[index] = &output;
        }

    private:
        std::array<StreamOutput*, Size> _connections;
    };
    
}


