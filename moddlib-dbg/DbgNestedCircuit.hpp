//
//  DbgNestedCircuit.hpp
//  moddlib
//
//  Created by Daniel Doubleday on 02.05.18.
//  Copyright © 2018 Daniel Doubleday. All rights reserved.
//

#pragma once


struct SimpleGenerator :
    Generator<SimpleGenerator>,
    InputBank<Inputs<1, FloatInput>>,
    SingleStreamOutput
{
    using freqIn = BIn_<0, 0>;
    
    void doGenerate()
    {
        float value = input<freqIn>().getValue();
        auto& out   = output<mainOut>().buffer();
        out.fill(value);
    }
};

struct ValueForwarder :
    Generator<ValueForwarder>,
    InputBank<Inputs<1, FloatInput>>,
    OutputBank<Outputs<1, FloatOutput>>
{
    using valIn = BIn_<0, 0>;
    using valOut = BOut_<0, 0>;
    
    void doGenerate()
    {
        float value = input<valIn>().getValue();
        auto& out   = output<valOut>();
        out.setValue(value);
    }
};

struct ChildCircuit :
    Circuit<ChildCircuit, ValueForwarder, SimpleGenerator>
{
    using forwarderModule = Mod_<0>;
    using generatorModule = Mod_<1>;
    
    using freqIn = Sel_<generatorModule, SimpleGenerator::freqIn>;
    
    ChildCircuit()
    {
        connect(
            moduleOut<forwarderModule, ValueForwarder::valOut>(),
            moduleIn<generatorModule, SimpleGenerator::freqIn>()
        );
    }
};

struct MasterCircuit :
    Circuit<MasterCircuit, ChildCircuit>,
    InputBank<Inputs<1, FloatPort>>
{
    using freqIn = Sel_<Mod_<1>, ChildCircuit::freqIn>;
    
    MasterCircuit()
    {
        connect(
            input<BIn_<0,0>>(),
            moduleIn<Mod_<0>, ChildCircuit::forwarderModule, ValueForwarder::valIn>()
        );
    }
};

void testNestedCircuit()
{
    using sel = Mod_<1>;
    static_assert(IsModuleSelector<sel>::value, "modselector");
}






