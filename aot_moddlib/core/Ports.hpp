//
//  Ports.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 1/5/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#ifndef Ports_h
#define Ports_h

namespace moddlib
{
    //==============================================================================
    // Ports - bridge between inputs and outputs
    //
    // Used in circuits to connect multiple inputs of the modules to one
    // input thats part of the circuits interface
    
    namespace detail
    {
        struct Port
        {
            FloatOutput& buffer()
            {
                return _buffer;
            }
         
        protected:
            FloatOutput _buffer;
        };
    }

    struct FloatPort : FloatInput, detail::Port
    {
        void prepare()
        {
            _buffer.setValue(getValue());
        }
    };
    
    struct TriggerPort : TriggerInput, detail::Port
    {
        void prepare()
        {
            auto value = getValue();
            if (value != _buffer.getValue())
            {
                LOG("triggerPort change value: from {} to {}", _buffer.getValue(), value);
            }
            _buffer.setValue(value);
        }
    };
    
    
    //==============================================================================
    // Portlink - Alias for inputs in a cicuit

    template <typename InputT>
    struct PortLink
    {
        void init() {}
        void prepare() {}
        void cleanup() {}
        
        template <typename OutputT>
        void setSource(OutputT& output)
        {
            _delegate->setSource(output);
        }
        
        void linkTo(InputT& port)
        {
            _delegate = &port;
        }
        
    private:
        InputT* _delegate;
    };
    
    struct StreamOutputAlias
    {
        operator StreamOutput&()
        {
            return *static_cast<StreamOutput*>(_delegate);
        }
        
        SampleBuffer& buffer() const
        {
            return _delegate->buffer();
        }
        
        void setOutput(StreamOutput& delegate)
        {
            _delegate = &delegate;
        }
    
    private:
        StreamOutput* _delegate;
    };
}

#endif /* Ports_h */
