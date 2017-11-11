//
//  EnvelopeBench.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 1/16/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>

#include "bench.hpp"
#include "Envelope.hpp"
#include "FastPower.hpp"

class EnvelopeSegExpBL : public usyn::EnvelopeSeg
{
public:
    
    EnvelopeSegExpBL() : _bias(-1) {}
    
    void setup(uint32_t length, float start, float end, float bias)
    {
        if (length > 0)
        {
            _range = std::fabs(end - start);
            _offset = std::min(start, end);
            
            if (_bias != bias || _length != length)
            {
                _bias = bias;
                
                float ratio;
                if (end >= start)
                {
                    _exp = bias;
                    ratio = (1 + bias) / _exp;
                }
                else
                {
                    _exp = 1 + bias;
                    ratio = bias / _exp;
                }
                _incr = std::pow(ratio, 1.f / length);
            }
        }
        
        _length = length;
        _start  = start;
        _end    = end;
    }
    
private:

    float _incr;
    float _exp;
    float _bias;
    float _range;
    float _offset;
};

class EnvelopeSegExpFast : public usyn::EnvelopeSeg
{
public:
    
    EnvelopeSegExpFast() : _bias(-1), _pow(0) {}
    
    void setup(uint32_t length, float start, float end, float bias)
    {
        if (length > 0)
        {
            _range = std::fabs(end - start);
            _offset = std::min(start, end);
            
            if (_bias != bias)
            {
                _bias = bias;
                
                float ratio;
                if (end >= start)
                {
                    _exp = bias;
                    ratio = (1 + bias) / _exp;
                }
                else
                {
                    _exp = 1 + bias;
                    ratio = bias / _exp;
                }
                _pow = usyn::FastPowP8(ratio);
            }
            
            if (_length != length)
            {
                _incr = _pow(1.f / length);
            }
        }
        
        _length = length;
        _start  = start;
        _end    = end;
    }
    
private:
    usyn::FastPowP8 _pow;
    float _incr;
    float _exp;
    float _bias;
    float _range;
    float _offset;
};


EnvelopeSegExpBL envelope;
struct EnvelopeBaseLineBench
{
    
    NOINLINE static void run(benchpress::context* ctx)
    {
        for (auto i = 0; i < ctx->num_iterations(); ++i)
        {
            envelope.setup(1024 + i % 2, 0, 1024, 0.25f);
        }
    }
};

EnvelopeSegExpFast fastEnvelope;
struct EnvelopeFast
{
    
    NOINLINE static void run(benchpress::context* ctx)
    {
        for (auto i = 0; i < ctx->num_iterations(); ++i)
        {
            fastEnvelope.setup(1024 + i % 2, 0, 1024, 0.25f);
        }
    }
};


BENCHMARK("EnvelopeSetup BL", [](benchpress::context* ctx)
{
    EnvelopeBaseLineBench::run(ctx);
});

BENCHMARK("EnvelopeSetup Fast", [](benchpress::context* ctx)
{
    EnvelopeFast::run(ctx);
});



