//
//  Connect.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 1/6/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#pragma once

namespace moddlib
{
    template <typename T>
    struct IsConnectable : public std::false_type {};
    
    template<> struct IsConnectable<FloatInput> : public std::true_type {};
    template<> struct IsConnectable<DefaultInput> : public std::true_type {};
    template<> struct IsConnectable<TriggerInput> : public std::true_type {};

    template<> struct IsConnectable<FloatPort> : public std::true_type {};
    template<> struct IsConnectable<TriggerPort> : public std::true_type {};

    template<> struct IsConnectable<StreamInput> : public std::true_type {};
    template<typename PortT> struct IsConnectable<PortLink<PortT>> : public std::true_type {};

    namespace detail
    {
        template <typename LeftT, typename RightT, typename Enable = void>
        struct Connector {};
        
        template <typename LeftT, typename RightT>
        struct Connector<LeftT, RightT, std::enable_if_t<!IsConnectable<LeftT>::value && IsConnectable<RightT>::value>>
        {
            static void connect(LeftT& left, RightT& right)
            {
                right.setSource(left);
            }
        };

        //==============================================================================
        // Specializations for port -> input
        
        template <>
        struct Connector<TriggerPort, TriggerInput>
        {
            static void connect(TriggerPort& port, TriggerInput& input)
            {
                input.setSource(port.buffer());
            }
        };

        template <>
        struct Connector<FloatPort, FloatInput>
        {
            static void connect(FloatPort& port, FloatInput& input)
            {
                input.setSource(port.buffer());
            }
        };

        template <>
        struct Connector<FloatPort, DefaultInput>
        {
            static void connect(FloatPort& port, DefaultInput& input)
            {
                input.setSource(port.buffer());
            }
        };

    }
    
    template <typename LeftT, typename RightT>
    void connect(LeftT& left, RightT& right)
    {
        detail::Connector<LeftT, RightT>::connect(left, right);
    }
}


