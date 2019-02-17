#include "JuceHeader.h"
#include "BandHandle.h"
#include "Utilities.h"

//==============================================================================
using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

//==============================================================================
BandHandle::BandHandle (const int id, AudioProcessorValueTreeState& s)
: bandId (id), state (s)
{
    setSize (20, 20);
    
    const auto bandNumber = std::to_string (bandId);
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
    
    setName ("Band " + bandNumber);
    
    resized();
}

BandHandle::~BandHandle()
{
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
    if (parameterId.containsIgnoreCase (ParameterNames::frequency) || parameterId.containsIgnoreCase (ParameterNames::gain))
    {
        MessageManager::callAsync ([this]
        {
            const auto x = frequencyParam->getNormalisableRange().convertTo0to1 (*frequencyParam) * getParentWidth();
            const auto y = gainParam->getNormalisableRange().convertTo0to1 (jlimit (0.1f, 2.0f, 2.0f - *gainParam)) * getParentHeight();
            
            setCentrePosition (x, y);
        });
    }
}

//==============================================================================
void BandHandle::paint (Graphics& g)
{
    g.setColour (Colours::pink);
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
    if (proportionalHeight > 0)
        gainParam->setValueNotifyingHost (1.0f - proportionalHeight);
    
    repaint();
}
