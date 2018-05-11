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
        static constexpr double SATURATION_COEFFICIENT = 0.2;
        
        // Inflection point where slope is zero.
        static constexpr float SATURATION_UPPER_INPUT = 1.0 / 0.774596669241483; //1 / std::sqrt(3.0 * SATURATION_COEFFICIENT);
        static constexpr float SATURATION_LOWER_INPUT = 0.0 - SATURATION_UPPER_INPUT;
    }
    
    template <typename FloatT = double>
    struct FourPoleFilter :
        Generator<FourPoleFilter<FloatT>>,
        SingleStreamOutput,
        InputBank<
            Inputs<3, StreamInput>>
    {
        using freqIn = BIn_<0, 1>;
        using qIn    = BIn_<0, 2>;
        
        void init()
        {
            _x.fill(0);
            _y.fill(0);
        }
        
        void doGenerate()
        {
            auto& in   = input<mainIn>().buffer();
            auto& freq = input<freqIn>().buffer();
            auto& q    = input<qIn>().buffer();
            auto& out  = output<mainOut>().buffer();
            
//            computeCoefficients(freq[0], q[0]);
            
            out.forEach([&](auto i, auto& val)
            {
//                if (oversampled)
//                {
//                    oneSample(0.0f);
//                }

                val = oneSample(in[i], freq[i], q[i]);
            });
            
            _y[0] += 1.0e-26;
            _y[1] -= 1.0e-26;
        }
        
    private:
    
        float oneSample(float input, FloatT fc, FloatT res)
        {
            fc = std::max<FloatT>(0.02, fc);
            if (fc < 0.1 && fc < prevF)
            {
                auto deltaF = std::min<FloatT>(0.01, (prevF - fc) / 2);
                fc = prevF - deltaF;
            }
            prevF = fc;
            
//            res = std::max<FloatT>(MINIMUM_Q, res);
            FloatT q = res * 4;
            FloatT f  = fc * 1.16;
            FloatT fb = q * (1.0 - 0.15 * f * f);
            
            input -= _y[3] * fb;
            input *= 0.35013 * (f*f)*(f*f);
            _y[0] = input + 0.3 * _x[0] + (1 - f) * _y[0];// Pole 1
            _x[0] = input;
            _y[1] = _y[0] + 0.3 * _x[1] + (1 - f) * _y[1];// Pole 2
            _x[1] = _y[0];
            _y[2] = _y[1] + 0.3 * _x[2] + (1 - f) * _y[2];// Pole 3
            _x[2] = _y[1];
            _y[3] = _y[2] + 0.3 * _x[3] + (1 - f) * _y[3];// Pole 4
            _y[3] = clip(_y[3]);
            _x[3] = _y[2];
            return _y[3];
        }
        
        static constexpr FloatT cubicPolynomial(FloatT x)
        {
            return x - (x * x * x * SATURATION_COEFFICIENT);
        }

        constexpr FloatT clip(FloatT x)
        {
            if (x > SATURATION_UPPER_INPUT)
            {
                return cubicPolynomial(SATURATION_UPPER_INPUT);
            }
            else if (x < SATURATION_LOWER_INPUT)
            {
                return cubicPolynomial(SATURATION_LOWER_INPUT);
            }
            else
            {
                return cubicPolynomial(x);
            }
        }
        
    private:
    
        std::array<FloatT, 4> _x;
        std::array<FloatT, 4> _y;
        FloatT prevF;
        bool oversampled = false;
    };
}
