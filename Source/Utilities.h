#pragma once

#include "JuceHeader.h"

namespace ParameterNames
{
    static String frequency {"frequency"};
    static String gain {"gain"};
    static String q {"q"};
    static String shape {"shape"};
    static String bypass {"bypass"};
    static String enabled {"enabled"};
}

//==============================================================================
enum FilterShape
{
    Bell = 0,
    LowCut,
    LowShelf,
    HighShelf,
    HighCut,
    Notch,
    BandPass,
    
    numOfShapes
};
