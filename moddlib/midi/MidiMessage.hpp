//
//  Midi.h
//  unitsynth
//
//  Created by Daniel Doubleday on 14/11/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#pragma once

#include <cmath>

namespace moddlib
{
    enum MidiType: uint8_t
    {

        MIDI_UNKNOWN            = 0x00,

        // channel voice messages
        MIDI_NOTE_OFF           = 0x80,
        MIDI_NOTE_ON            = 0x90,
        MIDI_CONTROL_CHANGE     = 0xB0,
        MIDI_PROGRAM_CHANGE     = 0xC0,
        MIDI_PITCH_BEND         = 0xE0,
        MIDI_AFTERTOUCH         = 0xD0, // aka channel pressure
        MIDI_POLY_AFTERTOUCH    = 0xA0, // aka key pressure

        // system messages
        MIDI_SYSEX              = 0xF0,
        MIDI_TIME_CODE          = 0xF1,
        MIDI_SONG_POS_POINTER   = 0xF2,
        MIDI_SONG_SELECT        = 0xF3,
        MIDI_TUNE_REQUEST       = 0xF6,
        MIDI_SYSEX_END          = 0xF7,
        MIDI_TIME_CLOCK         = 0xF8,
        MIDI_START              = 0xFA,
        MIDI_CONTINUE           = 0xFB,
        MIDI_STOP               = 0xFC,
        MIDI_ACTIVE_SENSING     = 0xFE,
        MIDI_SYSTEM_RESET       = 0xFF
    };

    struct MidiLimits
    {
        const int minBend = 0;
        const int maxBend = 16383; // (2^14)
    };
    
    union MidiMessage
    {
        struct
        {
            uint8_t bytes[4];
        } raw;
        
        MidiType type;
        
        uint8_t getMidiNote()
        {
            return raw.bytes[1];
        }
        
        float getMidiNoteInHertz()
        {
            return 440.0 * std::pow(2.0, (getMidiNote() - 69) / 12.0);
        }
        
        uint8_t getVelocity()
        {
            return raw.bytes[2];
        }
        
        uint8_t getControl()
        {
            return raw.bytes[1];
        }
        
        uint8_t getControlValue()
        {
            return raw.bytes[2];
        }
        
        uint8_t getAfterTouch()
        {
            return raw.bytes[1];
        }
        
        float getPitchBend()
        {
            int intValue = (((int)raw.bytes[2]) << 7) + raw.bytes[1];
            return ((float)(intValue - 0x2000)) / 0x2000;
        }
    };
    
    namespace midi
    {
        static inline MidiMessage createMidiMessage(uint8_t* values)
        {
            return MidiMessage { values[0], values[1], values[2] };
        }
        
        static inline MidiMessage noteOnMessage(uint8_t noteNumber, uint8_t velocity)
        {
            return MidiMessage { MidiType::MIDI_NOTE_ON, noteNumber, velocity };
        }
        
        static inline MidiMessage noteOffMessage(uint8_t noteNumber, uint8_t velocity)
        {
            return MidiMessage { MidiType::MIDI_NOTE_OFF, noteNumber, velocity };
        }
        
        static inline MidiMessage controllMessage(uint8_t controller, uint8_t value)
        {
            return MidiMessage { MidiType::MIDI_NOTE_ON, controller, value };
        }
    };
}

