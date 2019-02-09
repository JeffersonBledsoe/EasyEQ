#include "JuceHeader.h"
#include "BandHandle.h"
#include "PluginProcessor.h"

//==============================================================================
using FilterShape = EasyEqAudioProcessor::FilterShape;
using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

//==============================================================================
BandHandle::BandHandle (int id, AudioProcessorValueTreeState& s)
: bandId (id), state (s)
{
    const auto bandNumber = std::to_string (bandId);
    bandName.setText ("Band " + bandNumber, dontSendNotification);
    bandName.setJustificationType (Justification::centred);
    
    frequencySlider.setSkewFactor (1000);
    qSlider.setSkewFactor (0.707);
    state.addParameterListener ("shape", this);
    
    state.addParameterListener ("frequency_band" + std::to_string (bandId), this);
    state.addParameterListener ("gain_band" + std::to_string (bandId), this);
    state.addParameterListener ("q_band" + std::to_string (bandId), this);
    state.addParameterListener ("shape_band" + std::to_string (bandId), this);
    state.addParameterListener ("bypass_band" + std::to_string (bandId), this);
    
    auto shapeChoices = static_cast<AudioParameterChoice*> (state.getParameter ("shape_band" + bandNumber))->choices;
    shapeSelector.addItemList (shapeChoices, 1);
    bypassButton.setButtonText ("Bypass");
    
    addAndMakeVisible (bandName);
    addAndMakeVisible (frequencySlider);
    addAndMakeVisible (gainSlider);
    addAndMakeVisible (qSlider);
    addAndMakeVisible (bypassButton);
    addAndMakeVisible (shapeSelector);
    
    frequencyAttachment = std::make_unique<SliderAttachment> (state, "frequency_band" + bandNumber, frequencySlider);
    gainAttachment = std::make_unique<SliderAttachment> (state, "gain_band" + bandNumber, gainSlider);
    qAttachment = std::make_unique<SliderAttachment> (state, "q_band" + bandNumber, qSlider);
    bypassAttachment = std::make_unique<ButtonAttachment> (state, "bypass_band" + bandNumber, bypassButton);
    shapeAttachment = std::make_unique<ComboBoxAttachment> (state, "shape_band" + bandNumber, shapeSelector);
}

//==============================================================================
void BandHandle::parameterChanged (const String& parameterId, float newValue)
{
//    const auto changedBand = parameterId.getLastCharacters (1).getIntValue();
    const auto changedParameter = parameterId.upToFirstOccurrenceOf ("_", false, true);
    
    if (changedParameter == "shape")
    {
        if (newValue == FilterShape::Bell)
        {
            gainSlider.setEnabled (true);
            gainSlider.setVisible (true);
        }
        else if (newValue == FilterShape::LowShelf)
        {
            gainSlider.setEnabled (true);
            gainSlider.setVisible (true);}
        else if (newValue == FilterShape::HighShelf)
        {
            gainSlider.setEnabled (true);
            gainSlider.setVisible (true);
        }
        else
        {
            gainSlider.setEnabled (false);
            gainSlider.setVisible (false);
        }
    }
}

//==============================================================================
void BandHandle::paint (Graphics& g)
{
    g.setColour (Colour (bandId * 32, bandId * 16, bandId * 8));
    g.drawRoundedRectangle (getLocalBounds().reduced (3).toFloat(), 10, 4);
}

void BandHandle::resized()
{
    auto bounds = getLocalBounds().reduced (3, proportionOfHeight (0.05f));
    
    FlexBox flex;
    flex.flexDirection = FlexBox::Direction::column;
    flex.items.add (FlexItem (bandName).withFlex (1));
    flex.items.add (FlexItem (frequencySlider).withFlex (2));
    flex.items.add (FlexItem (gainSlider).withFlex (3));
    flex.items.add (FlexItem (qSlider).withFlex (2));
    flex.items.add (FlexItem (shapeSelector).withFlex (2).withHeight (10));
    flex.items.add (FlexItem (bypassButton).withFlex (2).withHeight (20));
    flex.performLayout (bounds);
}
