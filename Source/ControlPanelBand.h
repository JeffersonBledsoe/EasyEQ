#pragma once

#include "JuceHeader.h"

//==============================================================================
class ControlPanelBand : public Component,
                         public AudioProcessorValueTreeState::Listener
{
public:
    //==========================================================================
    ControlPanelBand (const int bandId, AudioProcessorValueTreeState& state);
    ~ControlPanelBand();
    ControlPanelBand (ControlPanelBand&& other)
    : bandId (other.bandId), state (other.state)
    {}

    //==========================================================================
    void resized() override;
    
    //==============================================================================
    void parameterChanged (const String& parameterID, float newValue) override;

private:
    //==========================================================================
    const int bandId { -1 };
    AudioProcessorValueTreeState& state;
    
    //==========================================================================
    AudioParameterChoice* shapeParam;
    AudioParameterBool* bypassParam;
    AudioParameterBool* enabledParam;
    void update();
    
    //==========================================================================
    Label bandName;
    Slider frequencySlider { Slider::LinearBarVertical, Slider::NoTextBox };
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> frequencyAttachment;
    Slider gainSlider { Slider::LinearBarVertical, Slider::NoTextBox };
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    Slider qSlider { Slider::LinearBarVertical, Slider::NoTextBox };
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> qAttachment;
    ToggleButton bypassButton;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
    ComboBox shapeSelector;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> shapeAttachment;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlPanelBand)
};
