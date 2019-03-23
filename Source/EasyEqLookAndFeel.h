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
    void drawLinearSlider (Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle, Slider&) override;
    void drawRotarySlider (Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, Slider&) override;
    
    //==========================================================================
    void drawComboBox (Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       ComboBox&) override;
    
private:
    //==========================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyEqLookAndFeel)
};
