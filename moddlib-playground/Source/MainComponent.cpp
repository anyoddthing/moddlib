/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include <fstream>
#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"
#include "aot_moddlib/instruments/WaveTableSynth.hpp"

static const bool DUMP_DEBUG_STREAM = true;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   :
    public AudioAppComponent,
    public MidiInputCallback
{
public:
    //==============================================================================
    MainContentComponent()
    {
        initComponents();
        
        setSize(800, 600);
        
        // specify the number of input and output channels that we want to open
        setAudioChannels(2, 2);
    }

    ~MainContentComponent()
    {
        shutdownAudio();
        
        const StringArray midiInputs(MidiInput::getDevices());
        for (int i = 0; i < midiInputs.size(); ++i)
        {
            if (deviceManager.isMidiInputEnabled(midiInputs[i]))
            {
                deviceManager.removeMidiInputCallback(midiInputs[i], this);
            }
        }
    }
    
    // Midi Callback
    void handleIncomingMidiMessage(juce::MidiInput *source, const juce::MidiMessage &message) override
    {
        if (_synth)
        {
            _synth->queueMessage(moddlib::midi::createMidiMessage(message.getRawData()));
        }
    }
    
    
    void initComponents()
    {
        _audioDeviceSelector = std::make_unique<AudioDeviceSelectorComponent>(
            deviceManager, 0, 0, 2, 2, true, false, true, false);
        addAndMakeVisible(_audioDeviceSelector.get());
        
        const StringArray midiInputs(MidiInput::getDevices());
        for (int i = 0; i < midiInputs.size(); ++i)
        {
            if (midiInputs[i].equalsIgnoreCase("Roland Digital Piano"))
            {
                DBG("Midi enabled: " << midiInputs[i] << ": " << (int)deviceManager.isMidiInputEnabled(midiInputs[i]));
                
                deviceManager.setMidiInputEnabled(midiInputs[i], true);
                deviceManager.addMidiInputCallback(midiInputs[i], this);
            }
        }
    }
    
    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        if (DUMP_DEBUG_STREAM)
        {
            _dbgStream = std::make_unique<std::ofstream>("testout.bin", std::ios::binary | std::ios::out);
        }
        _synth = std::make_unique<moddlib::WaveTableSynth>();
    }

    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
    {
        jassert(bufferToFill.buffer->getNumChannels() == 2);
        
        for (auto i = 0; i < bufferToFill.numSamples; i += 8)
        {
            _synth->generate(_sampleBuffer);
            auto offset = bufferToFill.startSample + i;
            bufferToFill.buffer->copyFrom(0, offset, _sampleBuffer, 8);
            bufferToFill.buffer->copyFrom(1, offset, _sampleBuffer, 8);
            
            if (DUMP_DEBUG_STREAM)
            {
                _dbgStream->write(reinterpret_cast<const char*>(_sampleBuffer.data()), _sampleBuffer.size() * sizeof(float));
            }
        }
    }

    void releaseResources() override
    {
        
    }

    //==============================================================================
    void paint(Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        if (_audioDeviceSelector)
        {
            _audioDeviceSelector->setSize(getWidth(), getHeight());
        }
    }


private:
    //==============================================================================

    // Your private member variables go here...
    moddlib::SampleBuffer _sampleBuffer;
    
    std::unique_ptr<AudioDeviceSelectorComponent> _audioDeviceSelector;
    std::unique_ptr<moddlib::WaveTableSynth> _synth;
    std::unique_ptr<std::ofstream> _dbgStream;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
    
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }
