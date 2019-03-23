#pragma once

#include "JuceHeader.h"
#include "ControlPanelBand.h"

//==============================================================================
class ControlPanel : public Component
{
public:
    //==========================================================================
    ControlPanel (AudioProcessorValueTreeState& state);
    ~ControlPanel() = default;
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==========================================================================
    AudioProcessorValueTreeState& state;
    std::array<ControlPanelBand, 8> bands;
    int currentBandId {-1};
    
    //==========================================================================
    AudioParameterChoice* shapeParam;
    void update();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlPanel)
};
