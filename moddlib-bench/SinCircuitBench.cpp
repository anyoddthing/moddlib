//
//  SinCircuitBench.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 1/10/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#include <thread>
#include <chrono>
#include "bench.hpp"
#include "instruments/SineSynth.hpp"
#include "SinCircuitBaseLine.hpp"

using namespace moddlib;

struct TestToneGenerator :
    public Generator<TestToneGenerator>,
    public SingleStreamOutput
{
    TestToneGenerator() : _phase(0), _buffer(64)
    {}
    
    void inputsPrepare() {}
    void inputsCleanup() {}
    void inputsInit() {}
    
    void doGenerate()
    {
        output<mainOut>().buffer().copy(_buffer.data(_phase));
        _phase = (_phase + 8) % _buffer.size();
    }
    
private:
    size_t _phase;
    TestToneBuffer _buffer;
};

struct TestToneCircuit :
    public Circuit<TestToneCircuit, TestToneGenerator, ADSREnvelopeGenerator>,
    public InputBank<
        Inputs<1, TriggerPort>>,
    public SingleStreamOutput
{
    using oscModule   = Mod_<0>;
    using envModule   = Mod_<1>;

    using triggerIn   = BIn_<0, 0>;

    TestToneCircuit()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // export ports
        
        connect(input<triggerIn>(), moduleIn<envModule, ADSREnvelopeGenerator::triggerIn>());
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // connect modules
        
        connect(moduleOut<oscModule, TestToneGenerator::mainOut>(), moduleIn<envModule, ADSREnvelopeGenerator::mainIn>());
    }
    
    float* mainOut()
    {
        auto& env = module<TestToneCircuit::envModule>();
        return env.output<ADSREnvelopeGenerator::mainOut>().buffer().data();
    }
};

struct SinCircuitBench
{
    static SinCircuitBench instance;
    static constexpr int size = 1024 * 1;
    static constexpr int hold = size * 0.2;
    
    SineSynthCircuit circuit;
    
    AlignedMemory mem;
    
    FloatOutput _trigger;
    FloatOutput _frequency;
    
    SinCircuitBench() : mem(size)
    {
        connect(_trigger, circuit.input<SineSynthCircuit::triggerIn>());
        connect(_frequency, circuit.input<SineSynthCircuit::frequencyIn>());
    }
    
    void trigger(float herz)
    {
        _trigger.up();
        _frequency.setValue(herz);
    }
    
    NOINLINE void test(benchpress::context* ctx)
    {
        fref circuitOut = circuit.moduleOut<SineSynthCircuit::envModule, ADSREnvelopeGenerator::mainOut>();
        trigger(440);
        for (size_t i = 0; i < size / 2; i += 8)
        {
            circuit.generate();
            simdv::memcopy(mem.data() + i, circuitOut);
        }
        _trigger.down();
        for (size_t i = size / 2; i < size; i += 8)
        {
            circuit.generate();
            simdv::memcopy(mem.data() + i, circuitOut);
        }
    }
};

struct TestToneCircuitBench
{
    static TestToneCircuitBench instance;
    static constexpr int size = 1024 * 1;
    static constexpr int hold = size * 0.2;
    
    TestToneCircuit circuit;
    
    AlignedMemory mem;
    
    FloatOutput _trigger;
    
    TestToneCircuitBench() : mem(size)
    {
        connect(_trigger, circuit.input<SineSynthCircuit::triggerIn>());
    }
    
    void trigger(float herz)
    {
        _trigger.up();
    }
    
    NOINLINE void test(benchpress::context* ctx)
    {
        auto circuitOut = circuit.mainOut();
        trigger(440);
        for (size_t i = 0; i < size / 2; i += 8)
        {
            circuit.generate();
            simdv::memcopy(mem.data() + i, circuitOut);
        }
        _trigger.down();
        for (size_t i = size / 2; i < size; i += 8)
        {
            circuit.generate();
            simdv::memcopy(mem.data() + i, circuitOut);
        }
    }
};

struct OldSchoolSinTest
{
    static OldSchoolSinTest instance;
    static constexpr int size = 1024 * 1;
    static constexpr int hold = size * 0.2;
    
