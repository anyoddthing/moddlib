/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include <fstream>
#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"
#include "aot_moddlib/instruments/SubtractiveSynth.hpp"
#include "aot_moddlib/instruments/WaveTableSynth.hpp"
#include "MidiKnobMatrix.h"
#include "Oscilloscope.h"
#include "Spectrogram.h"

static const bool DUMP_DEBUG_STREAM = false;

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
        setAudioChannels(0, 2);
        
        _openGLContext.attachTo(*getTopLevelComponent());
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
        
        _openGLContext.detach();
    }

    //==============================================================================
    // ui
    
    void sliderValueChanged(uint slider, uint8_t value)
    {
        _synth->getMidiPort(slider).setControlValue(value);
    }
    
    void initComponents()
    {
        using namespace std::placeholders;
//        _audioDeviceSelector = std::make_unique<AudioDeviceSelectorComponent>(
//            deviceManager, 0, 0, 2, 2, true, false, true, false);
//        addAndMakeVisible(_audioDeviceSelector.get());
        
        _midiKnobs = std::make_unique<MidiKnobMatrix>();
        _midiKnobs->setListener([this](int slider, uint8_t value) {
            sliderValueChanged(slider, value);
        });
        
        addAndMakeVisible(_midiKnobs.get());
        
        _oscilloscope = std::make_unique<Oscilloscope>();
        addAndMakeVisible(_oscilloscope.get());
        
        _spectrogram = std::make_unique<Spectrogram>();
        addAndMakeVisible(_spectrogram.get());
        
        const StringArray midiInputs(MidiInput::getDevices());
        for (int i = 0; i < midiInputs.size(); ++i)
        {
            deviceManager.setMidiInputEnabled(midiInputs[i], true);
            deviceManager.addMidiInputCallback(midiInputs[i], this);
        }
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();

        auto top = bounds.removeFromTop(200);
        _midiKnobs->setBounds(top);
        
//        _audioDeviceSelector->setBounds(top);
        _oscilloscope->setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));
        _spectrogram->setBounds(bounds);
    }
    
    void paint(Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    }


    //==============================================================================
    // audio
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        using namespace moddlib;
        if (DUMP_DEBUG_STREAM)
        {
            _dbgStream = std::make_unique<std::ofstream>("testout.bin", std::ios::binary | std::ios::out);
        }
        
        _waveTable = std::make_shared<WaveTable2D>();
        _waveTable->setupTables(40, waveTable::sawPartials());
        
        _synth = std::make_unique<SynthType>();
        
        _synth->configureVoices([this](uint, auto& voice)
        {
            module<CircuitType::oscModule>(voice.getCircuit()).setupTables(_waveTable);
        });
        
//        bool error = CircuitType::error;
        bindMidiController<Sel_<CircuitType::envModule, ADSREnvelopeGenerator::releaseIn>>(1, 0, 3);
        bindMidiController<Sel_<CircuitType::filterFreqIn>>(2, 0, 1);
        bindMidiController<Sel_<CircuitType::filterQIn>>(3, 0, 1);
    }
    
    template <typename SelT>
    void bindMidiController(int midiController, float min, float max)
    {
        auto& midiPort = _synth->getMidiPort(midiController);
        _synth->bindMidiController(moddlib::MidiPortMapper<CircuitType, SelT>(midiController, true));
        midiPort.setRange(min, max);
        
        _midiKnobs->setValue(midiController - 1, midiPort.getControlValue());
    }

    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
    {
        jassert(bufferToFill.buffer->getNumChannels() == 2);
        
        for (auto i = 0; i < bufferToFill.numSamples; i += 8)
        {
            _synth->generate(_sampleBuffer);
            auto offset = bufferToFill.startSample + i;
            bufferToFill.buffer->copyFrom(0, offset, _sampleBuffer.data(), 8);
            bufferToFill.buffer->copyFrom(1, offset, _sampleBuffer.data(), 8);
            
            if (DUMP_DEBUG_STREAM)
            {
                _dbgStream->write(reinterpret_cast<const char*>(_sampleBuffer.data()), _sampleBuffer.size() * sizeof(float));
            }
        }
        
        _oscilloscope->read(bufferToFill.buffer->getReadPointer(0), bufferToFill.numSamples);
        _spectrogram->read(bufferToFill.buffer->getReadPointer(0), bufferToFill.numSamples);
    }

    // Midi Callback
    void handleIncomingMidiMessage(juce::MidiInput *source, const juce::MidiMessage &message) override
    {
        if (_synth)
        {
            DBG(message.getDescription());
            _synth->queueMessage(moddlib::midi::createMidiMessage(message.getRawData()));
            
            if (message.isController() && message.getControllerNumber() < 16)
            {
                auto incr = message.getControllerValue() - 64;
                if (incr != 0)
                {
                    _midiKnobs->incrementValue(message.getControllerNumber() - 1, incr);
                }
            }
        }
    }
    
    void releaseResources() override {}
private:
    //==============================================================================

    using SynthType = moddlib::SubtractiveSynth<moddlib::WaveTableOscillator>;
    using CircuitType = typename SynthType::CircuitType;
    
//    using SynthT = moddlib::WaveTableSynth;
    std::unique_ptr<SynthType> _synth;
    std::shared_ptr<moddlib::WaveTable2D> _waveTable;
    
    moddlib::SampleBuffer _sampleBuffer;
    std::unique_ptr<MidiKnobMatrix> _midiKnobs;
    std::unique_ptr<AudioDeviceSelectorComponent> _audioDeviceSelector;
    std::unique_ptr<Oscilloscope> _oscilloscope;
    std::unique_ptr<Spectrogram> _spectrogram;
    std::unique_ptr<std::ofstream> _dbgStream;
    
    OpenGLContext _openGLContext;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }
