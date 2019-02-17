#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
//#include "HandleArea.h"
#include "ControlPanel.h"
#include "FrequencyResponseUnderlay.h"
#include "BandHandle.h"

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
    void resized() override;
    
    //==========================================================================
    void parameterChanged (const String& parameterId, float newValue) override;

private:
    //==========================================================================
    EasyEqAudioProcessor& processor;
    AudioProcessorValueTreeState& state;
    
    //==========================================================================
    int selectedBandId {-1};
    
    //==========================================================================
    FrequencyResponseUnderlay frequencyResponse;
//    HandleArea handleControl;
    OwnedArray<BandHandle> handles;
    ControlPanel controlPanel;
    
    //==========================================================================
    void changeListenerCallback (ChangeBroadcaster* broadcaster) override;
    void mouseDown (const MouseEvent& event) override;
    void mouseDoubleClick (const MouseEvent& event) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyEqAudioProcessorEditor)
};
