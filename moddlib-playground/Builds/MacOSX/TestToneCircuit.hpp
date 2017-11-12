//
//  TestToneGenerator.hpp
//  ModdlibPlayground
//
//  Created by Daniel Doubleday on 11/12/17.
//


struct TestSineSynthCircuit :
    public Circuit<TestSineSynthCircuit, PhaseGenerator, SineOscillator, ADSREnvelopeGenerator>,
    public InputBank<
        Inputs<1, TriggerPort>,
        Inputs<1, PortLink<FloatInput>>>,
    public SingleStreamOutput
{
    using phaseModule = Mod_<0>;
    using oscModule   = Mod_<1>;
    using envModule   = Mod_<2>;

    using triggerIn   = BIn_<0, 0>;
    using frequencyIn = BIn_<1, 0>;

    TestSineSynthCircuit()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // export ports
        
        connect(input<triggerIn>(), moduleIn<phaseModule, PhaseGenerator::triggerIn>());
        connect(input<triggerIn>(), moduleIn<envModule, ADSREnvelopeGenerator::triggerIn>());
        
        input<frequencyIn>().linkTo(moduleIn<phaseModule, PhaseGenerator::freqIn>());

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // connect modules
        
        connect(moduleOut<phaseModule, PhaseGenerator::mainOut>(), moduleIn<oscModule, SineOscillator::phaseIn>());
        connect(moduleOut<oscModule, SineOscillator::mainOut>(), moduleIn<envModule, ADSREnvelopeGenerator::mainIn>());
    }

    float* mainOut()
    {
        auto& env = module<TestSineSynthCircuit::envModule>();
        return env.output<ADSREnvelopeGenerator::mainOut>().buffer().data();
    }
};


