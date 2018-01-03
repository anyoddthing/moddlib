//
//  debug.hpp
//  unitsynth
//
//  Created by Daniel Doubleday on 12/18/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#ifndef debug_h
#define debug_h

#include <string.h>
#include <fstream>

#include "../../libs/fmtlib/fmt/format.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef DEBUG
#define LOG(format, ...) dbg::log(__FILENAME__, __LINE__, format, ##__VA_ARGS__);
#else
#define LOG(format, ...) do {} while (0)
#endif

namespace dbg
{
    template <typename... ArgsT>
    void log(char const * file, int line, char const * format, ArgsT&& ... args)
    {
        fmt::print("{:<30}", fmt::format("[{}:{}] ", file, line));
        fmt::print(format, std::forward<ArgsT>(args)...);
        fmt::print("\n");
    }
    
    static void dumpBuffer(std::string const & filename, char const * source, size_t num)
    {
        std::ofstream outfile(filename, std::ios::out | std::ios::binary);
        outfile.write(source, num);
    }
    
    static void dumpBuffer(std::string const & filename, float const * source, size_t num)
    {
        dumpBuffer(filename, reinterpret_cast<char const*>(source), num * sizeof(float));
    }
}

#endif /* debug_h */
