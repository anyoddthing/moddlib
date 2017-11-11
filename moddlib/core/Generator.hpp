//
//  Generator.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 1/4/16.
//  Copyright © 2016 Daniel Doubleday. All rights reserved.
//

#ifndef Generator_h
#define Generator_h

namespace moddlib
{
    /** CRTP type for all generators
     *
     * \param GenT  type implementing the generator
     */
    template <typename GenT>
    struct Generator : NonCopyable
    {
        void init()
        {
            initImpl(0);
            thiz()->inputsInit();
        }

        void generate()
        {
            thiz()->inputsPrepare();
            thiz()->doGenerate();
            thiz()->inputsCleanup();
        }

    private:

        GenT* thiz()
        {
            return static_cast<GenT*>(this);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // optional doInit

        template <typename T = GenT>
        void initImpl(long) { }

        template <typename T = GenT>
        auto initImpl(int)
            -> decltype(std::declval<T>().doInit(), void())
        {
            thiz()->doInit();
        }

    };
    
}

#endif /* Generator_h */
