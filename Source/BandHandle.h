#pragma once

#include "JuceHeader.h"

//==============================================================================
class BandHandle : public Component
{
public:
    //==========================================================================
    BandHandle();
    ~BandHandle() = default;

    //==============================================================================
    void paint (Graphics&) override;

private:
    //==============================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandHandle)
};
