/*
  ==============================================================================

    Oscilloscope.h
    Created: 26 Apr 2018 7:58:16pm
    Author:  Daniel Doubleday

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Oscilloscope :
    public AnimatedAppComponent
{
public:
    Oscilloscope();

    void paint (Graphics&) override;
    void update() override;
    
    void read(const float* buffer, size_t maxVals, size_t minSamples = 0);
    
private:
    Atomic<bool> _dataAvailable;
    Path _path;
    std::array<float, 4 * 1024> _buffer;
    uint _bufferPos;
    uint _numSamples;
    float _last = 1;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oscilloscope)
};
