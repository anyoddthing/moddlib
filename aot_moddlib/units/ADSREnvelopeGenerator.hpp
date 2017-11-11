//
//  EnvelopeGenerator.h
//  unitsynth
//
//  Created by Daniel Doubleday on 1/17/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#ifndef ADSREnvelopeGenerator_h
#define ADSREnvelopeGenerator_h

#include "../core/core.hpp"
#include "../util/Envelope.hpp"

namespace moddlib
{
    struct ADSREnvelope
        : FixedSegmentEnvelope<4>
    {
        ADSREnvelope()
        {
            setSegment<EnvelopeSegLog>(0);
            setSegment<EnvelopeSegExp>(1);
            setSegment<EnvelopeSegSustain>(2);
            setSegment<EnvelopeSegExp>(3);
            
            _sustain = 1;
            
            attackSeg().setup(time2Samples(0.01f), 0, 1, 0.05f);
            decaySeg().setup(time2Samples(0.1f), 1, _sustain, 0.1f);
            
            sustainSeg().setup(8, _sustain);
            releaseSeg().setup(time2Samples(0.1f), _sustain, 0, 0.1f);
            
            setLoop(2, 3);
        }
        
        void setAttack(float attack)
        {
            attackSeg().setLength(time2Samples(attack));
        }
        
        void setDecay(float decay)
        {
            decaySeg().setLength(time2Samples(decay));
        }
        
        void setSustain(float sustain)
        {
            if (_sustain != sustain)
            {
                decaySeg().setEnd(sustain);
                sustainSeg().setStart(sustain);
                releaseSeg().setStart(sustain);
                _sustain = sustain;
            }
        }
        
        void setRelease(float release)
        {
            releaseSeg().setLength(time2Samples(release));
        }
        
        void setAttackBias(float bias)
        {
            attackSeg().setBias(bias);
        }
        
        void setDecayBias(float bias)
        {
            decaySeg().setBias(bias);
        }
        
        void setReleaseBias(float bias)
        {
            releaseSeg().setBias(bias);
        }
        
        EnvelopeSegLog& attackSeg()
        {
            return getSegment<EnvelopeSegLog>(0);
        }
        
        EnvelopeSegExp& decaySeg()
        {
            return getSegment<EnvelopeSegExp>(1);
        }
        
        EnvelopeSegSustain& sustainSeg()
        {
            return getSegment<EnvelopeSegSustain>(2);
        }
        
        EnvelopeSegExp& releaseSeg()
        {
            return getSegment<EnvelopeSegExp>(3);
        }
        
    private:
    
        float _sustain;
    };
    
    struct ADSREnvelopeGenerator :
        Generator<ADSREnvelopeGenerator>,
        InputBank<
            Inputs<1, StreamInput>,
            Inputs<1, TriggerInput>,
            Inputs<5, DefaultInput>>,
        OutputBank<
            Outputs<2, StreamOutput>>
    {
        using triggerIn = BIn_<1, 0>;

        using attackIn     = BIn_<2, 0>;
        using decayIn      = BIn_<2, 1>;
        using decayLevelIn = BIn_<2, 2>;
        using sustainIn    = BIn_<2, 3>;
        using releaseIn    = BIn_<2, 4>;

        using envOut    = BOut_<0, 1>;

        ADSREnvelopeGenerator()
        {
            input<attackIn>().setup(0.01f);
            input<decayIn>().setup(0.1f);
            input<sustainIn>().setup(0.7f);
            input<releaseIn>().setup(2.5f);
        }
        
        void doGenerate()
        {
            auto triggerLast = input<triggerIn>().getLastValue();
            auto triggerNow = input<triggerIn>().getValue();
            if (triggerLast != triggerNow)
            {
                LOG("envelope trigger changed: from {} to {}", triggerLast, triggerNow);
            }
            
            if (input<triggerIn>().isTriggered())
            {
                _envelope.setAttack(input<attackIn>());
                _envelope.setDecay(input<decayIn>());
                _envelope.setSustain(input<sustainIn>());
                _envelope.setRelease(input<releaseIn>());
                
                _envelope.reset();
            }
            
            _envelope.generate(output<envOut>().buffer().size(), output<envOut>(), input<triggerIn>().isUp());
            
            if (input<mainIn>().isConnected())
            {
                simdv::mpy(output<mainOut>(), input<mainIn>(), output<envOut>());
            }
        }
        
        bool isPlaying()
        {
            return _envelope.isPlaying();
        }
        
//        ADSREnvelope& getEnvelope()
//        {
//            return _envelope;
//        }
        
    private:
        
        ADSREnvelope _envelope;
        
    };
}

#endif /* ADSREnvelopeGenerator_h */
