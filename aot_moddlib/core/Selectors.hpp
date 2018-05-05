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
    // based on https://blog.galowicz.de/2016/05/08/compile_time_type_lists/
    
    template <typename HeadT, typename TailT>
    struct SelList
    {
        using head = HeadT;
        using tail = TailT;
    };

    namespace detail
    {
        template <typename ListT, typename SelT>
        struct appendSel;

        // (empty list, std::nullptr_t item) -> Still an empty list
        template <>
        struct appendSel<std::nullptr_t, std::nullptr_t>
        {
            using type = std::nullptr_t;
        };

        // (empty list, T) -> List which only contains T
        // This is usually the recursion abort step when adding an item to a list
        template <typename SelT>
        struct appendSel<std::nullptr_t, SelT>
        {
            using type = SelList<SelT, std::nullptr_t>;
        };

        template <typename HeadT, typename TailT>
        struct appendSel<std::nullptr_t, SelList<HeadT, TailT>>
        {
            using type = SelList<HeadT, TailT>;
        };
        
        // (list, T) -> Recurse until tail of list, and return a version with T at its end
        template <typename HeadT, typename TailT, typename SelT>
        struct appendSel<SelList<HeadT, TailT>, SelT>
        {
            using type = SelList<HeadT, typename appendSel<TailT, SelT>::type>;
        };

        template <typename ListT, typename SelT>
        using appendSelT = typename appendSel<ListT, SelT>::type;

        template <typename... T>
        struct makeSel {};

        template <typename HeadT, typename ... RestT>
        struct makeSel<HeadT, RestT...>
        {
            using type = SelList<HeadT, typename makeSel<RestT...>::type>;
        };

        template <typename HeadT, typename TailT, typename ... RestT>
        struct makeSel<SelList<HeadT, TailT>, RestT...>
        {
            using type = typename appendSel<SelList<HeadT, TailT>, typename makeSel<RestT...>::type>::type;
        };

        template <>
        struct makeSel<>
        {
            using type = std::nullptr_t;
        };
    }
    
    template <typename... T>
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
    struct IsSelectorList<SelectorT, std::enable_if_t<
        std::is_same<
            SelList<typename SelectorT::head, typename SelectorT::tail>,
            SelectorT>::value
    >> : std::true_type {};
    
    //==============================================================================
    // default freq in
    
#define DEFAULT_PORT(_NAME_, _TYPE_)                                    \
    template <typename, typename = void>                                \
    struct _NAME_ { using type = NoBIn; };                              \
                                                                        \
    template <typename UnitT>                                           \
    struct _NAME_<UnitT, std::enable_if_t<                              \
        std::is_class<typename UnitT::_TYPE_>::value                    \
    >>                                                                  \
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
