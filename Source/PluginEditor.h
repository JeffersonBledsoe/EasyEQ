#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "ControlPanel.h"
#include "BandHandle.h"
#include "EasyEqLookAndFeel.h"

//==============================================================================
class EasyEqAudioProcessorEditor : public AudioProcessorEditor,
                                   public AudioProcessorValueTreeState::Listener,
                                   public ChangeListener
{
public:
    EasyEqAudioProcessorEditor (EasyEqAudioProcessor& processor,
                                AudioProcessorValueTreeState& state);
    ~EasyEqAudioProcessorEditor();

    //==========================================================================
    void paint (Graphics& g) override;
    
    //==========================================================================
    void parameterChanged (const String& parameterId, float newValue) override;

private:
    //==========================================================================
    EasyEqAudioProcessor& processor;
    AudioProcessorValueTreeState& state;
    EasyEqLookAndFeel laf;
    
    //==========================================================================
    OwnedArray<BandHandle> handles;
    BandHandle* selectedHandle = nullptr;
    
    //==========================================================================
    Path frequencyResponsePlotPath, frequencyResponseHitPath;
    void updatePlot (const std::vector<double>& frequencies, const std::vector<double>& magnitudes);
    Point<float> mouseDownPosition;
    
    //==========================================================================
    ControlPanel controlPanel;
    void updateControlPanelPosition (const Point<float> position);
    
    //==========================================================================
    void changeListenerCallback (ChangeBroadcaster* broadcaster) override;
    void mouseDown (const MouseEvent& event) override;
    void mouseDoubleClick (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyEqAudioProcessorEditor)
};
