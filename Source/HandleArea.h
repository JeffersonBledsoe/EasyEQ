#pragma once

#include "JuceHeader.h"
#include "BandHandle.h"

//==============================================================================
class HandleArea : public Component,
                   public AudioProcessorValueTreeState::Listener,
                   public ChangeBroadcaster
{
public:
    //==========================================================================
    HandleArea ( AudioProcessorValueTreeState& s);
    ~HandleArea();

    //==========================================================================
    void parameterChanged (const String& parameterId, float newValue) override;
    
    //==========================================================================
    int selectedBandId {-1};

private:
    //==========================================================================
    AudioProcessorValueTreeState& state;
    OwnedArray<BandHandle> handles;
    
    //==========================================================================
    void mouseDown (const MouseEvent& event) override;
    void mouseDoubleClick (const MouseEvent& event) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HandleArea)
};
