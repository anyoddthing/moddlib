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
        template <typename LeftT, typename RightT, typename Optional, typename Enable = void>
        struct Connector {};
        
        template <typename LeftT, typename RightT>
        struct Connector<LeftT, RightT, std::false_type, std::enable_if_t<!IsConnectable<LeftT>::value && IsConnectable<RightT>::value>>
        {
            static void connect(LeftT& left, RightT& right)
            {
                right.setSource(left);
            }
        };
        
        template <typename LeftT>
        struct Connector<LeftT, NoInput, std::true_type>
        {
            static void connect(LeftT& left, NoInput& input)
            {
            }
        };
        
        //==============================================================================
        // Specializations for port -> input
        
        template <typename Optional>
        struct Connector<TriggerPort, TriggerInput, Optional>
        {
            static void connect(TriggerPort& port, TriggerInput& input)
            {
                input.setSource(port.buffer());
            }
        };

        template <typename Optional>
        struct Connector<FloatPort, FloatInput, Optional>
        {
            static void connect(FloatPort& port, FloatInput& input)
            {
                input.setSource(port.buffer());
            }
        };

        template <typename Optional>
        struct Connector<FloatPort, DefaultInput, Optional>
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
        detail::Connector<LeftT, RightT, std::false_type>::connect(left, right);
    }
    
    template <typename LeftT, typename RightT>
    void tryconnect(LeftT& left, RightT& right)
    {
        detail::Connector<LeftT, RightT, std::true_type>::connect(left, right);
    }

}


