#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EasyEqAudioProcessorEditor::EasyEqAudioProcessorEditor (EasyEqAudioProcessor& p,
                                                        AudioProcessorValueTreeState& s)
: AudioProcessorEditor (&p), processor (p), state (s)
{
    for (auto i {0}; i < 8; ++i)
        addAndMakeVisible (handles.add (new BandHandle (i, state)));
    
    setSize (1000, 600);
}

//==============================================================================
void EasyEqAudioProcessorEditor::paint (Graphics& g)
{
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.fillRect (getLocalBounds());
}

void EasyEqAudioProcessorEditor::resized()
{
    FlexBox flex;
    flex.flexDirection = FlexBox::Direction::row;
    
    for (auto* handle : handles)
        flex.items.add (FlexItem (*handle).withFlex (1));
    
    flex.performLayout (getLocalBounds());
}
