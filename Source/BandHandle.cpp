#include "JuceHeader.h"
#include "BandHandle.h"
#include "Utilities.h"

//==============================================================================
using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

//==============================================================================
BandHandle::BandHandle (const int id, AudioProcessorValueTreeState& s)
: bandId (id), state (s), colour (BandColours::getColourForBand (id))
{
    const auto bandNumber = std::to_string (bandId);
    setName ("Band_Handle_" + String (bandId));
    
    frequencyParam = static_cast<AudioParameterFloat*> (state.getParameter (ParameterNames::frequency + "_band" + bandNumber));
    gainParam = static_cast<AudioParameterFloat*> (state.getParameter (ParameterNames::gain + "_band" + bandNumber));
    qParam = static_cast<AudioParameterFloat*> (state.getParameter (ParameterNames::q + "_band" + bandNumber));
    shapeParam = static_cast<AudioParameterChoice*> (state.getParameter (ParameterNames::shape + "_band" + bandNumber));
    bypassParam = static_cast<AudioParameterBool*> (state.getParameter (ParameterNames::bypass + "_band" + bandNumber));
    
    state.addParameterListener ("frequency_band" + bandNumber, this);
    state.addParameterListener ("gain_band" + bandNumber, this);
    state.addParameterListener ("q_band" + bandNumber, this);
    state.addParameterListener ("shape_band" + bandNumber, this);
    state.addParameterListener ("bypass_band" + bandNumber, this);
    
    setSize (20, 20);
    
//    resized();
    startTimerHz (30);
}

BandHandle::~BandHandle()
{
    stopTimer();
    
    const auto bandNumber = std::to_string (bandId);
    state.removeParameterListener ("frequency_band" + bandNumber, this);
    state.removeParameterListener ("gain_band" + bandNumber, this);
    state.removeParameterListener ("q_band" + bandNumber, this);
    state.removeParameterListener ("shape_band" + bandNumber, this);
    state.removeParameterListener ("bypass_band" + bandNumber, this);
}

//==============================================================================
void BandHandle::parameterChanged (const String& parameterId, float newValue)
{
    if (MessageManager::getInstance()->isThisTheMessageThread())
        update();
    else
        needsUpdate.store (true);
}

void BandHandle::timerCallback()
{
    if (needsUpdate.load() == true)
        update();
}

void BandHandle::update()
{
    jassert (MessageManager::getInstance()->isThisTheMessageThread());
    
    const auto x = valueToNormalisedFrequency (*frequencyParam) * getParentWidth();
    const auto y = (1.0f - gainParam->getNormalisableRange().convertTo0to1 (*gainParam)) * getParentHeight();
    
    setCentrePosition (x, y);
    needsUpdate.store (false);
}

//==============================================================================
void BandHandle::paint (Graphics& g)
{
    ColourGradient fillGradient (Colours::black.brighter (0.6f), Point<float> (getLocalBounds().getCentreX(), 0),
                                 colour, Point<float> (getLocalBounds().getCentreX(), getHeight()),
                                 false);
    g.setGradientFill (fillGradient);
    g.fillEllipse (getLocalBounds().toFloat());
    
    g.setColour (Colours::white);
    g.drawText (String (bandId), getLocalBounds(), Justification::centred);
}

//==============================================================================
void BandHandle::mouseDoubleClick (const MouseEvent& event)
{
    const auto thisBand = ParameterNames::enabled + "_band" + std::to_string (bandId);
    
    static_cast<AudioParameterBool*> (state.getParameter (thisBand))->setValueNotifyingHost (false);
}

void BandHandle::mouseDown (const MouseEvent& event)
{
    mouseDownPosition = event.getEventRelativeTo (getParentComponent()).position;
    
    if (event.mods.isPopupMenu())
    {
        PopupMenu menu;
        const auto choices = shapeParam->choices;
        
        for (auto i {0}; i < choices.size(); ++i)
            menu.addItem (i + 1, choices[i]);
        
        menu.showMenuAsync ({}, [this] (int result) { DBG (result); shapeParam->setValueNotifyingHost (result - 1); });
    }
}

void BandHandle::mouseDrag (const MouseEvent& event)
{
    if (event.getDistanceFromDragStart() < 1)
        return;
    
    const auto proportionalWidth = (mouseDownPosition + event.getOffsetFromDragStart().toFloat()).x / (double) getParentWidth();
    const auto proportionalHeight = (mouseDownPosition + event.getOffsetFromDragStart().toFloat()).y / (double) getParentHeight();
    
    frequencyParam->setValueNotifyingHost (proportionalWidth);
    gainParam->setValueNotifyingHost (1.0f - proportionalHeight);
}
