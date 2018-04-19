//
//  FFT.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 1/6/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#ifndef FFT_h
#define FFT_h

#include <Accelerate/Accelerate.h>
#include "../core/core.hpp"

namespace moddlib
{
    template <size_t N>
    struct FFT
    {
        FFT() :
            _realBuffer(N/2),
            _imagBuffer(N/2)
        {
            _dspSplitComplex.realp = _realBuffer.data();
            _dspSplitComplex.imagp = _imagBuffer.data();
            _fftSetup = vDSP_create_fftsetup(log2N(), kFFTRadix2);
        }
        
        ~FFT()
        {
            vDSP_destroy_fftsetup(_fftSetup);
        }
    
        void computeFFT(float* inAudioData, float* outFFTData)
        {
            if (inAudioData == NULL || outFFTData == NULL) return;
            
            auto fftLength  = N / 2;
            auto normFactor = 1.f / (2*N);
            
            //Generate a split complex vector from the real data
            vDSP_ctoz((COMPLEX *)inAudioData, 2, &_dspSplitComplex, 1, fftLength);
            
            //Take the fft and scale appropriately
            vDSP_fft_zrip(_fftSetup, &_dspSplitComplex, 1, log2N(), kFFTDirection_Forward);
//            vDSP_vsmul(_dspSplitComplex.realp, 1, &normFactor, _dspSplitComplex.realp, 1, fftLength);
//            vDSP_vsmul(_dspSplitComplex.imagp, 1, &normFactor, _dspSplitComplex.imagp, 1, fftLength);
            
            //Zero out the nyquist value
            _dspSplitComplex.imagp[0] = 0.0;
            
            //Convert the fft data to dB
            vDSP_zvmags(&_dspSplitComplex, 1, outFFTData, 1, fftLength);
            
            const int elements = N/2;
            vvsqrtf(outFFTData, outFFTData, &elements);
        }
        
    private:
    
        constexpr int log2N(int val = N)
        {
            return val == 1 ? 0 : log2N(val / 2) + 1;
        }
        
    private:
        simd::AlignedMemory   _realBuffer;
        simd::AlignedMemory   _imagBuffer;
        FFTSetup        _fftSetup;
        DSPSplitComplex _dspSplitComplex;
    };
}

#endif /* FFT_h */
