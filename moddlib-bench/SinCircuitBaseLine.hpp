//
//  SinCircuitBaseLine.h
//  unitsynth
//
//  Created by Daniel Doubleday on 1/10/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#pragma once

#include <vector>
#include <memory>

#include "moddlib.hpp"

namespace moddlib
{
    struct BLOutput
    {
    
    };
    
    struct BLSingleValOutput
    {
        virtual float getValue() = 0;
        virtual void setValue(float value) = 0;
        
        void up()
        {
            setValue(1.f);
        }
        
        void down()
        {
            setValue(0);
        }
        
        operator float()
        {
            return getValue();
        }
        
        BLSingleValOutput& operator=(float value)
        {
            setValue(value);
            return *this;
        }
    };
    
    struct BLFloatOutput : BLSingleValOutput, FloatBuffer
    {
        void setValue(float value) override
        {
            _value = value;
        }
        
        float getValue() override
        {
            return FloatBuffer::getValue();
        }
        
    };
    
    struct BLStreamOutput : BLOutput
    {
        SampleBuffer& buffer()
        {
            return _buffer;
        }
        
        simd::Vec vec(size_t offset = 0)
        {
            return simd::Vec(ptr(offset));
        }
        
        float* ptr(size_t offset = 0)
        {
            return buffer().data() + offset;
        }
        
    private:
    
        SampleBuffer _buffer;
    };
    
    struct BLGeneratorDelegate
    {
        virtual void pullData() = 0;
        virtual void reset() = 0;
    };
    
    struct BLUnit
    {
        virtual void init() = 0;
        virtual void pull() = 0;
        virtual void reset() = 0;
        virtual void generate() = 0;
    };
    
    struct BLGeneratorBase : BLUnit
    {
        void init() override {}
        
        void addListener(BLGeneratorDelegate* delegate)
        {
            _listeners.push_back(delegate);
        }
        
        void pull() override
        {
            std::for_each(_listeners.begin(), _listeners.end(), [](auto& input) { input->pullData(); });
        }
        
        void reset() override
        {
            std::for_each(_listeners.begin(), _listeners.end(), [](auto& input) { input->reset(); });
        }
        
    private:
        
        std::vector<BLGeneratorDelegate*> _listeners;
    };
    
    
    struct BLGenerator : BLGeneratorBase
    {
        virtual void doGenerate() = 0;
        
        void generate() override
        {
            pull();
            doGenerate();
            reset();
        };
    };
    
    struct BLCircuit : BLGeneratorBase
    {
        void generate() override
        {
            pull();
            
            std::for_each(_modules.begin(), _modules.end(), [](auto& module) { module->generate(); });
        
            reset();
        }
    
        template <typename UnitT>
        UnitT* addUnit()
        {
            _modules.push_back(std::make_unique<UnitT>());
            return static_cast<UnitT*>(_modules.back().get());
        }
        
    private:
    
        std::vector<std::unique_ptr<BLUnit>> _modules;
    };
    
    struct BLInput
    {
        virtual bool isConnected() = 0;
    };
    
    struct BLSingleValInput : BLInput
    {
        virtual float getValue() = 0;
        virtual void setSource(FloatBuffer&) = 0;
        
        virtual void setup() {}
        
        virtual operator float()
        {
            return getValue();
        }
        
        virtual bool isUp()
        {
            return getValue() > 0.01f;
        }

        virtual bool isDown()
        {
            return !isUp();
        }
    };
    
    struct BLStreamInput : BLInput
    {
        SampleBuffer& buffer()
        {
            return _source->buffer();
        }
        
        simd::Vec vec(size_t offset = 0)
        {
            return simd::Vec(ptr(offset));
        }
        
        float* ptr(size_t offset = 0)
        {
            return buffer().data() + offset;
        }
    
        void setSource(BLStreamOutput& source)
        {
            _source = &source;
        }

        bool isConnected()
        {
            return _source != nullptr;
        }
        
    private:
        
        BLStreamOutput* _source;
        
    };
    
    struct BLFloatInput : BLSingleValInput
    {
        float getValue() override
        {
            return _source->getValue();
        }
        
        bool isConnected() override
        {
            return _source != &FlatOutput::sharedOutput();
        }

        void setSource(FloatBuffer& source) override
        {
            _source = &source;
        }

        
    private:
        
        FloatBuffer* _source;
    };
    
    struct BLTrackingInput : BLFloatInput, BLGeneratorDelegate
    {
        BLTrackingInput(BLGenerator* generator)
        {
            generator->addListener(this);
        }
        
        void pullData() override {}
        
        void reset() override
        {
            _lastValue = getValue();
        }
        
        bool changed()
        {
            return getValue() != _lastValue;
        }
        
        float getLastValue()
        {
            return _lastValue;
        }
        
    private:
    
        float _lastValue;
    
    };
    
