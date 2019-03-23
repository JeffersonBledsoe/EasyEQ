#pragma once

#include "JuceHeader.h"

//==============================================================================
class ControlPanelBand : public Component
{
public:
    //==========================================================================
    ControlPanelBand (const int bandId);
    ~ControlPanelBand() = default;
    ControlPanelBand (ControlPanelBand&& other)
    : bandId (other.bandId)
    {}


    //==========================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==========================================================================
    const int bandId { -1 };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlPanelBand)
};
