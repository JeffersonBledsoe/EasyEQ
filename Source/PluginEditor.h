#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "HandleArea.h"
#include "ControlPanel.h"

//==============================================================================
class EasyEqAudioProcessorEditor : public AudioProcessorEditor,
                                   public ChangeListener
{
public:
    EasyEqAudioProcessorEditor (EasyEqAudioProcessor& processor,
                                AudioProcessorValueTreeState& state);
    ~EasyEqAudioProcessorEditor() = default;

    //==========================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==========================================================================
    AudioProcessorValueTreeState& state;
    
    //==========================================================================
    HandleArea handleArea;
    ControlPanel controlPanel;
    
    //==========================================================================
    void changeListenerCallback (ChangeBroadcaster* broadcaster) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyEqAudioProcessorEditor)
};
