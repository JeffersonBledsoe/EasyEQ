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
    void updatePlot (const std::vector<double>& frequencies, const std::vector<double>& magnitudes);

    //==========================================================================
    void paint (Graphics&) override;

private:
    //==========================================================================
    Path plotPath;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyResponseUnderlay)
};
