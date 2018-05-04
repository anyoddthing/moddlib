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
    MasterCircuit()
    {
        connect(
            input<BIn_<0,0>>(),
            moduleIn<Mod_<0>, ChildCircuit::forwarderModule, ValueForwarder::valIn>()
        );
    }
};

struct Unpacker
{
    template <typename SelT> static std::enable_if_t<
        std::is_same<typename SelT::tail, std::nullptr_t>::value,
    void> unpack()
    {
        std::cout << "last: " << SelT::head::bank << std::endl;
    }
    
    template <typename SelT> static std::enable_if_t<
        !std::is_same<typename SelT::tail, std::nullptr_t>::value,
    void> unpack()
    {
        std::cout << "elem: " << SelT::head::bank << std::endl;
        unpack<typename SelT::tail>();
    }
};


void testNestedCircuit()
{
//    using sel = Sel_<Bank_<0>, Bank_<1>, Bank_<12>>;
//    Unpacker::unpack<sel>();
    using sel = Mod_<1>;
    static_assert(IsModuleSelector<sel>::value, "modselector");
//    static_assert(IsModuleSelector<Bank_<0>>::value, "modselector");
}






