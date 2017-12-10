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
        static uint32_t getMaxHarmonics(float baseFrequency)
        {
            return static_cast<uint32_t>(Engine::instance().getFrameRate() / (3.0f * baseFrequency) + 0.5f);
        }

        constexpr static size_t getMinTableSize(uint32_t maxHarmonic, uint32_t overSampling = 1)
        {
            return roundUpToPower2(maxHarmonic) * 2 * overSampling;
        }
    };

    struct IdentityPartialScaler
    {
        float operator()(size_t partial, size_t numPartials)
        {
            return 1;
        }
    };

    struct HannPartialScaler
    {
        float operator()(size_t partial, size_t numPartials)
        {
            float val = std::cos(simd::pi<float> * 0.5f * partial / numPartials);
            return val * val;
        }
    };

    struct HammingPartialScaler
    {
        float operator()(size_t partial, size_t numPartials)
        {
            return 0.53836f - 0.46164f * std::cos((partial - numPartials) * simd::pi<float> / numPartials);
        }
    };

    template <typename PartialsScaler = HannPartialScaler>
    struct WaveTable
    {
        WaveTable() {}
        
        WaveTable(size_t tableSize) : _buffer(tableSize)
        {
            // only support 2^x wave table sizes
            assert(tableSize && ((tableSize & (tableSize -1)) == 0));
        }

        void fillSinesum(const std::vector<float>& partials, size_t numPartials)
        {
            assert(_buffer.size() > 0);
            
            size_t tableSize = _buffer.size();
            float phase = 0;
            const double phaseIncr = ( 2.0 * M_PI ) / (double)tableSize;

            PartialsScaler scaler;
            for (size_t i = 0; i < tableSize; ++i)
            {
                float partialPhase = phase;
                for (size_t p = 0; p < numPartials; ++p)
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
            size_t tableSize = _buffer.size();
            float lookup = 0.5f * (phase + 1) * tableSize;
            size_t index1 = static_cast<size_t>(lookup);
            size_t index2 = ( index1 + 1 ) & ( tableSize - 1 ); // faster mod that only works if tableSize is a power of 2
            float val1 = _buffer[index1];
            float val2 = _buffer[index2];
            float frac = lookup - (float)index1;

            return val2 + frac * ( val2 - val1 );
        }

    private:
        AlignedMemory _buffer;
    };

    template <typename PartialsScaler = HannPartialScaler>
    struct WaveTable2D
    {
        using WaveTableType = WaveTable<PartialsScaler>;
        
        WaveTable2D(float baseFrequency, const std::vector<float> partials, uint32_t overSampling = 1)
            : _baseFrequency(baseFrequency)
        {
            auto iter = std::find_if(partials.rbegin(), partials.rend(), [](auto v) { return v > 0; });
            auto highestPartial = static_cast<uint32_t>(partials.size() - std::distance(iter, partials.rbegin()));
            
            auto numTables = static_cast<size_t>(std::ceil(std::log2(highestPartial)));
            auto tableSize = waveTable::getMinTableSize(highestPartial, overSampling);
            
            _tables = std::vector<WaveTableType>(numTables);
            for (auto i = 0; i <  numTables; ++i)
            {
                auto maxPartial = waveTable::getMaxHarmonics(baseFrequency);
                _tables[i] = WaveTableType(tableSize);
                _tables[i].fillSinesum(partials, maxPartial);
                baseFrequency *= 2;
            }
        }

    private:
        float _baseFrequency;
        std::vector<WaveTableType> _tables;
    };

}