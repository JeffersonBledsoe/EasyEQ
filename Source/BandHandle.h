#pragma once

#include "JuceHeader.h"

//==============================================================================
class BandHandle : public Component,
                   public AudioProcessorValueTreeState::Listener
{
public:
    //==========================================================================
    BandHandle (int bandId, AudioProcessorValueTreeState& state);
    ~BandHandle() = default;
    
    //==============================================================================
    void parameterChanged (const String& parameterID, float newValue) override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    int bandId;
    AudioProcessorValueTreeState& state;
    
    //==========================================================================
    Label bandName;
    
    Slider frequencySlider { Slider::RotaryVerticalDrag, Slider::TextBoxBelow };
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> frequencyAttachment;
    
    Slider gainSlider { Slider::RotaryVerticalDrag, Slider::TextBoxBelow };
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    
    Slider qSlider { Slider::RotaryVerticalDrag, Slider::TextBoxBelow };
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> qAttachment;
    
    ToggleButton bypassButton;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
    
    ComboBox shapeSelector;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> shapeAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandHandle)
};
