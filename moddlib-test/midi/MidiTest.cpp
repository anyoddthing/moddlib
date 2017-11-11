//
//  MidiTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 14/11/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"

#include "Midi.hpp"

#ifndef DISABLE_TEST

using namespace moddlib;

TEST_CASE("Midi Test")
{
    
    SECTION("Can parse message")
    {
        // Note On 1 [ 90 3B 17 ] 0
        // Note Off 1 [ 80 3B 0A ] 138.321
        
        uint8_t stream[3 * 2] = {
            0x90, 69, 0x17,
            0x80, 69, 0x0A,
        };
        
        MidiMessage msg = { stream[0], stream[1], stream[2] };
        CHECK(msg.type == MidiType::MIDI_NOTE_ON);
        CHECK(msg.getMidiNote() == 69); // da A
        CHECK(msg.getMidiNoteInHertz() == Approx(440));
        
        
        auto msg2 = midi::createMidiMessage(&stream[3]);
        CHECK(msg2.type == MidiType::MIDI_NOTE_OFF);
        
    }
}

#endif // DISABLE_TEST