    struct BLDefaultInput : BLTrackingInput
    {
        BLDefaultInput(BLGenerator* generator) : BLTrackingInput(generator)
        {
            setSource(_defaultValue);
        }
        
        void setup(float value)
        {
            _defaultValue.setValue(value);
        }
        
    private:
    
        BLFloatOutput _defaultValue;
    };
    
    struct BLTriggerInput : BLTrackingInput
    {
        BLTriggerInput(BLGenerator* generator) : BLTrackingInput(generator)
        {}
        
        bool isTriggered()
        {
            return getValue() > getLastValue();
        }
    };
    
    
    struct BLPhaseGenerator : BLGenerator
    {
        BLTriggerInput triggerIn;
        BLFloatInput freqIn;
        BLStreamOutput mainOut;
        
        BLPhaseGenerator() : triggerIn(this)
        {
        }
        
        void init() override
        {
            _lastIncrement  = 0;
            _phaseIncrement = 0;
        }

        void doGenerate() override
        {
            auto triggered = triggerIn.isTriggered();

            if (triggered)
            {
//                LOG("triggered");
                auto frequency  = freqIn.getValue();
                _phaseIncrement = Engine::instance().convertFrequencyToPhaseIncrement(frequency);
                if (_lastIncrement > 0)
                {
                    auto deltaIncr = _phaseIncrement - _lastIncrement;
                    simd::Vec deltaV = simd::Vec(1, 2, 3, 4) * deltaIncr;

                    // not resetting phase here
                    simdv::initPhase(
                        mainOut.ptr(),
                        mainOut.vec() + deltaV + 8 * _lastIncrement,
                        _phaseIncrement);

                    _lastIncrement = _phaseIncrement;
                }
                else
                {
                    simdv::initPhase(mainOut.ptr(), _phaseIncrement);
                    _lastIncrement = _phaseIncrement;
                }
            }
            else
            {
                simdv::updatePhase(mainOut.ptr(), _phaseIncrement);
            }
        }

    private:

        float wrapPhase(float val)
        {
            return (val > 1.0f) ? val - 2.0f : val;
        }

    private:
        float _lastIncrement;
        float _phaseIncrement;
    };
    
    struct BLSineOscillator : BLGenerator
    {
        BLStreamInput phaseIn;
        BLStreamOutput mainOut;
        
        void doGenerate()
        {
            simdv::fastSin(mainOut.buffer(), phaseIn.buffer());
        }
    };
    
    struct BLEnvelopeADSR : BLGenerator
    {
        BLStreamInput mainIn;
        BLTriggerInput triggerIn;
        
        
        BLDefaultInput attackIn;
        BLDefaultInput decayIn;
        BLDefaultInput sustainIn;
        BLDefaultInput releaseIn;

        BLDefaultInput targetRatioAIn;
        BLDefaultInput targetRatioDRIn;
        
        BLStreamOutput mainOut;
        BLStreamOutput envOut;

        enum State { IDLE, ATTACKING, DECAYING, SUSTAINING, RELEASING };

        BLEnvelopeADSR() : _state(IDLE), _level(0),
            _attackCoef(0), _decayCoef(0), _releaseCoef(0),
            _attackBase(0), _decayBase(0), _releaseBase(0),
            triggerIn(this), attackIn(this), decayIn(this), sustainIn(this), releaseIn(this), targetRatioAIn(this), targetRatioDRIn(this)
        {
            attackIn.setup(0.01f);
            decayIn.setup(0.2f);
            sustainIn.setup(1.f);
            releaseIn.setup(0.5f);

            targetRatioAIn.setup(0.3f);
            targetRatioDRIn.setup(0.0001f);
            
            updateParams();
        }

        void doInit()
        {
            _state = IDLE;
            _level = 0;
        }

        void doGenerate()
        {
//            updateParams();

            fref env     = envOut.buffer();

            if (triggerIn.isTriggered())
            {
                _state = ATTACKING;
            }
            else if (triggerIn.isDown() && _state != IDLE)
            {
                _state = RELEASING;
            }

            env.forEach([&](int i, float& sample)
            {
                switch (_state) {
                case IDLE:
                    break;
                case ATTACKING:
                    _level = _attackBase + _level * _attackCoef;
                    if (_level >= 1.f)
                    {
                        _level = 1.f;
                        _state = DECAYING;
                    }
                    break;
                case DECAYING:
                {
                    _level = _decayBase + _level * _decayCoef;
                    auto sustainLevel = sustainIn.getValue();
                    if (_level <= sustainLevel)
                    {
                        _level = sustainLevel;
                        _state = SUSTAINING;
                    }
                    break;
                }
                case SUSTAINING:
                    break;
                case RELEASING:
                    _level = _releaseBase + _level * _releaseCoef;
                    if (_level < Engine::DB90)
                    {
                        _level = 0.f;
                        _state = IDLE;
                    }
                }

                sample = _level;

                auto& audioIn = mainIn;
                if (audioIn.isConnected())
                {
                    simdv::mpy(mainOut.buffer().data(), mainIn.buffer().data(), env.data());
                }
            });
        }

