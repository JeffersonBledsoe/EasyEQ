#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "HandleArea.h"
#include "ControlPanel.h"
#include "FrequencyResponseUnderlay.h"

//==============================================================================
class EasyEqAudioProcessorEditor : public AudioProcessorEditor,
                                   public ChangeListener
{
public:
    EasyEqAudioProcessorEditor (EasyEqAudioProcessor& processor,
                                AudioProcessorValueTreeState& state);
    ~EasyEqAudioProcessorEditor() = default;

    //==========================================================================
    void resized() override;

private:
    //==========================================================================
    AudioProcessorValueTreeState& state;
    
    //==========================================================================
    FrequencyResponseUnderlay frequencyResponse;
    HandleArea handleControl;
    ControlPanel controlPanel;
    
    //==========================================================================
    void changeListenerCallback (ChangeBroadcaster* broadcaster) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyEqAudioProcessorEditor)
};
