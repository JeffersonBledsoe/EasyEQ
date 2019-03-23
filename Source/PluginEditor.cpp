#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utilities.h"

//==============================================================================
EasyEqAudioProcessorEditor::EasyEqAudioProcessorEditor (EasyEqAudioProcessor& p,
                                                        AudioProcessorValueTreeState& s)
: AudioProcessorEditor (&p), processor (p), state (s), handleEditor (s), controlPanel (s)
{
    setLookAndFeel (&laf);
    
    setSize (1000, 600);
    
    processor.addChangeListener (this);
    updatePlot (processor.getFrequencies(), processor.getMagnitudes());
    addAndMakeVisible (handleEditor);
    addAndMakeVisible (controlPanel);
}

EasyEqAudioProcessorEditor::~EasyEqAudioProcessorEditor()
{
    processor.removeChangeListener (this);
    
    setLookAndFeel (nullptr);
}

//==============================================================================
void EasyEqAudioProcessorEditor::paint (Graphics& g)
{
    // todo: remove once response is moved to handle editor
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId) );
    g.fillRect (getLocalBounds());
    
    g.setColour (Colour::fromRGB (235, 235, 235));
    g.fillPath (frequencyResponsePlotPath);
    
    g.setOpacity (0.4f);
    g.drawHorizontalLine (getLocalBounds().getCentreY(), getX(), getRight());
}

void EasyEqAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    handleEditor.setBounds (bounds.removeFromTop (proportionOfHeight (0.8f)));
    controlPanel.setBounds (bounds);
}

//==============================================================================
void EasyEqAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster* broadcaster)
{
    if (auto* p = dynamic_cast<EasyEqAudioProcessor*> (broadcaster))
        updatePlot (processor.getFrequencies(), processor.getMagnitudes());
    else
        DBG ("change received from unknown source");
}

//==============================================================================
void EasyEqAudioProcessorEditor::updatePlot (const std::vector<double>& frequencies, const std::vector<double>& magnitudes)
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
