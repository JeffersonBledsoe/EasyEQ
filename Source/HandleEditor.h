#pragma once

#include "JuceHeader.h"
#include "BandHandle.h"

//==============================================================================
class HandleEditor : public Component,
                     public AudioProcessorValueTreeState::Listener
{
public:
    //==========================================================================
    HandleEditor (AudioProcessorValueTreeState& state);
    ~HandleEditor();
    
    //==========================================================================
    void parameterChanged (const String& parameterId, float newValue) override;

    //==========================================================================
    void paint (Graphics&) override;

private:
    //==========================================================================
    AudioProcessorValueTreeState& state;
    
    //==========================================================================
    OwnedArray<BandHandle> handles;
    void updateHandle (int bandId, bool shouldAdd);
    
    //==========================================================================
    Point<float> mouseDownPosition;
    void mouseDown (const MouseEvent& event) override;
    void mouseDoubleClick (const MouseEvent& event) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HandleEditor)
};
