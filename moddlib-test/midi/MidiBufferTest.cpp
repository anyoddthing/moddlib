//
//  MidiTest.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 14/11/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>
#include <thread>
#include <chrono>
#include "catch.hpp"

#include "aot_moddlib.hpp"

#ifndef DISABLE_TEST

using namespace moddlib;

TEST_CASE("MidiBuffer Test")
{
    
    SECTION("Sequencing")
    {
        MidiBuffer buffer;

        std::thread consumer([&]()
        {
            uint8_t noteNumber = 0;
            while (noteNumber != 99)
            {
                MidiMessage nextMessage;
                if (buffer.dequeue(nextMessage))
                {
                    CHECK(nextMessage.getMidiNote() == noteNumber);
                    noteNumber++;
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                }
            }
        });
        
        std::thread producer([&]() {
            for (uint8_t noteNumber = 0; noteNumber != 100; ++noteNumber)
            {
                buffer.enqueue(midi::noteOnMessage(noteNumber, 100));
                std::this_thread::sleep_for(std::chrono::nanoseconds(std::rand() % 10000));
            }
        });
        
        consumer.join();
        producer.join();
    }
}

#endif // DISABLE_TEST
