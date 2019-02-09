#include "PluginProcessor.h"
#include "PluginEditor.h"

using FilterShape = EasyEqAudioProcessor::FilterShape;
using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

//==============================================================================
EasyEqAudioProcessorEditor::EasyEqAudioProcessorEditor (EasyEqAudioProcessor& p,
                                                        AudioProcessorValueTreeState& s)
: AudioProcessorEditor (&p), processor (p), state (s)
{
    state.addParameterListener ("shape", this);
    
    frequencySlider.setSkewFactor (1000);
    frequencySlider.setPopupDisplayEnabled (false, true, this);
    gainSlider.setPopupDisplayEnabled (false, true, this);
    qSlider.setPopupDisplayEnabled (false, true, this);
    qSlider.setSkewFactor (0.707);
    auto shapeChoices = static_cast<AudioParameterChoice*> (state.getParameter ("shape_band0"))->choices;
    shapeSelector.addItemList (shapeChoices, 1);
    
    addAndMakeVisible (frequencySlider);
    addAndMakeVisible (gainSlider);
    addAndMakeVisible (qSlider);
    addAndMakeVisible (bypassButton);
    addAndMakeVisible (shapeSelector);
    
    frequencyAttachment = std::make_unique<SliderAttachment> (state, "frequenc_band0", frequencySlider);
    gainAttachment = std::make_unique<SliderAttachment> (state, "gain_band0", gainSlider);
    qAttachment = std::make_unique<SliderAttachment> (state, "q_band0", qSlider);
    bypassAttachment = std::make_unique<ButtonAttachment> (state, "bypass_band0", bypassButton);
    shapeAttachment = std::make_unique<ComboBoxAttachment> (state, "shape_band0", shapeSelector);
    
    setSize (600, 600);
}

//==============================================================================
void EasyEqAudioProcessorEditor::paint (Graphics& g)
{
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.fillRect (getLocalBounds());
    
    g.setColour (Colours::white);
    g.setFont (14);
    g.drawText (frequencySlider.getName(), getLocalBounds(), Justification::centred);
}

void EasyEqAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    FlexBox flex;
    flex.flexDirection = FlexBox::Direction::row;
    flex.items.add (FlexItem (frequencySlider).withFlex (2));
    flex.items.add (FlexItem (gainSlider).withFlex (3));
    flex.items.add (FlexItem (qSlider).withFlex (2));
    auto sliderArea = bounds.removeFromTop (proportionOfHeight (0.9f));
    flex.performLayout (sliderArea);
    
    bypassButton.setBounds (bounds.removeFromLeft (proportionOfWidth (0.1f)));
    shapeSelector.setBounds (bounds);
}

//==============================================================================
void EasyEqAudioProcessorEditor::parameterChanged (const String& parameterID, float newValue)
{
    if (newValue == FilterShape::Bell)                  gainSlider.setEnabled (true);
    else if (newValue == FilterShape::LowShelf)         gainSlider.setEnabled (true);
    else if (newValue == FilterShape::HighShelf)        gainSlider.setEnabled (true);
    else
        gainSlider.setEnabled (false);
}
