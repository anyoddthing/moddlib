/*
  ==============================================================================

    Types.h
    Created: 27 Jun 2015 10:39:40am
    Author:  Daniel Doubleday

  ==============================================================================
*/

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <functional>

#include "aot_simdlib/aot_simdlib.hpp"

#define fref auto&&

using uint = unsigned int;
using uchar = unsigned char;

using Action = std::function<void()>;
using simdv = simd::recipes<8>;

//namespace std
//{
//    template< class... >
//    using void_t = void;
//}

namespace moddlib
{

    class NonCopyable
    {
    public:
        NonCopyable() = default;
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
        
        NonCopyable(NonCopyable&&) noexcept;
        NonCopyable& operator=(NonCopyable&&);
    };

    inline NonCopyable::NonCopyable(NonCopyable&&) noexcept = default;
    inline NonCopyable& NonCopyable::operator=(NonCopyable&&) = default;
}

#endif  // TYPES_H_INCLUDED
