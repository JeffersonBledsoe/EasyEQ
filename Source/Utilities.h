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

//==============================================================================
inline float frequencyToNormalisedFrequency (float value)
{
    return std::log (value / 20.0f) / std::log (20000 / 20);
}

inline float normalisedFrequencyToFrequency (float normalisedValue)
{
    return std::exp (normalisedValue * (std::log (20000 / 20) + std::log (20)));
}
