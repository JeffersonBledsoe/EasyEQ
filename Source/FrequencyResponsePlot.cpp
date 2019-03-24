#include "JuceHeader.h"
#include "FrequencyResponsePlot.h"

//==============================================================================
FrequencyResponsePlot::FrequencyResponsePlot()
{
    
}

//==============================================================================
void FrequencyResponsePlot::updatePlot (const std::vector<double>& frequencies, const std::vector<double>& magnitudes)
{
    if (frequencies.size() < 1 || magnitudes.size() < 1)
        return;
    
    frequencyResponsePlotPath.clear();
    
    const auto yFactor = getHeight() / Decibels::gainToDecibels (2.0);
    const auto xFactor = getWidth() / frequencies.size();
    
    frequencyResponsePlotPath.startNewSubPath (getX(),
                                               getLocalBounds().getCentreY() - yFactor * std::log (magnitudes[0]) / std::log (MathConstants<double>::euler));
    
    for (auto frequency {1}; frequency < frequencies.size(); ++frequency)
    {
        frequencyResponsePlotPath.lineTo (getX() + frequency * xFactor,
                                          getLocalBounds().getCentreY() - yFactor * std::log (magnitudes[frequency]) / std::log (MathConstants<double>::euler));
    }
    
    PathStrokeType frequencyResponseHitPathStroke (6.0f);
    frequencyResponseHitPathStroke.createStrokedPath (frequencyResponseHitPath, frequencyResponsePlotPath);
    
    PathStrokeType frequencyResponsePlotPathStroke (3.0f);
    frequencyResponsePlotPathStroke.createStrokedPath (frequencyResponsePlotPath, frequencyResponsePlotPath);
    
    repaint();
}

//==============================================================================
void FrequencyResponsePlot::paint (Graphics& g)
{
    g.setColour (Colour::fromRGB (235, 235, 235));
    g.fillPath (frequencyResponsePlotPath);
    
    g.setOpacity (0.4f);
    g.drawHorizontalLine (getLocalBounds().getCentreY(), getX(), getRight());
}

//==============================================================================
bool FrequencyResponsePlot::hitTest (int x, int y)
{
    if (frequencyResponseHitPath.contains (x, y))
        return true;
    
    return false;
}
