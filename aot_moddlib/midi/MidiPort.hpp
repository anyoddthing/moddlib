//
//  MidiInputPort.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 12/23/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#pragma once

#include "../core/core.hpp"

namespace moddlib
{
    struct MidiPort : FloatOutput
    {
    public:

        MidiPort(float min, float max, float mid) :
            _min(min), _mid(mid), _max(max)
        {
            setControlValue(1);
            setControlValue(0);
        }

        MidiPort(float min = 0, float max = 1) :
            MidiPort(min, max, (max - min) / 2)
        {
        }
        
        void incrementControlValue(int value)
        {
            auto diff = value - 64;
            value = getControlValue() + diff;
            value = std::min<int>(128, std::max<int>(0, value));
            
            setControlValue(value);
        }

        void setControlValue(uint8_t value)
        {
            if (value >= 64)
            {
                auto prcnt = (value - 64) / 64.f;
                setValue(_mid + prcnt * (_max - _mid));
            }
            else
            {
                auto prcnt = value / 64.f;
                setValue(_min + prcnt * (_mid - _min));
            }
            
            _controlValue = value;
        }

        float getControlValue()
        {
            return _controlValue;
        }
        
        void setRange(float min, float mid, float max)
        {
            _min = min;
            _mid = mid;
            _max = max;
        }

        void setRange(float min, float max)
        {
            setRange(min, (max - min) / 2, max);
        }

    private:
        float _min;
        float _mid;
        float _max;
        uint8_t _controlValue;
    };

    struct MidiPortBank
    {
        void setRelative(uint controller, bool value)
        {
            _isRelative[controller] = value;
        }
        
        MidiPort& operator[](uint index)
        {
            return _ports[index];
        }
        
        void processMessage(MidiMessage midiMessage)
        {
            uint8_t control = midiMessage.getControl();
            uint8_t value = midiMessage.getControlValue();
            if (_isRelative[control])
            {
                if (value != 64)
                {
                    _ports[control].incrementControlValue(value);
                }
            }
            else
            {
                _ports[control].setControlValue(value);
            }
        }
        
    private:
        std::array<MidiPort, 128> _ports;
        std::bitset<128> _isRelative;
    };
}



