#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utilities.h"

//==============================================================================
EasyEqAudioProcessorEditor::EasyEqAudioProcessorEditor (EasyEqAudioProcessor& p,
                                                        AudioProcessorValueTreeState& s)
: AudioProcessorEditor (&p), state (s), handleControl (s), controlPanel (s)
{
    addAndMakeVisible (frequencyResponse);
    addAndMakeVisible (handleControl);
    addAndMakeVisible (controlPanel);
    
    handleControl.addChangeListener (this);
    
    setSize (1000, 600);
}

//==============================================================================
void EasyEqAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    auto handleArea = bounds.removeFromTop (proportionOfHeight (0.8f));
    
    frequencyResponse.setBounds (handleArea);
    handleControl.setBounds (handleArea);
    
    controlPanel.setBounds (bounds);
}

//==============================================================================
void EasyEqAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster* broadcaster)
{
    auto* h = static_cast<HandleArea*> (broadcaster);
    
    controlPanel.setSelectedBand (h->selectedBandId);
}
