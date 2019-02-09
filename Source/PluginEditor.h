#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
class EasyEqAudioProcessorEditor : public AudioProcessorEditor,
                                   public AudioProcessorValueTreeState::Listener
{
public:
    EasyEqAudioProcessorEditor (EasyEqAudioProcessor& processor,
                                AudioProcessorValueTreeState& state);
    ~EasyEqAudioProcessorEditor() = default;
    
    //==============================================================================
    void parameterChanged (const String& parameterID, float newValue) override;

    //==========================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==========================================================================
    EasyEqAudioProcessor& processor;
    AudioProcessorValueTreeState& state;
    
    //==========================================================================
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyEqAudioProcessorEditor)
};
