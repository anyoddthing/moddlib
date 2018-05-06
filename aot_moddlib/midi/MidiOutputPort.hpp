//
//  MidiInputPort.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 12/23/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#ifndef MidiInputPort_h
#define MidiInputPort_h

#include "../core/core.hpp"

namespace moddlib
{
    class MidiOutputPort : public FloatOutput
    {
    public:

        MidiOutputPort(float min, float max, float mid, bool isRelative = false) :
            _min(min), _mid(mid), _max(max), _isRelative(isRelative)
        {
            setControlValue(1);
            setControlValue(0);
        }

        MidiOutputPort(float min = 0, float max = 1) :
            MidiOutputPort(min, max, (max - min) / 2)
        {
        }

        void setControlValue(int value)
        {
            std::cout << "setControlValue " << (int)value << std::endl;
            if (_isRelative)
            {
                auto diff = value - 64;
                value = static_cast<int>(getControlValue()) + diff;
                value = std::min(128, std::max(0, value));
            }
            
            if (value >= 64)
            {
                auto prcnt = (value - 64) / 63.f;
                setValue(_mid + prcnt * (_max - _mid));
            }
            else
            {
                auto prcnt = value / 63.f;
                setValue(_min + prcnt * (_mid - _min));
            }
        
        }

        uint8_t getControlValue()
        {
            return static_cast<uint8_t>(0.5f + 127.f * (_value - _min) / (_max - _min));
        }

        void setIsRelative(bool value)
        {
            _isRelative = value;
        }
        
    private:

        float _min;
        float _mid;
        float _max;
        bool _isRelative;
    };

}


#endif /* MidiInputPort_h */
