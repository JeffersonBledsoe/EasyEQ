#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utilities.h"

//==============================================================================
EasyEqAudioProcessorEditor::EasyEqAudioProcessorEditor (EasyEqAudioProcessor& p,
                                                        AudioProcessorValueTreeState& s)
: AudioProcessorEditor (&p), state (s), handleArea (s), controlPanel (s)
{
    addAndMakeVisible (handleArea);
    addAndMakeVisible (controlPanel);
    
    handleArea.addChangeListener (this);
 
    setSize (1000, 600);
}

//==============================================================================
void EasyEqAudioProcessorEditor::paint (Graphics& g)
{
    g.setColour (Colours::black);
    g.fillRect (getLocalBounds());
}

void EasyEqAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    handleArea.setBounds (bounds.removeFromTop (proportionOfHeight (0.8f)));
    controlPanel.setBounds (bounds);
}

//==============================================================================
void EasyEqAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster* broadcaster)
{
    auto* h = static_cast<HandleArea*> (broadcaster);
    
    controlPanel.setSelectedBand (h->selectedBandId);
}
