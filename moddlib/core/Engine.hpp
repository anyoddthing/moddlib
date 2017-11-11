//
//  Engine.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 18/07/15.
//  Copyright (c) 2015 Daniel Doubleday. All rights reserved.
//

#ifndef unitsynth_Engine_hpp
#define unitsynth_Engine_hpp

#include <vector>
#include <queue>
#include "Types.hpp"
#include "readerwriterqueue.h"

namespace moddlib
{
    struct Engine
    {
        static constexpr uint32_t FRAMES_PER_BLOCK = 8;
        static constexpr uint32_t DEFAULT_FRAMERATE = 44100;
        
        static constexpr float DB90 = (1.0 / (1 << 15));
        static constexpr float DB96 = (1.0 / 63095.73444801943);
        
        static Engine& instance()
        {
            static Engine engine;
            return engine;
        }
        
        Engine()
        {
            setFrameRate(DEFAULT_FRAMERATE);
        }

        uint32_t getFrameCount()
        {
            return _frameCount;
        }
        
        void incrFrameCount()
        {
            _frameCount += FRAMES_PER_BLOCK;
        }

        uint32_t getFrameRate()
        {
            return _frameRate;
        }

        void setFrameRate(uint32_t frameRate)
        {
            _frameRate      = frameRate;
            _framePeriod    = 1.0 / frameRate;
            _inverseNyquist = 2.0 / frameRate;
        }

        float getFramePeriod()
        {
            return _framePeriod;
        }

        float getCurrentTime()
        {
            return _frameCount * _framePeriod;
        }

        float getInverseNyquist()
        {
            return _inverseNyquist;
        }
        
        uint32_t convertTimeToSamples(float time)
        {
            return static_cast<uint32_t>(time * _frameRate);
        }

        float convertTimeToExponentialScaler(float duration)
        {
            auto numFrames = duration * getFrameRate();
            return std::pow(DB90, (1.0 / numFrames));
        }
        
        float convertFrequencyToPhaseIncrement(float frequency)
        {
            return frequency * getInverseNyquist();
        }
        
        bool queueCommand(Action&& command)
        {
            return _commands.try_enqueue(std::move(command));
        }
        
    private:
        
        uint32_t _frameCount;
        uint32_t _frameRate;
        float _framePeriod;
        float _inverseNyquist;

        moodycamel::ReaderWriterQueue<Action> _commands;
    };
  
    static float frequency2Phase(float frequency)
    {
        return Engine::instance().convertFrequencyToPhaseIncrement(frequency);
    }
    
    static uint32_t time2Samples(float time)
    {
        return Engine::instance().convertTimeToSamples(time);
    }
    
    static uint32_t framerate()
    {
        return Engine::instance().getFrameRate();
    }
    
}


#endif
