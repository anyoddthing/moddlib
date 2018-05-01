//
//  Engine.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 18/07/15.
//  Copyright (c) 2015 Daniel Doubleday. All rights reserved.
//

#pragma once

#include "../../libs/readerwriterqueue/readerwriterqueue.h"

#include <vector>
#include <queue>

#include "Types.hpp"

namespace moddlib
{
    struct Engine
    {
        static constexpr uint FRAMES_PER_BLOCK = 8;
        static constexpr uint DEFAULT_FRAMERATE = 44100;
        
        static constexpr float DB90 = (1.0 / (1 << 15));
        static constexpr float DB96 = (1.0 / 63095.73444801943);
        
        static Engine& instance()
        {
            return _instance;
        }
        
        Engine()
        {
            setFrameRate(DEFAULT_FRAMERATE);
        }

//        uint getFrameCount()
//        {
//            return _frameCount;
//        }
//        
//        void incrFrameCount()
//        {
//            _frameCount += FRAMES_PER_BLOCK;
//        }

        uint getFrameRate()
        {
            return _frameRate;
        }

        void setFrameRate(uint frameRate)
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
        
        uint convertTimeToSamples(float time)
        {
            return static_cast<uint>(time * _frameRate);
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
        static Engine _instance;
        uint _frameCount;
        uint _frameRate;
        float _framePeriod;
        float _inverseNyquist;

        moodycamel::ReaderWriterQueue<Action> _commands;
    };
  
    [[maybe_unused]]
    static float frequency2Phase(float frequency)
    {
        return Engine::instance().convertFrequencyToPhaseIncrement(frequency);
    }
    
    [[maybe_unused]]
    static uint time2Samples(float time)
    {
        return Engine::instance().convertTimeToSamples(time);
    }
    
    [[maybe_unused]]
    static uint framerate()
    {
        return Engine::instance().getFrameRate();
    }
    
}


