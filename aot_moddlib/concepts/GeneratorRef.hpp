//
//  GeneratorRef.hpp
//  moddlib
//
//  Created by Daniel Doubleday on 1/3/18.
//  Copyright © 2018 Daniel Doubleday. All rights reserved.
//

#pragma once

namespace moddlib
{
    class GeneratorRef
    {
    public:
        template <typename T>
        GeneratorRef(T&& val, std::enable_if_t<!std::is_same<T, GeneratorRef&>::value>* = 0) : _data(std::make_shared<Model<std::remove_reference_t<T>>>(std::forward<T>(val))) {}
        
        template <typename T>
        GeneratorRef(std::reference_wrapper<T> val) : _data(std::make_shared<Model<T&>>(val.get())) {}
        
        void init()
        {
            _data->init();
        }
        
        void generate()
        {
            _data->generate();
        }
        
        SampleBuffer& output(uint i, uint j)
        {
            return _data->output(i, j);
        }
        
    private:
        struct Concept
        {
            virtual ~Concept() {}
            virtual void init() = 0;
            virtual void generate() = 0;
            virtual SampleBuffer& output(uint i, uint j) = 0;
        };
        
        template <typename T>
        struct Model : Concept
        {
            Model(T&& val) : _data(std::forward<T>(val)) {}
            
            void init() override
            {
                _data.init();
            }
            
            void generate() override
            {
                _data.generate();
            }
            
            SampleBuffer& output(uint i, uint j) override
            {
                return _data.output(i, j);
            }
            
        private:
            T _data;
        };
        
        std::shared_ptr<Concept> _data;
    };
}

