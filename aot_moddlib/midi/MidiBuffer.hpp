//
//  Header.h
//  unitsynth
//
//  Created by Daniel Doubleday on 14/11/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#ifndef MidiBuffer_hpp
#define MidiBuffer_hpp

#include <queue>

#include "readerwriterqueue.h"
#include "Midi.hpp"

namespace moddlib
{
    struct MidiBuffer
    {
        MidiBuffer() : _queue(64)
        {}
    
        bool dequeue(MidiMessage& message)
        {
            return _queue.try_dequeue(message);
        }
    
        bool enqueue(const MidiMessage message)
        {
            return _queue.try_enqueue(message);
        }
    
    private:
        moodycamel::ReaderWriterQueue<MidiMessage> _queue;
    };

}

#endif /* MidiBuffer_hpp */
