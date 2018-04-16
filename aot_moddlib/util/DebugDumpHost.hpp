//
//  DebugDumpHost.h
//  moddlib
//
//  Created by Daniel Doubleday on 12/17/17.
//  Copyright © 2017 Daniel Doubleday. All rights reserved.
//

#pragma once

#include "../concepts/GeneratorRef.hpp"


namespace moddlib
{
    struct DebugDumpHost
    {
        DebugDumpHost(const std::string& path, GeneratorRef generator) : _generator(generator)
        {
            _dbgStream = std::make_unique<std::ofstream>(path, std::ios::binary | std::ios::out);
        }
        
        void init()
        {
            _generator.init();
        }
        
        void generate(uint numSamples)
        {
            auto& out = _generator.output(0, 0);
            
            // too lazy to support odd sample counts here
            assert(numSamples % out.size() == 0);
            
            for (auto i = 0; i < numSamples; i += out.size())
            {
                _generator.generate();
                _dbgStream->write(reinterpret_cast<const char*>(out.data()), out.size() * sizeof(float));
            }
        }
    
    private:
        std::unique_ptr<std::ofstream> _dbgStream;
        GeneratorRef _generator;
    };
}

