//
//  Envelope.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 1/10/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#ifndef Envelope_h
#define Envelope_h

namespace moddlib
{
    struct EnvelopeSegSustain
    {
        void reset()
        {
            _state = _setup;
        }
        
        float currentValue()
        {
            return _state.value;
        }
        
        void setLength(uint32_t length)
        {
            setup(length, _setup.value);
        }
        
        void setStart(float start)
        {
            setup(_setup.length, start);
        }
        
        void setEnd(float end)
        {
            setup(_setup.length, end);
        }
        
        void overwriteStart(float start)
        {
            _state.value = start;
        }
        
        void setup(uint32_t length, float end)
        {
            _setup.length = length;
            _setup.count  = length;
            _setup.value  = end;
        }
        
        uint32_t generate(uint32_t max, float* dest)
        {
            uint32_t numGenerated = std::min(_state.count, max);
            for (size_t i = 0; i < numGenerated; ++i)
            {
                dest[i] = _state.value;
            }
            _state.count -= numGenerated;
            return numGenerated;
        }
    
    private:
    
        struct State
        {
            State() : count(0), length(0), value(0) {}
            
            uint32_t count;
            uint32_t length;
            float value;
        } _state, _setup;
    };

    struct EnvelopeSegLinear
    {
        void reset()
        {
            _state = _setup;
        }
                float currentValue()
        {
            return _state.value;
        }
        
        void setLength(uint32_t length)
        {
            setup(length, _setup.value, _setup.end);
        }
        
        void setStart(float start)
        {
            setup(_setup.length, start, _setup.end);
        }
        
        void setEnd(float end)
        {
            setup(_setup.length, _setup.value, end);
        }
        
        void overwriteStart(float start)
        {
            _state.value = start;
            if (_state.length > 0)
            {
                _state.incr = (_state.end - _state.value) / _state.length;
            }
        }
        
        void setup(uint32_t length, float start, float end)
        {
            _setup.length = length;
            _setup.count  = length;
            _setup.end    = end;
            _setup.value  = start;
            
            if (_setup.length > 0)
            {
                _setup.incr = (_setup.end - _setup.value) / _setup.length;
            }
        }
        
        uint32_t generate(uint32_t max, float* dest)
        {
            uint32_t numGenerated = 0;
            while (max > 0 && _state.count > 0)
            {
                dest[numGenerated] = _state.value;
                _state.value += _state.incr;
                numGenerated++;
                max--;
                _state.count--;
            }
            return numGenerated;
        }
        
    private:
        struct State
        {
            State() : count(0), length(0), value(0) {}
            
            uint32_t count;
            uint32_t length;
            float value;
            float end;
        
            float incr;
        } _state, _setup;
    };

    struct EnvelopeSegExp
    {
        EnvelopeSegExp() : _state() {}
     
        void reset()
        {
            _state = _setup;
        }
        
        float currentValue()
        {
            return _state.value;
        }
        
        void setLength(uint32_t length)
        {
            setup(length, _setup.value, _setup.end, _setup.bias);
        }
        
        void setStart(float start)
        {
            setup(_setup.length, start, _setup.end, _setup.bias);
        }
        
        void setEnd(float end)
        {
            setup(_setup.length, _setup.value, end, _setup.bias);
        }
        
        void setBias(float bias)
        {
            setup(_setup.length, _setup.value, _setup.end, bias);
        }
        
        void setup(uint32_t length, float start, float end, float bias)
        {
            configure(_setup, length, start, end, bias);
            _setup.count = length;
        }
        
        void overwriteStart(float start)
        {
            configure(_state, _state.length, start, _state.end, _state.bias);
        }
        
        uint32_t generate(uint32_t max, float* dest)
        {
            uint32_t numGenerated = 0;
            while (max > 0 && _state.count > 0)
            {
                dest[numGenerated] = _state.value;
                _state.exp *= _state.incr;
                _state.value = ((_state.exp - _state.bias) * _state.range) + _state.offset;
                numGenerated++;
                max--;
                _state.count--;
            }
            return numGenerated;
        }
        
    private:
        struct State
        {
            State() : count(0), length(0), value(0), bias(-1), exp(-1) {}
            
            uint32_t count;
            uint32_t length;
            float value;
            float end;
            float incr;
            float exp;
            float bias;
            float range;
            float offset;
        } _state, _setup;
        
    private:
    
        static void configure(State& state, uint32_t length, float start, float end, float bias)
        {
            if (length > 0)
            {
                float ratio, exp;
                if (end >= start)
                {
                    state.range  = end - start;
                    state.offset = start;
                    exp     = bias;
                    ratio   = (1 + bias) / exp;
                }
                else
                {
                    state.range  = start - end;
                    state.offset = end;
                    exp     = 1 + bias;
                    ratio   = bias / exp;
                }
                
                if (state.exp != exp || state.length != length)
                {
                    state.incr   = std::pow(ratio, 1.f / length);
                    state.exp    = exp;
                    state.length = length;
                }
            }
            
            state.bias   = bias;
            state.end    = end;
            state.value  = start;

        }
    };

