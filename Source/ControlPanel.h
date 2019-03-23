#pragma once

#include "JuceHeader.h"
#include "ControlPanelBand.h"

//==============================================================================
class ControlPanel : public Component,
                     public AudioProcessorValueTreeState::Listener
{
public:
    //==========================================================================
    ControlPanel (AudioProcessorValueTreeState& s);
    ~ControlPanel();
    
    //==============================================================================
    void setSelectedBand (int bandId);
    
    //==============================================================================
    void parameterChanged (const String& parameterID, float newValue) override;
    void paint (Graphics&) override;
    void resized() override;

private:
    //==========================================================================
    AudioProcessorValueTreeState& state;
    std::array<ControlPanelBand, 8> bands;
    int currentBandId {-1};
    
    //==========================================================================
    AudioParameterChoice* shapeParam;
    void update();
    
    //==========================================================================
    Label bandName;
    Slider frequencySlider { Slider::RotaryVerticalDrag, Slider::NoTextBox };
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> frequencyAttachment;
    Slider gainSlider { Slider::RotaryVerticalDrag, Slider::NoTextBox };
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    Slider qSlider { Slider::RotaryVerticalDrag, Slider::NoTextBox };
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> qAttachment;
    ToggleButton bypassButton;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
    ComboBox shapeSelector;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> shapeAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlPanel)
};
