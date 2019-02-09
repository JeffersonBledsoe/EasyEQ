#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "BandHandle.h"

//==============================================================================
class EasyEqAudioProcessorEditor : public AudioProcessorEditor
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
    EasyEqAudioProcessor& processor;
    AudioProcessorValueTreeState& state;
    
    //==========================================================================
    OwnedArray<BandHandle> handles;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyEqAudioProcessorEditor)
};
