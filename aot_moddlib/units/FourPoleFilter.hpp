//
//  FourPoleFilter.hpp
//  moddlib
//
//  Created by Daniel Doubleday on 1/13/18.
//  Copyright © 2018 Daniel Doubleday. All rights reserved.
//

#pragma once

#include <cmath>
#include "../core/core.hpp"

namespace moddlib
{
    namespace
    {
        static constexpr float SATURATION_COEFFICIENT = 0.2;
        
        static constexpr float cubicPolynomial(float x)
        {
            return x - (x * x * x * SATURATION_COEFFICIENT);
        }
        
        // Inflection point where slope is zero.
        static constexpr float SATURATION_UPPER_INPUT = 1.0 / 0.774596669241483; //1 / std::sqrt(3.0 * SATURATION_COEFFICIENT);
        static constexpr float SATURATION_LOWER_INPUT = 0.0 - SATURATION_UPPER_INPUT;
        static constexpr float SATURATION_UPPER_OUTPUT = cubicPolynomial(SATURATION_UPPER_INPUT);
        static constexpr float SATURATION_LOWER_OUTPUT = cubicPolynomial(SATURATION_LOWER_INPUT);
        
        static constexpr float MINIMUM_FREQUENCY = 1.0; // blows up if near 0.01
        static constexpr float MINIMUM_Q = 0.00001;
    }
    
    struct FourPoleFilter:
        Generator<FourPoleFilter>,
        InputBank<
            Inputs<3, StreamInput>>,
        OutputBank<
            Outputs<1, StreamOutput>>
    {
        using freqIn = BIn_<0, 1>;
        using qIn = BIn_<0, 1>;
        
        void init()
        {
            _x.fill(0);
            _y.fill(0);
        }
        
        void doGenerate()
        {
            auto& in =   input<mainIn>().buffer();
            auto& freq = input<freqIn>().buffer();
            auto& q    = input<qIn>().buffer();
            auto& out  = output<mainOut>().buffer();
            
            out.forEach([&](auto i, auto& val)
            {
                computeCoefficients(freq[i], q[i]);
                if (oversampled)
                {
                    oneSample(0.0f);
                }
                
                val = oneSample(in[0]);
            });
        }
        
    private:
    
        float oneSample(float x)
        {
            float coeff = 0.3;
            x -= _y[3] * feedback; // feedback
            x *= 0.35013 * fTo4th;
            _y[0] = x + coeff * _x[0] + (1 - f) * _y[0]; // pole 1
            _x[0] = x;
            _y[1] = _y[0] + coeff * _x[1] + (1 - f) * _y[1]; // pole 2
            _x[1] = _y[0];
            _y[2] = _y[1] + coeff * _x[2] + (1 - f) * _y[2]; // pole 3
            _x[2] = _y[1];
            _y[3] = _y[2] + coeff * _x[3] + (1 - f) * _y[3]; // pole 4
            _y[3] = clip(_y[3]);
            _x[3] = _y[2];
            
            return _y[3];
        }
        
        constexpr float clip(float x)
        {
            if (x > SATURATION_UPPER_INPUT)
            {
                return SATURATION_UPPER_OUTPUT;
            }
            else if (x < SATURATION_LOWER_INPUT)
            {
                return SATURATION_LOWER_OUTPUT;
            }
            else
            {
                return cubicPolynomial(x);
            }
        }
        
        void computeCoefficients(float freq, float q)
        {
            float normalizedFrequency = freq * Engine::instance().getFramePeriod();
            float fudge = 4.9f - 0.27f * q;
            if (fudge < 3.0f)
                fudge = 3.0f;
            
            f = normalizedFrequency * (oversampled ? 1.0f : 2.0f) * fudge;

            float fSquared = f * f;
            fTo4th = fSquared * fSquared;
            feedback = 0.5f * freq * (1.0f - 0.15f * fSquared);
        }
        
    private:
    
        std::array<float, 4> _x;
        std::array<float, 4> _y;

        bool oversampled = true;
        float f;
        float fTo4th;
        float feedback;
    };
}
