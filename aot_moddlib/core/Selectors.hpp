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
    
    template <int Pos>
    struct Out_
    {
        enum { pos = Pos };
    };
    
    using NoOut = Out_<-1>;
    
    template <int Pos>
    struct In_
    {
        enum { pos = Pos };
    };
    
    using NoIn = In_<-1>;

    template <int Pos>
    struct Mod_
    {
        enum { pos = Pos };
    };
    
    using NoMod = Mod_<-1>;

    template <int Bank, int Pos>
    struct BIn_
    {
        enum { bank = Bank };
        enum { pos = Pos };
    };
    
    using NoBIn = BIn_<-1, -1>;

    template <int Bank, int Pos>
    struct BOut_
    {
        enum { bank = Bank };
        enum { pos = Pos };
    };

    using NoBout = BOut_<-1, -1>;
    
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

    template <typename, template <typename> class, typename = void>
    struct HasPort : std::false_type {};

    template <typename UnitT, template <typename> class Selector>
    struct HasPort<UnitT, Selector, std::void_t<Selector<UnitT>>> : std::true_type {};
    
    template <typename, typename = void>
    struct FreqIn
    {
        using type = NoBIn;
    };
    
    template <typename UnitT>
    struct FreqIn<UnitT, std::void_t<typename UnitT::freqIn>>
    {
        using type = typename UnitT::freqIn;
    };
    
    template <typename UnitT>
    using FreqInT = typename FreqIn<UnitT>::type;
    
    template <typename UnitT>
    struct PhaseIn
    {
        using type = typename UnitT::phaseIn;
    };
    
    template <typename UnitT>
    using PhaseInT = typename PhaseIn<UnitT>::type;

    template <typename UnitT>
    struct TriggerIn
    {
        using type = typename UnitT::triggerIn;
    };
    
    template <typename UnitT>
    using TriggerInT = typename TriggerIn<UnitT>::type;
}
