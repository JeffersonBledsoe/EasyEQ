#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utilities.h"

//==============================================================================
EasyEqAudioProcessorEditor::EasyEqAudioProcessorEditor (EasyEqAudioProcessor& p,
                                                        AudioProcessorValueTreeState& s)
: AudioProcessorEditor (&p), processor (p), state (s), handleControl (s), controlPanel (s)
{
    addAndMakeVisible (frequencyResponse);
    addAndMakeVisible (handleControl);
    addAndMakeVisible (controlPanel);
    
    handleControl.addChangeListener (this);
    
    setSize (1000, 600);
    
    processor.addChangeListener (this);
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
    DBG ("change receive");
    
    if (auto* h = dynamic_cast<HandleArea*> (broadcaster))
    {
        controlPanel.setSelectedBand (h->selectedBandId);
        
        frequencyResponse.updatePlot (processor.getFrequencies(), processor.getMagnitudes());
    }
    else if (auto* p = dynamic_cast<EasyEqAudioProcessor*> (broadcaster))
    {
        DBG ("processor change");
        frequencyResponse.updatePlot (processor.getFrequencies(), processor.getMagnitudes());
    }
}
