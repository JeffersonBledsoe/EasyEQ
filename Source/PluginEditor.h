#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "ControlPanel.h"
#include "EasyEqLookAndFeel.h"
#include "HandleEditor.h"

//==============================================================================
class EasyEqAudioProcessorEditor : public AudioProcessorEditor,
                                   public ChangeListener
{
public:
    EasyEqAudioProcessorEditor (EasyEqAudioProcessor& processor,
                                AudioProcessorValueTreeState& state);
    ~EasyEqAudioProcessorEditor();

    //==========================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    //==========================================================================
    EasyEqAudioProcessor& processor;
    AudioProcessorValueTreeState& state;
    EasyEqLookAndFeel laf;
    
    //==========================================================================
    HandleEditor handleEditor;
    ControlPanel controlPanel;
    
    //==========================================================================
    Path frequencyResponsePlotPath, frequencyResponseHitPath;
    void updatePlot (const std::vector<double>& frequencies, const std::vector<double>& magnitudes);
    
    //==========================================================================
    void changeListenerCallback (ChangeBroadcaster* broadcaster) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyEqAudioProcessorEditor)
};
