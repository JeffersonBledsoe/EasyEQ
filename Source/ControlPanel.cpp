#include "JuceHeader.h"
#include "ControlPanel.h"
#include "Utilities.h"
#include <numeric>

//==============================================================================
using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

std::array<ControlPanelBand, 8> createBandControls (AudioProcessorValueTreeState& state)
{
   std::array<ControlPanelBand, 8> bands
    {
        ControlPanelBand { 0, state },
        ControlPanelBand { 1, state },
        ControlPanelBand { 2, state },
        ControlPanelBand { 3, state },
        ControlPanelBand { 4, state },
        ControlPanelBand { 5, state },
        ControlPanelBand { 6, state },
        ControlPanelBand { 7, state }
    };
    
    return bands;
}

//==============================================================================
ControlPanel::ControlPanel (AudioProcessorValueTreeState& s)
: state (s), bands (createBandControls (s))
{
    setOpaque (true);
    setBufferedToImage (true);
    
    for (auto& bandControl : bands)
        addChildComponent (bandControl);
}

//==============================================================================
void ControlPanel::paint (Graphics& g)
{
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId).brighter (0.12f));
    g.fillRect (getLocalBounds());
}

void ControlPanel::resized()
{
    Grid grid;
    using Track = Grid::TrackInfo;
    grid.templateRows    = { Track (1_fr) };
    grid.autoColumns = Track (1_fr);
    
    grid.autoFlow = Grid::AutoFlow::column;
    
    for (auto& bandControl: bands)
        grid.items.add (bandControl);
    
    grid.performLayout (getLocalBounds());
}
