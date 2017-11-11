//
//  Engine.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 21/11/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>

#include "core.hpp"
#include "fmt/format.cc"

moddlib::SampleBuffers moddlib::SampleBuffers::_instance;
moddlib::FloatOutput   moddlib::FlatOutput::_output;
moddlib::StreamOutput  moddlib::FlatOutput::_stream;
