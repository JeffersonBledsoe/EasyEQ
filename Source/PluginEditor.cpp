#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utilities.h"

//==============================================================================
EasyEqAudioProcessorEditor::EasyEqAudioProcessorEditor (EasyEqAudioProcessor& p,
                                                        AudioProcessorValueTreeState& s)
: AudioProcessorEditor (&p), processor (p), state (s), handleEditor (s), controlPanel (s)
{
    processor.addChangeListener (this);
    setLookAndFeel (&laf);

    addAndMakeVisible (handleEditor);
    addAndMakeVisible (frequencyResponsePlot);
    addAndMakeVisible (controlPanel);
    
    setSize (1000, 600);
}

EasyEqAudioProcessorEditor::~EasyEqAudioProcessorEditor()
{
    processor.removeChangeListener (this);
    
    setLookAndFeel (nullptr);
}

//==============================================================================
void EasyEqAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    handleEditor.setBounds (bounds.removeFromTop (proportionOfHeight (0.8f)));
    frequencyResponsePlot.setBounds (handleEditor.getBounds());
    controlPanel.setBounds (bounds);
}

//==============================================================================
void EasyEqAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster* broadcaster)
{
    if (auto* p = dynamic_cast<EasyEqAudioProcessor*> (broadcaster))
        frequencyResponsePlot.updatePlot (processor.getFrequencies(), processor.getMagnitudes());
    else
        DBG ("change received from unknown source");
}
