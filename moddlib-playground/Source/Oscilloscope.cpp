/*
  ==============================================================================

    Oscilloscope.cpp
    Created: 26 Apr 2018 7:58:16pm
    Author:  Daniel Doubleday

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Oscilloscope.h"

//==============================================================================
Oscilloscope::Oscilloscope() : _dataAvailable(false), _bufferPos(0), _numSamples(0)
{
    setFramesPerSecond(30);
    setOpaque(true);
}

void Oscilloscope::paint(Graphics& g)
{
    auto bounds = getLocalBounds();
    g.fillAll(Colours::black);
    g.setColour(Colours::white);
    g.drawRect(0, 0, bounds.getWidth(), bounds.getHeight());
    g.drawLine(0, bounds.getHeight() / 2, bounds.getWidth(), bounds.getHeight() / 2);
    g.drawText(String(_numSamples), 5, 5, 50, 20, Justification::Flags::left);
    
    float scaleFactorY = 2 *  bounds.getHeight() / 2;
    float scaleFactorX = bounds.getWidth();
    g.strokePath(_path, PathStrokeType(2),
        AffineTransform::verticalFlip(bounds.getHeight() / scaleFactorY)
        .scaled(scaleFactorX, scaleFactorY)
        .translated(0, -0.5 * bounds.getHeight())
    );
}

void Oscilloscope::update()
{
    if (_dataAvailable.get())
    {
        _path.clear();
        auto numDots = std::min<size_t>(200, _numSamples);
        float dotIncr = 1.0f / numDots;
        float sampleIncr = 1.0f / _numSamples;
        
        for (float dotPos = 0; dotPos < 1.0; dotPos += dotIncr)
        {
            float samplePos = dotPos * _numSamples;
            uint index = static_cast<uint>(samplePos);
            uint index2 = std::min(index + 1, _numSamples - 1);
            float targetY;
            if (index != index2)
            {
                float fPos1 = 1 - (samplePos - index);
                float fPos2 = 1 - (index2 - samplePos);
                targetY = fPos1 * _buffer[index] + fPos2 * _buffer[index2];
            }
            else
            {
                targetY = _buffer[index];
            }
            
            _path.lineTo(dotPos, targetY);
            samplePos += sampleIncr;
        }
        
        _bufferPos = 0;
        _last = 1;
        _dataAvailable.set(false);
    }
}

void Oscilloscope::read(const float* buffer, size_t maxVals, size_t minSamples)
{
    if (!_dataAvailable.get())
    {
        // find zero crossing
        for (auto i = 0; i < maxVals; ++i)
        {
            float current = buffer[i];
            
            if ((_last < 0 && current >= 0) || _bufferPos == _buffer.size())
            {
                if (_bufferPos == 0)
                {
                    _buffer[_bufferPos++] = 0;
                }
                else
                {
                    _buffer[_bufferPos++] = 0;
                    _numSamples = _bufferPos;
                    _dataAvailable.set(true);

                    break;
                }
            }
            else if (_bufferPos > 0)
            {
                _buffer[_bufferPos++] = current;
            }
            
            _last = current;
        }
    }
}





