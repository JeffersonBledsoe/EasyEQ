#include "JuceHeader.h"
#include "BandHandle.h"

//==============================================================================
BandHandle::BandHandle()
{
}

//==============================================================================
void BandHandle::paint (Graphics& g)
{
    g.setColour (Colours::green);
    g.fillEllipse (getLocalBounds().toFloat());
}
