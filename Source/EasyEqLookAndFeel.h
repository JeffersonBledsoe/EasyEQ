#pragma once

#include "JuceHeader.h"

//==============================================================================
class EasyEqLookAndFeel : public LookAndFeel_V4
{
public:
    //==========================================================================
    EasyEqLookAndFeel();
    ~EasyEqLookAndFeel() = default;
    
    //==========================================================================
    void drawRotarySlider (Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, Slider&) override;
    
private:
    //==========================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyEqLookAndFeel)
};
