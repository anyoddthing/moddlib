//
//  WaveTable.hpp
//  moddlib
//
//  Created by Daniel Doubleday on 11/19/17.
//  Copyright © 2017 Daniel Doubleday. All rights reserved.
//

#pragma once

// adopted from https://github.com/cinder/Cinder/blob/master/src/cinder/audio/WaveTable.cpp

namespace moddlib
{
    namespace waveTable
    {
        static uint getMaxHarmonic(float baseFrequency, uint frameRate)
        {
            return (uint) (frameRate / (3.5f * baseFrequency) + 0.5f);
        }
        
        static uint getMaxHarmonic(float baseFrequency)
        {
            return getMaxHarmonic(baseFrequency, Engine::instance().getFrameRate());
        }

        constexpr static uint getMinTableSize(uint maxHarmonic, uint overSampling = 1)
        {
            return std::max(64u, roundUpToPower2(maxHarmonic) * 2 * overSampling);
        }
        
        static std::vector<float> sawPartials(float baseFrequency)
        {
            uint maxHarms = waveTable::getMaxHarmonic(baseFrequency);
            std::vector<float> partials(maxHarms);
            partials[0] = 1;
            for (uint i = 1; i < maxHarms; ++i)
            {
                partials[i] = 1.0f / (i + 1);
            }
            return partials;
        }
    };

    struct IdentityPartialScaler
    {
        float operator()(uint partial, uint numPartials)
        {
            return 1;
        }
    };

    struct HannPartialScaler
    {
        float operator()(uint partial, uint numPartials)
        {
            float val = std::cos(simd::pi<float> * 0.5f * partial / numPartials);
            return (val * val);
        }
    };

    struct HammingPartialScaler
    {
        float operator()(uint partial, uint numPartials)
        {
            return 0.53836f - 0.46164f * std::cos(((int) partial - (int) numPartials) * simd::pi<float> / numPartials);
        }
    };

    struct WaveTable
    {
        WaveTable() {}
        
        WaveTable(uint tableSize) : _buffer(tableSize)
        {
            // only support 2^x wave table sizes
            assert(tableSize && ((tableSize & (tableSize -1)) == 0));
        }

        float & operator [] (uint index)
        {
            return _buffer[index];
        }
        
        float* data()
        {
            return _buffer.data();
        }
        
        uint size()
        {
            return _buffer.size();
        }
        
        template <typename PartialsScaler = IdentityPartialScaler>
        void fillSinesum(std::vector<float> const & partials, uint numPartials)
        {
            assert(_buffer.size() > 0);
            _buffer.zero_mem();

            uint tableSize = _buffer.size();
            float phase = 0;
            const double phaseIncr = ( 2.0 * M_PI ) / (double)tableSize;

            PartialsScaler scaler;
            for (auto i = 0; i < tableSize; ++i)
            {
                float partialPhase = phase;
                for (auto p = 0; p < numPartials; ++p)
                {
                    _buffer[i] += scaler(p, numPartials) * partials[p] * std::sin(static_cast<float>(partialPhase));
                    partialPhase += phase;
                }

                phase += phaseIncr;
            }

            normalizeArray(_buffer.data(), _buffer.size());
        }

        float lookup(float phase)
        {
            uint tableSize = _buffer.size();
            float lookup = 0.5f * (phase + 1) * tableSize;
            uint index1 = static_cast<uint>(lookup);
            uint index2 = ( index1 + 1 ) & ( tableSize - 1 ); // faster mod that only works if tableSize is a power of 2
            float val1 = _buffer[index1];
            float val2 = _buffer[index2];
            float frac = lookup - static_cast<float>(index1);
            return val1 + frac * ( val2 - val1 );
        }

    private:
        AlignedMemory _buffer;
    };

    struct WaveTable2D
    {
        template <typename PartialsScaler = IdentityPartialScaler>
        void setupTables(float baseFrequency, std::vector<float> const & partials, uint overSampling = 1)
        {
            _baseFrequency = baseFrequency;
            auto iter = std::find_if(partials.rbegin(), partials.rend(), [](auto v) { return v > 0; });
            
            uint highestPartial = (uint) (partials.size() - std::distance(iter, partials.rbegin()));
            uint numTables = (uint) std::ceil(std::log2(highestPartial + 0.1f));
            
            uint tableSize = waveTable::getMinTableSize(highestPartial, overSampling);
            _tables = std::vector<WaveTable>(numTables);
            for (uint i = 0; i < numTables; ++i)
            {
                uint maxPartial = waveTable::getMaxHarmonic(baseFrequency);
                _tables[i] = WaveTable(tableSize);
                _tables[i].fillSinesum<PartialsScaler>(partials, std::min(maxPartial, (uint) partials.size()));
                baseFrequency *= 2;
            }
        }
        
        float lookup(float phase, float frequency)
        {
            if (_tables.size() == 0) return 0;
            
            if (frequency < _baseFrequency)
            {
                return _tables[0].lookup(phase);
            }
            else
            {
                float lookup = approxLog2(frequency / _baseFrequency);
                
                int index1 = lookup;
                int index2 = index1 + 1;
                
                if (index2 < _tables.size())
                {
                    float val1 = _tables[index1].lookup(phase);
                    float val2 = _tables[index2].lookup(phase);
                    float frac = lookup - index1;
                    return val1 + frac * ( val2 - val1 );
                }
                else
                {
                    return _tables.back().lookup(phase);
                }
            }
        }

    private:
        float _baseFrequency;
        std::vector<WaveTable> _tables;
    };

}
