//
//  main.cpp
//  moddlib-dbg
//
//  Created by Daniel Doubleday on 12/17/17.
//  Copyright © 2017 Daniel Doubleday. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "aot_moddlib.hpp"
#include "util/DebugDumpHost.hpp"

#include "DbgWaveTable.hpp"
#include "DbgCreateSine.hpp"



//==============================================================================



int main()
{
    createSine();
//    Engine::instance().setFrameRate(Engine::DEFAULT_FRAMERATE * 2);
//    oversamplingTest();
//    testWaveTable2D<IdentityPartialScaler>("identity1");
//    testWaveTable2D<HammingPartialScaler>("hamming1");
//    testWaveTable2D<HannPartialScaler>("hann1");
    
//    testWaveTable2D<IdentityPartialScaler>("identity2", 2);
//    testWaveTable2D<HammingPartialScaler>("hamming2", 2);
//    testWaveTable2D<HannPartialScaler>("hann2", 2);

    return 0;
}
