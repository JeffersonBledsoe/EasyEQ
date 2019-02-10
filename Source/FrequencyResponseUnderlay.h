#pragma once

#include "JuceHeader.h"

//==============================================================================
class FrequencyResponseUnderlay : public Component
{
public:
    //==========================================================================
    FrequencyResponseUnderlay();
    ~FrequencyResponseUnderlay() = default;

    //==========================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyResponseUnderlay)
};
