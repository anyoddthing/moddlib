/*
  ==============================================================================

    MidiKnobMatrix.h
    Created: 5 May 2018 5:00:01pm
    Author:  Daniel Doubleday

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class MidiKnobMatrix    :
    public Component,
    private Slider::Listener
{
public:

    using Listener = std::function<void(int, uint8_t)>;
    
    MidiKnobMatrix()
    {
        initComponents();
    }
    
    void initComponents()
    {
        for (auto& slider : _sliders)
        {
            slider = std::make_unique<Slider>(Slider::RotaryHorizontalVerticalDrag, Slider::NoTextBox);
            slider->setRange({0.0, 128.0}, 1);
            slider->addListener(this);
            addAndMakeVisible(slider.get());
        }
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        auto top = bounds.removeFromTop(bounds.getHeight() / 2);
        auto sliderWidth = bounds.getWidth() / 8;
        for (auto i = 0; i < 8; ++i)
        {
            auto& slider = *_sliders[i];
            slider.setBounds(top.removeFromLeft(sliderWidth));
        }
        
        for (auto i = 8; i < 16; ++i)
        {
            auto& slider = *_sliders[i];
            slider.setBounds(bounds.removeFromLeft(sliderWidth));
        }
    }
    
    void sliderValueChanged(Slider *slider) override
    {
        if (_listener)
        {
            auto match = std::find_if(_sliders.begin(), _sliders.end(),
                [slider](auto& other) { return other.get() == slider; });
            
            _listener(
                static_cast<int>(match - _sliders.begin()),
                static_cast<uint8_t>(slider->getValue()));
        }
    }
    
    void setValue(int slider, uint8_t value)
    {
        const MessageManagerLock mmLock;
        _sliders[slider]->setValue(value, dontSendNotification);
    }
    
    void incrementValue(int slider, int value)
    {
        const MessageManagerLock mmLock;
        _sliders[slider]->setValue(_sliders[slider]->getValue() + value, dontSendNotification);
    }
    
    void setListener(Listener listener)
    {
        _listener = listener;
    }

private:

    std::array<std::unique_ptr<Slider>, 16> _sliders;
    Listener _listener;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiKnobMatrix)
};
