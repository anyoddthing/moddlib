//
//  Voice.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 19/09/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#ifndef Voice_h
#define Voice_h

namespace moddlib
{
    /**
     * A Voice holds state for the currently playing circuit
     */
    class Voice : public NonCopyable
    {
    public:

        // enum is sorted for voice stealing
        enum State : uint8_t { IDLE, RAMP_DOWN, FADE_OUT, ON };

        void setState(State value)
        {
            if (_state != value)
            {
                _state = value;
            }
        }

        State getState()
        {
            return _state;
        }

        bool isActive()
        {
            return _state > 0;
        }

        uint8_t getLastNote()
        {
            return _lastNote;
        }

        void setNote(uint8_t note)
        {
            _lastNote = note;
        }

    private:
        State _state;
        uint8_t _lastNote;
    };
}


#endif /* Voice_h */
