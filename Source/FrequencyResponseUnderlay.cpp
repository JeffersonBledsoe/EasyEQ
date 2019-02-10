#include "JuceHeader.h"
#include "FrequencyResponseUnderlay.h"

//==============================================================================
FrequencyResponseUnderlay::FrequencyResponseUnderlay()
{
    setInterceptsMouseClicks (false, false);
}

void FrequencyResponseUnderlay::paint (Graphics& g)
{
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId).brighter());
    g.fillRect (getLocalBounds());
    
    g.setColour (Colours::red);
    g.strokePath (plotPath, PathStrokeType (2));
}

//==============================================================================
void FrequencyResponseUnderlay::updatePlot (const std::vector<double>& frequencies, const std::vector<double>& magnitudes)
{
    DBG ("update");
    
    if (frequencies.size() < 1 || magnitudes.size() < 1)
        return;
    
    plotPath.clear();
    
    plotPath.startNewSubPath (getX(), getLocalBounds().getCentreY());
    
    const auto yFactor = getHeight() / Decibels::gainToDecibels (2.0);
    const auto xFactor = getWidth() / frequencies.size();
    
    for (auto frequency {0}; frequency < frequencies.size(); ++frequency)
    {
        plotPath.lineTo (getX() + frequency * xFactor,
                         getLocalBounds().getCentreY() - yFactor * std::log (magnitudes [frequency]) / std::log (2));
    }
    
    DBG ((int) plotPath.isEmpty());
    
    repaint();
}
