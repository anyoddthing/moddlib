//
//  Blocks.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 18/07/15.
//  Copyright (c) 2015 Daniel Doubleday. All rights reserved.
//

#ifndef Circuit_hpp
#define Circuit_hpp

#include "Generator.hpp"

namespace moddlib
{
    template <typename CircuitT, typename ...moduleTs>
    struct Circuit
    {
        using mainModule = Mod_<0>;

        //==============================================================================
        // main calls

        /** Initialization call
         * 
         * Called to reset the circuit and all sub-modules to their respective initial state.
         * A midi synth would call this on every note-on message.
         */
        void init()
        {
            inputsInitImpl(0);
            circuitInit(_modules);
        }

        /** Main render loop callback
         * 
         * Recursively pulls all inputs and calls doGenerate on every generator.
         */
        void generate()
        {
            inputsPrepareImpl(0);
            circuitGenerate(_modules);
            inputsCleanupImpl(0);
        }

        //==============================================================================
        // module access

        template <typename SelectorT>
        constexpr std::tuple_element_t<SelectorT::pos, std::tuple<moduleTs...>>&
        /* moduleT& */ module()
        {
            return std::get<SelectorT::pos>(_modules);
        }
        
        template <typename SelT>
        struct ModuleAccessor
        {
            template <typename CurrentCircuitT>
            static auto& unpack(CurrentCircuitT& circuit)
            {
                return SelT::compile_error;
            }
        };
        
        template <typename FirstT, typename TailT>
        struct ModuleAccessor<SelList<FirstT, TailT>>
        {
            template <typename CurrentCircuitT>
            static auto& unpack(CurrentCircuitT& circuit)
            {
                auto& subcircuit = circuit.template module<FirstT>();
                return ModuleAccessor<TailT>::unpack(subcircuit);
            }
        };
        
        template <typename FirstT>
        struct ModuleAccessor<SelList<FirstT, std::nullptr_t>>
        {
            template <typename CurrentCircuitT>
            static auto& unpack(CurrentCircuitT& circuit)
            {
                 return unpack(circuit, typename FirstT::selectorType {});
            }
            
        private:
        
            template <typename CurrentCircuitT>
            static auto& unpack(CurrentCircuitT& circuit, ModuleSelectorTag)
            {
                return circuit.template module<FirstT>();
            }
            
            template <typename CurrentCircuitT>
            static auto& unpack(CurrentCircuitT& circuit, InputSelectorTag)
            {
                return circuit.template input<FirstT>();
            }
            
            template <typename CurrentCircuitT>
            static auto& unpack(CurrentCircuitT& circuit, OutputSelectorTag)
            {
                return circuit.template output<FirstT>();
            }

        };

        template <typename... SelectorT>
        constexpr auto moduleIn() -> decltype(auto)
        {
            return ModuleAccessor<Sel_<SelectorT...>>::unpack(*this);
        }

        template <typename... SelectorT>
        constexpr auto moduleOut() -> decltype(auto)
        {
            return ModuleAccessor<Sel_<SelectorT...>>::unpack(*this);
        }

        constexpr std::tuple<moduleTs...>& getModules()
        {
            return _modules;
        }

    private:

        CircuitT* thiz()
        {
            return static_cast<CircuitT*>(this);
        }

        //==============================================================================
        // optional inputsInit - supports circuits w/out input bank

        template <typename T = CircuitT>
        void inputsInitImpl(long) { }

        template <typename T = CircuitT>
        auto inputsInitImpl(int)
            -> decltype(std::declval<T>().inputsInit(), void())
        {
            thiz()->inputsInit();
        }

        //==============================================================================
        // optional inputsPrepare - supports circuits w/out input bank

        template <typename T = CircuitT>
        void inputsPrepareImpl(long) { }

        template <typename T = CircuitT>
        auto inputsPrepareImpl(int)
            -> decltype(std::declval<T>().inputsPrepare(), void())
        {
            thiz()->inputsPrepare();
        }

        //==============================================================================
        // optional inputsCleanup - supports circuits w/out input bank

        template <typename T = CircuitT>
        void inputsCleanupImpl(long) { }

        template <typename T = CircuitT>
        auto inputsCleanupImpl(int)
            -> decltype(std::declval<T>().inputsCleanup(), void())
        {
            thiz()->inputsCleanup();
        }

        //==============================================================================
        // init loop

        template <std::size_t I> std::enable_if_t<
            I == sizeof...(moduleTs),
        void> circuitInit(std::tuple<moduleTs...>& ) { }

        template <std::size_t I = 0> std::enable_if_t<
            I < sizeof...(moduleTs),
        void> circuitInit(std::tuple<moduleTs...>& inputs)
        {
            circuitInit(std::get<I>(inputs));
            circuitInit<I + 1>(inputs);
        }

        template <typename T>
        void circuitInit(Generator<T>& generator)
        {
            generator.init();
        }

        template <typename ...SubmoduleT>
        void circuitInit(Circuit<SubmoduleT...>& circuit)
        {
            circuit.init();
        }

        //==============================================================================
        // generate loop

        template <std::size_t I> std::enable_if_t<
            I == sizeof...(moduleTs),
        void> circuitGenerate(std::tuple<moduleTs...>& ) { }

        template <std::size_t I = 0> std::enable_if_t<
            I < sizeof...(moduleTs),
        void> circuitGenerate(std::tuple<moduleTs...>& modules)
        {
            circuitGenerate(std::get<I>(modules));
            circuitGenerate<I + 1>(modules);
        }

        template <typename T>
        void circuitGenerate(Generator<T>& generator)
        {
            generator.generate();
        }

        template <typename ...SubmoduleT>
        void circuitGenerate(Circuit<SubmoduleT...>& circuit)
        {
            circuit.generate();
        }
        
    private:
        std::tuple<moduleTs...> _modules;
    };
    
    //==============================================================================
    // Helpers methods to prevent need for this->template lookup

    template <typename... SelectorT, typename CircuitT>
    auto moduleIn(CircuitT* circuit) -> decltype(auto)
    {
        return circuit->template moduleIn<SelectorT...>();
    }
    
    template <typename... SelectorT, typename CircuitT>
    auto moduleIn(CircuitT& circuit) -> decltype(auto)
    {
        return circuit.template moduleIn<SelectorT...>();
    }
    
    template <typename... SelectorT, typename CircuitT>
    auto moduleOut(CircuitT* circuit) -> decltype(auto)
    {
        return circuit->template moduleOut<SelectorT...>();
    }
    
    template <typename... SelectorT, typename CircuitT>
    auto moduleOut(CircuitT& circuit) -> decltype(auto)
    {
        return circuit.template moduleOut<SelectorT...>();
    }
}

#endif

