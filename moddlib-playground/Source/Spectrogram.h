/*
  ==============================================================================

    Spectrogram.h
    Created: 28 Apr 2018 7:33:11pm
    Author:  Daniel Doubleday

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Spectrogram :
    public AnimatedAppComponent
{
public:
    Spectrogram();

    void update() override;
    void paint (Graphics&) override;
    void read(const float* buffer, size_t maxVals);
    
private:
    enum
    {
        fftOrder    = 13,
        fftSize     = 1 << fftOrder,
        fftBuckets  = fftSize / 2,
        fftSqrtSize = 64,
    };

    Atomic<bool> _dataAvailable;
    dsp::FFT _forwardFFT;
    Image _spectrogramImage;
    std::array<float, fftSize> _buffer;
    std::array<float, 2 * fftSize> _fftData;
    size_t _bufferPos;
    size_t _numSamples;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Spectrogram)
};