    BLSineCircuit circuit;
    
    AlignedMemory mem;
    
    BLFloatOutput _trigger;
    BLFloatOutput _frequency;
    
    OldSchoolSinTest() : mem(size)
    {
    }
    
    void trigger(float herz)
    {
        circuit.triggerIn.up();
        circuit.freqIn.setValue(herz);
    }
    
    NOINLINE void test(benchpress::context* ctx)
    {
        auto circuitOut = circuit.mainOutput().data();
        trigger(440);
        for (size_t i = 0; i < size / 2; i += 8)
        {
            circuit.generate();
            simdv::memcopy(mem.data() + i, circuitOut);
        }
        circuit.triggerIn.down();
        for (size_t i = size / 2; i < size; i += 8)
        {
            circuit.generate();
            simdv::memcopy(mem.data() + i, circuitOut);
        }
    }
};

struct OldSchoolSinTest2
{
    static OldSchoolSinTest2 instance;
    static constexpr int size = 1024 * 1;
    static constexpr int hold = size * 0.2;
    
    BLCustomSineCircuit circuit;
    
    AlignedMemory mem;
    
    BLFloatOutput _trigger;
    BLFloatOutput _frequency;
    
    OldSchoolSinTest2() : mem(size)
    {
    }
    
    void trigger(float herz)
    {
        circuit.triggerIn.up();
        circuit.freqIn.setValue(herz);
    }
    
    NOINLINE void test(benchpress::context* ctx)
    {
        auto circuitOut = circuit.mainOutput().data();
        trigger(440);
        for (size_t i = 0; i < size / 2; i += 8)
        {
            circuit.generate();
            simdv::memcopy(mem.data() + i, circuitOut);
        }
        circuit.triggerIn.down();
        for (size_t i = size / 2; i < size; i += 8)
        {
            circuit.generate();
            simdv::memcopy(mem.data() + i, circuitOut);
        }
    }
};

struct OldSchoolTestToneCircuitBench
{
    static OldSchoolTestToneCircuitBench instance;
    static constexpr int size = 1024 * 1;
    static constexpr int hold = size * 0.2;
    
    BLCustomSineCircuit circuit;
    
    AlignedMemory mem;
    
    OldSchoolTestToneCircuitBench() : mem(size)
    {
    }
    
    void trigger(float herz)
    {
        circuit.triggerIn.up();
    }
    
    NOINLINE void test(benchpress::context* ctx)
    {
        auto circuitOut = circuit.mainOutput().data();
        trigger(440);
        for (size_t i = 0; i < size / 2; i += 8)
        {
            circuit.generate();
            simdv::memcopy(mem.data() + i, circuitOut);
        }
        circuit.triggerIn.down();
        for (size_t i = size / 2; i < size; i += 8)
        {
            circuit.generate();
            simdv::memcopy(mem.data() + i, circuitOut);
        }
    }
};

SinCircuitBench SinCircuitBench::instance;
TestToneCircuitBench TestToneCircuitBench::instance;
OldSchoolSinTest OldSchoolSinTest::instance;
OldSchoolSinTest2 OldSchoolSinTest2::instance;
OldSchoolTestToneCircuitBench OldSchoolTestToneCircuitBench::instance;

BENCHMARK("SinCircuit Template", [](benchpress::context* ctx)
{
    BENCH_ITER SinCircuitBench::instance.test(ctx);
})

BENCHMARK("SinCircuit OldSchool", [](benchpress::context* ctx)
{
    BENCH_ITER OldSchoolSinTest::instance.test(ctx);
})

BENCHMARK("SinCircuit OldSchool2", [](benchpress::context* ctx)
{
    BENCH_ITER OldSchoolSinTest2::instance.test(ctx);
})

BENCHMARK("TestTone Template", [](benchpress::context* ctx)
{
    BENCH_ITER TestToneCircuitBench::instance.test(ctx);
})

BENCHMARK("TestTone OldSchool", [](benchpress::context* ctx)
{
    BENCH_ITER OldSchoolTestToneCircuitBench::instance.test(ctx);
})











