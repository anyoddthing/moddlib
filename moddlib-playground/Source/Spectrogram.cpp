/*
  ==============================================================================

    Spectrogram.cpp
    Created: 28 Apr 2018 7:33:11pm
    Author:  Daniel Doubleday

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Spectrogram.h"

//==============================================================================
Spectrogram::Spectrogram() :
    _forwardFFT(fftOrder),
    _spectrogramImage(Image::RGB, fftSqrtSize, fftSqrtSize, false)
{
    setFramesPerSecond(30);
    setOpaque(true);
}

void Spectrogram::paint (Graphics& g)
{
    g.fillAll (Colours::black);

    g.setOpacity (1.0f);
    g.drawImage(_spectrogramImage, getLocalBounds().toFloat());
}

void Spectrogram::update()
{
    if (_dataAvailable.get())
    {    
        _forwardFFT.performFrequencyOnlyForwardTransform(_fftData.data());
        auto levels = FloatVectorOperations::findMinAndMax(_fftData.data(), fftBuckets);
        float minDB = 0, maxDB = -100;
//        DBG("maxLevel=" << levels.getEnd() << " minLevel=" << levels.getStart());
        for (auto i = 0; i < fftBuckets; ++i)
        {
            float relLevel = _fftData[i] / levels.getEnd();
            float db = std::max(10 * std::log10(relLevel), -30.0f);
            minDB = std::min(minDB, db);
            maxDB = std::max(maxDB, db);
            float deltaDb = (db + 30) / 30;
            
            auto level = jmap(deltaDb, 0.0f, 1.0f, 0.0f, 0.66f);
            auto y = fftSqrtSize - i / fftSqrtSize - 1;
            auto x = i % fftSqrtSize;
            _spectrogramImage.setPixelAt(x, y, Colour::fromHSV(0.66 - level, 1, deltaDb, 1));
        }
//        DBG("maxDB=" << maxDB << " minDB=" << minDB);
        _dataAvailable.set(false);
    }
}

void Spectrogram::read(const float* buffer, size_t maxVals)
{
    for (auto i = 0; i < maxVals; ++i)
    {
        if (_bufferPos == fftSize)
        {
            if (!_dataAvailable.get())
            {
//                zeromem(_fftData.data(), sizeof(float) * _fftData.size());
                memcpy(_fftData.data(), _buffer.data(), sizeof(float) * _buffer.size());
                _dataAvailable.set(true);
            }
            
            _bufferPos = 0;
        }
        
        _buffer[_bufferPos++] = buffer[i];
    }
}
