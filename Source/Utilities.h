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

namespace BandColours
{
    inline Colour getColourForBand (const int bandId) noexcept
    {
        if (bandId == 0)              return Colour::fromRGB (222, 75, 248);
        else if (bandId == 1)         return Colour::fromRGB (241, 228, 166);
        else if (bandId == 2)         return Colour::fromRGB (145, 227, 162);
        else if (bandId == 3)         return Colour::fromRGB (107, 197, 250);
        else if (bandId == 4)         return Colour::fromRGB (162, 115, 227);
        else if (bandId == 5)         return Colour::fromRGB (245, 133, 184);
        else if (bandId == 6)         return Colour::fromRGB (245, 87, 84);
        else if (bandId == 7)         return Colour::fromRGB (21, 148, 135);
        else
        {
            jassertfalse;
            return {};
        }
    }
}

//==============================================================================
namespace FilterShapes
{
    enum Shape
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
    
    constexpr auto getFilterShapeNameForId (const int id)
    {
        switch (id)
        {
            case FilterShapes::Bell:            return "Bell";
            case FilterShapes::LowCut:          return "Low Cut";
            case FilterShapes::LowShelf:        return "Low Shelf";
            case FilterShapes::HighShelf:       return "High Shelf";
            case FilterShapes::HighCut:         return "High Cut";
            case FilterShapes::Notch:           return "Notch";
            case FilterShapes::BandPass:        return "Band Pass";
            default: break;
        }
        
        return "";
    }
}

//==============================================================================
inline float valueToNormalisedFrequency (float value, float min = 20.0f, float max = 20000.0f)
{
    return std::logf (value / min) / std::logf (max / min);
}

inline float normalisedValueToFrequency (float normalisedValue, float min = 20.0f, float max = 20000.0f)
{
    return min * std::expf (normalisedValue * std::logf (max / min));
}
