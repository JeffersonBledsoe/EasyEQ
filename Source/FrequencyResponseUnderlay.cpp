#include "JuceHeader.h"
#include "FrequencyResponseUnderlay.h"

//==============================================================================
FrequencyResponseUnderlay::FrequencyResponseUnderlay()
{
    setName ("Response Underlay");
}

void FrequencyResponseUnderlay::paint (Graphics& g)
{
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId) );
    g.fillRect (getLocalBounds());
    
    g.setColour (Colour::fromRGB (235, 235, 235));
    g.fillPath (plotPath);
}

//==============================================================================
void FrequencyResponseUnderlay::updatePlot (const std::vector<double>& frequencies, const std::vector<double>& magnitudes)
{
    if (frequencies.size() < 1 || magnitudes.size() < 1)
        return;
    
    plotPath.clear();
    
    const auto yFactor = getHeight() / Decibels::gainToDecibels (2.0);
    const auto xFactor = getWidth() / frequencies.size();
    
    plotPath.startNewSubPath (getX(),
                              getLocalBounds().getCentreY() - yFactor * std::log (magnitudes[0]) / std::log (2));
    
    for (auto frequency {1}; frequency < frequencies.size(); ++frequency)
    {
        plotPath.lineTo (getX() + frequency * xFactor,
                         getLocalBounds().getCentreY() - yFactor * std::log (magnitudes[frequency]) / std::log (2));
    }
    
    PathStrokeType hitPathStroke (6.0f);
    hitPathStroke.createStrokedPath (hitPath, plotPath);
    
    PathStrokeType plotPathStroke (3.0f);
    plotPathStroke.createStrokedPath (plotPath, plotPath);
    
    repaint();
}

//==============================================================================
bool FrequencyResponseUnderlay::hitTest (int x, int y)
{
    if (hitPath.contains (x, y))
        return true;
    
    return false;
}
