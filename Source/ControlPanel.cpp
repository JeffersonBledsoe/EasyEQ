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
    {
        state.removeParameterListener ("shape_band" + std::to_string (currentBandId), this);
        frequencyAttachment.reset();
        gainAttachment.reset();
        qAttachment.reset();
        bypassAttachment.reset();
        shapeAttachment.reset();
        shapeSelector.clear();
    }
    
    currentBandId = bandId;
    const auto bandNumber = std::to_string (bandId);
    bandName.setText ("Band " + bandNumber, dontSendNotification);
    
    shapeParam = dynamic_cast<AudioParameterChoice*> (state.getParameter (ParameterNames::shape + "_band" + bandNumber));
    state.addParameterListener ("shape_band" + std::to_string (bandId), this);
    shapeSelector.addItemList (shapeParam->choices, 1);
    
    frequencySlider.setEnabled (true);
    gainSlider.setEnabled (true);
    qSlider.setEnabled (true);
    shapeSelector.setEnabled (true);
    bypassButton.setEnabled (true);
    
    frequencySlider.setSkewFactor (1000.0);
    
    frequencyAttachment.reset (new SliderAttachment (state, "frequency_band" + bandNumber, frequencySlider));
    gainAttachment.reset (new SliderAttachment (state, "gain_band" + bandNumber, gainSlider));
    qAttachment = std::make_unique<SliderAttachment> (state, "q_band" + bandNumber, qSlider);
    bypassAttachment = std::make_unique<ButtonAttachment> (state, "bypass_band" + bandNumber, bypassButton);
    shapeAttachment = std::make_unique<ComboBoxAttachment> (state, "shape_band" + bandNumber, shapeSelector);
    
    frequencySlider.setSkewFactor (1000.0);
}

ControlPanel::~ControlPanel()
{
    state.removeParameterListener ("shape_band" + std::to_string (currentBandId), this);
}

//==============================================================================
void ControlPanel::parameterChanged (const String& parameterId, float newValue)
{
    if (MessageManager::getInstance()->isThisTheMessageThread())
        update();
    else
        MessageManager::callAsync ([this] { update(); });
}

void ControlPanel::update()
{
    if (*shapeParam == FilterShape::Bell)              gainSlider.setEnabled (true);
    else if (*shapeParam == FilterShape::LowShelf)     gainSlider.setEnabled (true);
    else if (*shapeParam == FilterShape::HighShelf)    gainSlider.setEnabled (true);
    else
        gainSlider.setEnabled (false);
}

//==============================================================================
void ControlPanel::paint (Graphics& g)
{
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId).brighter (0.12f));
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
    
    FlexBox endColumn;
    endColumn.flexDirection = FlexBox::Direction::column;
    endColumn.items.add (FlexItem (bypassButton).withFlex (2));
    endColumn.items.add (FlexItem (shapeSelector).withFlex (3));
    
    flex.items.add (FlexItem (endColumn).withFlex (2));
    flex.performLayout (bounds);
}
