#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "EasyEqLookAndFeel.h"
#include "ControlPanel.h"
#include "HandleEditor.h"
#include "FrequencyResponsePlot.h"

//==============================================================================
class EasyEqAudioProcessorEditor : public AudioProcessorEditor,
                                   public ChangeListener
{
public:
    EasyEqAudioProcessorEditor (EasyEqAudioProcessor& processor,
                                AudioProcessorValueTreeState& state);
    ~EasyEqAudioProcessorEditor();

    //==========================================================================
    void resized() override;

private:
    //==========================================================================
    EasyEqAudioProcessor& processor;
    AudioProcessorValueTreeState& state;
    EasyEqLookAndFeel laf;
    
    //==========================================================================
    HandleEditor handleEditor;
    FrequencyResponsePlot frequencyResponsePlot;
    ControlPanel controlPanel;
    
    //==========================================================================
    void changeListenerCallback (ChangeBroadcaster* broadcaster) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyEqAudioProcessorEditor)
};
