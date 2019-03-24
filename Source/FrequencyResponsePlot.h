#pragma once

#include "JuceHeader.h"

//==============================================================================
class FrequencyResponsePlot : public Component
{
public:
    //==========================================================================
    FrequencyResponsePlot();
    ~FrequencyResponsePlot() = default;
    
    //==========================================================================
    void updatePlot (const std::vector<double>& frequencies, const std::vector<double>& magnitudes);
    void paint (Graphics&) override;
    bool hitTest (int x, int y) override;

private:
    //==========================================================================
    Path frequencyResponsePlotPath, frequencyResponseHitPath;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyResponsePlot)
};
