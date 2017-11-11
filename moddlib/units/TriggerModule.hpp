//
//  TriggerModule.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 2/28/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#ifndef TriggerModule_h
#define TriggerModule_h

namespace moddlib
{
    struct TriggerModule
    {
        void triggerDown()
        {
            _trigger.down();
        }

        void triggerNote(MidiMessage message)
        {
            _trigger.up();
            _frequency = message.getMidiNoteInHertz();
        }
        
        FloatOutput& getTrigger()
        {
            return _trigger;
        }
        
        FloatOutput& getFrequency()
        {
            return _frequency;
        }
        
    protected:
        FloatOutput _trigger;
        FloatOutput _frequency;
    };
}

#endif /* TriggerModule_h */
