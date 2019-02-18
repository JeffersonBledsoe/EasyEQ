#include "JuceHeader.h"
#include "ControlPanel.h"
#include "Utilities.h"

//==============================================================================
using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

//==============================================================================
ControlPanel::ControlPanel (AudioProcessorValueTreeState& s)
: state (s)
{
    bandName.setText ("Select a band", dontSendNotification);
    bandName.setJustificationType (Justification::centred);
    
    frequencySlider.setSkewFactor (1000);
    qSlider.setSkewFactor (0.707);
    shapeSelector.setText ("No band selected");
    bypassButton.setButtonText ("Bypass");
    
    addAndMakeVisible (bandName);
    addAndMakeVisible (frequencySlider);
    addAndMakeVisible (gainSlider);
    addAndMakeVisible (qSlider);
    addAndMakeVisible (shapeSelector);
    addAndMakeVisible (bypassButton);
    
    frequencySlider.setEnabled (false);
    gainSlider.setEnabled (false);
    qSlider.setEnabled (false);
    shapeSelector.setEnabled (false);
    bypassButton.setEnabled (false);
    
    setSize (500, 120);
}

//==============================================================================
void ControlPanel::setSelectedBand (int bandId)
{
    frequencyAttachment.reset();
    gainAttachment.reset();
    qAttachment.reset();
    bypassAttachment.reset();
    shapeAttachment.reset();
    shapeSelector.clear();
    
    currentBandId = bandId;
    const auto bandNumber = std::to_string (bandId);
    bandName.setText ("Band " + bandNumber, dontSendNotification);
    
    auto shapeParam = static_cast<AudioParameterChoice*> (state.getParameter (ParameterNames::shape + "_band" + bandNumber));
    state.addParameterListener ("shape_band" + std::to_string (bandId), this);
    shapeSelector.addItemList (shapeParam->choices, 1);
    
    frequencySlider.setEnabled (true);
    gainSlider.setEnabled (true);
    qSlider.setEnabled (true);
    shapeSelector.setEnabled (true);
    bypassButton.setEnabled (true);
    
    frequencyAttachment = std::make_unique<SliderAttachment> (state, "frequency_band" + bandNumber, frequencySlider);
    gainAttachment = std::make_unique<SliderAttachment> (state, "gain_band" + bandNumber, gainSlider);
    qAttachment = std::make_unique<SliderAttachment> (state, "q_band" + bandNumber, qSlider);
    bypassAttachment = std::make_unique<ButtonAttachment> (state, "bypass_band" + bandNumber, bypassButton);
    shapeAttachment = std::make_unique<ComboBoxAttachment> (state, "shape_band" + bandNumber, shapeSelector);
    
    frequencySlider.setSkewFactor (500);
    frequencySlider.textFromValueFunction = [this] (double value) { return String (std::exp (std::log (value))); };
}

ControlPanel::~ControlPanel()
{
    state.removeParameterListener ("shape_band" + std::to_string (currentBandId), this);
}

//==============================================================================
void ControlPanel::parameterChanged (const String& parameterId, float newValue)
{
    if (parameterId.containsIgnoreCase (ParameterNames::shape))
    {
        if (newValue == FilterShape::Bell)              gainSlider.setEnabled (true);
        else if (newValue == FilterShape::LowShelf)     gainSlider.setEnabled (true);
        else if (newValue == FilterShape::HighShelf)    gainSlider.setEnabled (true);
        else
            gainSlider.setEnabled (false);
    }
}

//==============================================================================
void ControlPanel::paint (Graphics& g)
{
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId).brighter (0.05f));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 10);
}

void ControlPanel::resized()
{
    auto bounds = getLocalBounds().reduced (3, proportionOfHeight (0.07f));
    
    FlexBox flex;
    flex.flexDirection = FlexBox::Direction::row;
    flex.items.add (FlexItem (bandName).withFlex (1));
    flex.items.add (FlexItem (frequencySlider).withFlex (2));
    flex.items.add (FlexItem (gainSlider).withFlex (3));
    flex.items.add (FlexItem (qSlider).withFlex (2));
    flex.items.add (FlexItem (shapeSelector).withFlex (2).withHeight (40).withAlignSelf (FlexItem::AlignSelf::center));
    flex.items.add (FlexItem (bypassButton).withFlex (2).withHeight (20).withAlignSelf (FlexItem::AlignSelf::center));
    flex.performLayout (bounds);
}