    struct EnvelopeSegLog
    {
        EnvelopeSegLog() : _setup() {}
    
        void reset()
        {
            _state = _setup;
        }
        
        float currentValue()
        {
            return _state.value;
        }
        
        void setLength(uint32_t length)
        {
            setup(length, _setup.value, _setup.end, _setup.bias);
        }
        
        void setStart(float start)
        {
            setup(_setup.length, start, _setup.end, _setup.bias);
        }
        
        void setEnd(float end)
        {
            setup(_setup.length, _setup.value, end, _setup.bias);
        }
        
        void setBias(float bias)
        {
            setup(_setup.length, _setup.value, _setup.end, bias);
        }
        
        void overwriteStart(float start)
        {
            configure(_state, _state.length, start, _state.end, _state.bias);
        }
        
        void setup(uint32_t length, float start, float end, float bias)
        {
            configure(_setup, length, start, end, bias);
            _setup.count = length;
        }
        
        uint32_t generate(uint32_t max, float* dest)
        {
            uint32_t numGenerated = 0;
            while (max > 0 && _state.count > 0)
            {
                dest[numGenerated] = _state.value;
                _state.exp *= _state.incr;
                _state.value = ((1.f - (_state.exp - _state.bias)) * _state.range) + _state.offset;
                numGenerated++;
                max--;
                _state.count--;
            }
            return numGenerated;
        }
        
    private:
        struct State
        {
            State() : count(0), length(0), value(0), bias(-1), exp(-1) {}
            
            uint32_t count;
            uint32_t length;
            float value;
            float end;
            float incr;
            float exp;
            float bias;
            float range;
            float offset;
        } _state, _setup;
        
    private:
    
        static void configure(State& state, uint32_t length, float start, float end, float bias)
        {
            if (length > 0)
            {
                float ratio, exp;
                if (end >= start)
                {
                    state.range  = end - start;
                    state.offset = start;
                    exp     = 1 + bias;
                    ratio   = bias / exp;
                }
                else
                {
                    state.range  = start - end;
                    state.offset = end;
                    exp     = bias;
                    ratio   = (1 + bias) / exp;
                }
                
                if (state.exp != exp || state.length != length)
                {
                    state.incr   = std::pow(ratio, 1.f / length);
                    state.exp    = exp;
                    state.length = length;
                }
            }
            
            state.bias  = bias;
            state.end   = end;
            state.value = start;
        }
    };

    template <typename... SegTs>
    struct SegmentedEnvelopeBase
    {
        SegmentedEnvelopeBase()  :
            _currentSegment(0)
        {}
        
        using SizeT = std::integral_constant<int, sizeof...(SegTs)>;
        
        constexpr size_t size()
        {
            return sizeof...(SegTs);
        }
        
        template <size_t I>
        auto& getSegment()
        {
            static_assert(I < sizeof...(SegTs), "no such segment");
            return std::get<I>(_segments);
        }
        
        template <size_t I>
        void setup(uint32_t length, float start, float end)
        {
            getSegment<I>().setup(length, start, end);
        }
        
        void reset()
        {
            tupleForEach(_segments, [](auto&& seg) { seg.reset(); });
            _currentSegment = 0;
        }

        size_t getCurrentSegment()
        {
            return _currentSegment;
        }
        
        
    protected:
        
        void setCurrentSegment(size_t segment)
        {
            _currentSegment = segment;
        }
        
        void incrCurrentSegment()
        {
            ++_currentSegment;
        }
        
        size_t seekSegment()
        {
            return 0;
        }
        
    private:
    
        template <size_t I = 0> std::enable_if_t<I < sizeof...(SegTs),
        size_t> seekSegmentImpl(uint32_t samplePos, uint32_t lastSamplePos = 0)
        {
            auto nextSegmentStart = lastSamplePos + getSegment<I>().getLength();
            if (nextSegmentStart > samplePos)
            {
                return I;
            }
            else
            {
                return seekSegmentImpl<I + 1>(samplePos, nextSegmentStart);
            }
        }
        
        template <size_t Size = 0> std::enable_if_t<Size >= sizeof...(SegTs),
        size_t> seekSegmentImpl(uint32_t samplePos, uint32_t lastSamplePos = 0)
        {
            return sizeof...(SegTs);
        }

        size_t _currentSegment;
        std::tuple<SegTs...> _segments;
    };

