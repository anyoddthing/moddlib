//
//  Engine.cpp
//  unitsynth
//
//  Created by Daniel Doubleday on 21/11/15.
//  Copyright © 2015 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>

#include "core.hpp"
#include "../../libs/fmtlib/fmt/format.cc"

moddlib::Engine         moddlib::Engine::_instance;
moddlib::SampleBuffers  moddlib::SampleBuffers::_instance;
moddlib::NoInput        moddlib::NoInput::instance;
moddlib::FloatOutput    moddlib::FlatOutput::_output;
moddlib::StreamOutput   moddlib::FlatOutput::_stream;