    private:

        void updateParams()
        {
            if (attackIn.changed() || targetRatioAIn.changed())
            {
                auto attack      = attackIn.getValue();
                auto targetRatio = targetRatioAIn.getValue();

                _attackCoef = calcCoef(attack, targetRatio);
                _attackBase = (1.f + targetRatio) * (1.f - _attackCoef);
            }

            auto ratioDRChanged = targetRatioDRIn.changed();
            if (decayIn.changed() || ratioDRChanged)
            {
                auto decay       = decayIn.getValue();
                auto targetRatio = targetRatioDRIn.getValue();

                _decayCoef = calcCoef(decay, targetRatio);
                _decayBase = (sustainIn.getValue() - targetRatio) * (1.f - _decayCoef);
            }

            if (releaseIn.changed() || ratioDRChanged)
            {
                auto release     = releaseIn.getValue();
                auto targetRatio = targetRatioDRIn.getValue();
                
                _releaseCoef = calcCoef(release, targetRatio);
                _releaseBase = -targetRatio * (1.f - _decayCoef);
            }

            if (sustainIn.changed())
            {
                auto targetRatio = targetRatioDRIn.getValue();
                _decayBase = (sustainIn.getValue() - targetRatio) * (1.f - _decayCoef);
            }
        }

        float calcCoef(float rate, float targetRatio)
        {
            auto numSamples = rate * Engine::instance().getFrameRate();
            return std::exp(-std::log((1.f + targetRatio) / targetRatio) / numSamples);
        }

    private:
        static constexpr float MIN_DURATION = 1.0f / 100000.0f;

        State _state;
        float _level;

        float _attackCoef;
        float _decayCoef;
        float _releaseCoef;
        float _attackBase;
        float _decayBase;
        float _releaseBase;

        bool  _deactivateVoice;
    };
    
    struct BLTestToneGenerator : BLGenerator
    {
        BLStreamOutput mainOut;
        
        BLTestToneGenerator() : _phase(0), _buffer(64)
        {}
        
        void doGenerate() override
        {
            mainOut.buffer().copy(_buffer.data(_phase));
            _phase = (_phase + 8) % _buffer.size();
        }
    
    private:
        size_t _phase;
        TestToneBuffer _buffer;
    };
    
    struct BLSineCircuit : BLCircuit
    {
        BLPhaseGenerator* phaseGenerator;
        BLSineOscillator* sineOscillator;
        BLEnvelopeADSR* envelope;
        
        BLFloatOutput triggerIn;
        BLFloatOutput freqIn;
        
        BLSineCircuit()
        {
            phaseGenerator = addUnit<BLPhaseGenerator>();
            sineOscillator = addUnit<BLSineOscillator>();
            envelope       = addUnit<BLEnvelopeADSR>();
            
            phaseGenerator->triggerIn.setSource(triggerIn);
            phaseGenerator->freqIn.setSource(freqIn);
            envelope->triggerIn.setSource(triggerIn);
            
            sineOscillator->phaseIn.setSource(phaseGenerator->mainOut);
            envelope->mainIn.setSource(sineOscillator->mainOut);
        }
        
        SampleBuffer& mainOutput()
        {
            return envelope->mainOut.buffer();
        }
    };
    
    struct BLCustomSineCircuit : BLGenerator
    {
        BLPhaseGenerator phaseGenerator;
        BLSineOscillator sineOscillator;
        BLEnvelopeADSR envelope;
        
        BLFloatOutput triggerIn;
        BLFloatOutput freqIn;
        
        BLCustomSineCircuit()
        {
            phaseGenerator.triggerIn.setSource(triggerIn);
            phaseGenerator.freqIn.setSource(freqIn);
            envelope.triggerIn.setSource(triggerIn);
            
            sineOscillator.phaseIn.setSource(phaseGenerator.mainOut);
            envelope.mainIn.setSource(sineOscillator.mainOut);
        }
        
        SampleBuffer& mainOutput()
        {
            return envelope.mainOut.buffer();
        }
        
        void doGenerate() override
        {
            phaseGenerator.generate();
            sineOscillator.generate();
            envelope.generate();
        }
    };
    
    struct BLTestToneCircuit : BLGenerator
    {
        BLTestToneGenerator testToneGen;
        BLEnvelopeADSR envelope;
        
        BLFloatOutput triggerIn;
        BLFloatOutput freqIn;
        
        BLTestToneCircuit()
        {
            envelope.triggerIn.setSource(triggerIn);
            envelope.mainIn.setSource(testToneGen.mainOut);
        }
        
        SampleBuffer& mainOutput()
        {
            return envelope.mainOut.buffer();
        }
        
        void doGenerate() override
        {
            testToneGen.generate();
            envelope.generate();
        }
    };

}


