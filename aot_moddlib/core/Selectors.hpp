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
    //==============================================================================
    // Basic selectors
    
    struct ModuleSelectorTag {};
    struct InputSelectorTag {};
    struct OutputSelectorTag {};
    struct BankSelectorTag {};
    
    template <int Bank>
    struct Bank_
    {
        using selectorType = BankSelectorTag;
        enum { bank = Bank };
    };
    
    using NoBank = Bank_<-1>;

    template <int Pos>
    struct Mod_
    {
        using selectorType = ModuleSelectorTag;
        enum { pos = Pos };
    };
    
    using NoMod = Mod_<-1>;

    template <int Bank, int Pos>
    struct BIn_
    {
        using selectorType = InputSelectorTag;
        enum { bank = Bank };
        enum { pos = Pos };
    };
    
    using NoBIn = BIn_<-1, -1>;

    template <int Bank, int Pos>
    struct BOut_
    {
        using selectorType = OutputSelectorTag;
        enum { bank = Bank };
        enum { pos = Pos };
    };

    using NoBOut = BOut_<-1, -1>;
    
    //==============================================================================
    // selector list
    
    template <typename HeadT, typename TailT>
    struct SelList
    {
        using head = HeadT;
        using tail = TailT;
    };

    namespace detail
    {
        template <typename... T>
        struct makeSel {};

        template <typename HeadT, typename ... RestT>
        struct makeSel<HeadT, RestT...>
        {
            using type = SelList<HeadT, typename makeSel<RestT...>::type>;
        };

        template <>
        struct makeSel<>
        {
            using type = std::nullptr_t;
        };
    }
    
    template <typename ... T>
    using Sel_ = typename detail::makeSel<T...>::type;

    //==============================================================================
    // traits
    
    template <typename, typename = void>
    struct IsModuleSelector : std::false_type {};
    
    template <typename SelectorT>
    struct IsModuleSelector<SelectorT, std::enable_if_t<
        std::is_same<typename SelectorT::selectorType, ModuleSelectorTag>::value
    >> : std::true_type {};
    
    template <typename, typename = void>
    struct IsInputSelector : std::false_type {};
    
    template <typename SelectorT>
    struct IsInputSelector<SelectorT, std::enable_if_t<
        std::is_same<typename SelectorT::selectorType, InputSelectorTag>::value
    >> : std::true_type {};
    
    template <typename, typename = void>
    struct IsOutputSelector : std::false_type {};
    
    template <typename SelectorT>
    struct IsOutputSelector<SelectorT, std::enable_if_t<
        std::is_same<typename SelectorT::selectorType, OutputSelectorTag>::value
    >> : std::true_type {};
    
    template <typename, typename = void>
    struct IsSelectorList : std::false_type {};

    template <typename SelectorT>
    struct IsSelectorList<SelectorT, std::void_t<
        std::is_same<
            SelList<typename SelectorT::head, typename SelectorT::tail>,
            SelectorT>
    >> : std::true_type {};
    
    //==============================================================================
    // Default ins and outs
    
    template <typename UnitT>
    struct MainOut
    {
        using type = typename UnitT::mainOut;
    };
    
    template <typename UnitT>
    using MainOutT = typename MainOut<UnitT>::type;
    
    template <typename UnitT>
    struct MainIn
    {
        using type = typename UnitT::mainIn;
    };
    
    template <typename UnitT>
    using MainInT = typename MainIn<UnitT>::type;

    //==============================================================================
    // default freq in
    
#define DEFAULT_PORT(_NAME_, _TYPE_)                                    \
    template <typename, typename = void>                                \
    struct _NAME_ { using type = NoBIn; };                              \
                                                                        \
    template <typename UnitT>                                           \
    struct _NAME_<UnitT, std::void_t<typename UnitT::_TYPE_>>           \
    {                                                                   \
        using type = typename UnitT::_TYPE_;                            \
    };                                                                  \
                                                                        \
    template <typename UnitT>                                           \
    using _NAME_ ## T = typename _NAME_<UnitT>::type;
    
    DEFAULT_PORT(FreqIn, freqIn)
    DEFAULT_PORT(PhaseIn, phaseIn)
    DEFAULT_PORT(TriggerIn, triggerIn)
}