    template <size_t SizeT, typename ...SegmentsT>
    struct SegmentedEnvelopeKernel : SegmentedEnvelopeBase<SegmentsT...>
    {
    };

    template <typename ...SegmentsT>
    struct SegmentedEnvelopeKernel <1, SegmentsT...> : SegmentedEnvelopeBase<SegmentsT...>
    {
        uint32_t generate(uint32_t max, float* dest)
        {
            return this->template getSegment<0>().generate(max, dest);
        }
    };
    
    template <typename ...SegmentsT>
    struct SegmentedEnvelope : SegmentedEnvelopeKernel <sizeof...(SegmentsT), SegmentsT...> {};


    namespace detail
    {
        struct SegmentConcept
        {
            virtual ~SegmentConcept() = default;
            virtual void reset() = 0;
            virtual float currentValue() = 0;
            virtual void overwriteStart(float start) = 0;
            virtual uint32_t generate(uint32_t max, float* dest) = 0;
            virtual void* getSegment() = 0;
        };
        
        template <typename SegmentT>
        struct SegmentModel : public SegmentConcept
        {
            void reset() override
            {
                _segment.reset();
            }
            
            float currentValue() override
            {
                return _segment.currentValue();
            }
            
            void overwriteStart(float start) override
            {
                _segment.overwriteStart(start);
            }
            
            uint32_t generate(uint32_t max, float* dest) override
            {
                return _segment.generate(max, dest);
            }
            
            void* getSegment() override
            {
                return &_segment;
            }
            
            SegmentT _segment;
        };

    }
    
    enum EnvelopeState { IDLE, ATTACKING, LOOPING, RELEASING };
    
    template <typename ContainerT>
    struct MultiSegmentEnvelopeBase
    {
        MultiSegmentEnvelopeBase()
        {
            setNoLoop();
            _currentSegment = static_cast<uint8_t>(_segments.size());
        }
        
        template <typename SegmentT>
        SegmentT& getSegment(uint8_t index)
        {
            return *static_cast<SegmentT*>(_segments[index]->getSegment());
        }
        
        template <typename SegmentT>
        void setSegment(uint8_t index)
        {
            _segments[index] = std::make_unique<detail::SegmentModel<SegmentT>>();
        }
        
        void reset()
        {
            for_each(_segments.begin(), _segments.end(), [](fref seg) { seg->reset(); });
            _currentSegment = 0;
        }
        
        void generate(uint32_t max, float* dest, bool triggerOn = true)
        {
            auto generated = fill(max, dest, triggerOn);
            for (size_t i = generated; i < max; ++i)
            {
                dest[i] = 0;
            }
        }
        
        uint32_t fill(uint32_t max, float* dest, bool triggerOn = true)
        {
            if (!(triggerOn) && _currentSegment < _releaseStart)
            {
                auto currentValue = _segments[_currentSegment]->currentValue();
                _segments[_currentSegment]->reset();
                _currentSegment = _releaseStart;
                _segments[_currentSegment]->overwriteStart(currentValue);
            }
            
            uint32_t generatedSamples = 0;
            while (_currentSegment < _segments.size())
            {
                generatedSamples += _segments[_currentSegment]->generate(max - generatedSamples, &dest[generatedSamples]);
                
                if (generatedSamples < max)
                {
                    _segments[_currentSegment]->reset();
                    if (_shouldLoop > 0 && _currentSegment == _releaseStart - 1)
                    {
                        _currentSegment = _loopStart;
                    }
                    else
                    {
                        _currentSegment++;
                    }
                }
                else
                {
                    break;
                }
            }
            return generatedSamples;
        }
        
        bool isPlaying()
        {
            return _currentSegment < _segments.size();
        }
        
        void setLoop(uint8_t loopStart, uint8_t releaseStart)
        {
            _shouldLoop   = 1;
            _loopStart    = loopStart;
            _releaseStart = releaseStart;
        }
        
        void setNoLoop()
        {
            _shouldLoop   = 0;
            _releaseStart = 0;
        }
                
    protected:
    
        uint8_t _currentSegment;
        uint8_t _shouldLoop;
        uint8_t _loopStart;
        uint8_t _releaseStart;
        
        ContainerT _segments;
    };
    
    template <size_t I>
    struct FixedSegmentEnvelope : MultiSegmentEnvelopeBase<std::array<std::unique_ptr<detail::SegmentConcept>, I>>
    {};
    
    struct MultiSegmentEnvelope : MultiSegmentEnvelopeBase<std::vector<std::unique_ptr<detail::SegmentConcept>>>
    {
        template <typename SegmentT>
        void addSegment()
        {
            _segments.push_back(std::make_unique<detail::SegmentModel<SegmentT>>());
        }
    };
}

#endif /* Envelope_h */
