#include "JuceHeader.h"
#include "ControlPanelBand.h"
#include "Utilities.h"

//==============================================================================
ControlPanelBand::ControlPanelBand (const int id)
: bandId (id)
{
}

//==============================================================================
void ControlPanelBand::paint (Graphics& g)
{
    g.setColour (BandColours::getColourForBand (bandId));
    g.fillRect (getLocalBounds());

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("ControlPanelBand", getLocalBounds(),
                Justification::centred, true);
}

void ControlPanelBand::resized()
{
}
