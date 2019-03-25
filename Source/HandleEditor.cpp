#include "JuceHeader.h"
#include "HandleEditor.h"
#include "Utilities.h"

//==============================================================================
HandleEditor::HandleEditor (AudioProcessorValueTreeState& s)
: state (s)
{
    for (auto i {0}; i < 8; ++i)
    {
        const auto bandEnabledParameterName = ParameterNames::enabled + "_band" + std::to_string (i);
        state.addParameterListener (bandEnabledParameterName, this);
        
        if (*dynamic_cast<AudioParameterBool*> (state.getParameter (bandEnabledParameterName)) == true)
            addAndMakeVisible (handles.add (new BandHandle (i, state)));
    }
}

HandleEditor::~HandleEditor()
{
    for (auto i {0}; i < 8; ++i)
        state.removeParameterListener (ParameterNames::enabled + "_band" + std::to_string (i), this);
}

//==============================================================================
void HandleEditor::paint (Graphics& g)
{
    
}

//==============================================================================
void HandleEditor::parameterChanged (const String& parameterId, float newValue)
{
    const auto bandId = parameterId.getLastCharacters (1).getIntValue();
    
    if (MessageManager::getInstance()->isThisTheMessageThread())
        updateHandle (bandId, newValue > 0.5 ? true : false);
    else
    {
        MessageManager::callAsync ([this, bandId, newValue]
                                   {
                                       updateHandle (bandId, newValue > 0.5 ? true : false);
                                   });
    }
}

void HandleEditor::updateHandle (int bandId, bool shouldAdd)
{
    if (shouldAdd == true)
        addAndMakeVisible (handles.add (new BandHandle (bandId, state)));
    else
    {
        handles.removeObject (*std::find_if (handles.begin(), handles.end(),
                                             [bandId] (BandHandle* handle) { return handle->getBandId() == bandId; }));
        
        resized();
    }
}

//==============================================================================
void HandleEditor::mouseDown (const MouseEvent& event)
{
    mouseDownPosition = event.position;
}

void HandleEditor::mouseDoubleClick (const MouseEvent& event)
{
    if (event.originalComponent != this)
        return;
    
    for (auto i {0}; i < 8; ++i)
    {
        const auto enabledParamName = ParameterNames::enabled + "_band" + std::to_string (i);
        
        if (auto* enabledParam = dynamic_cast<AudioParameterBool*> (state.getParameter (enabledParamName)); *enabledParam == false)
        {
            enabledParam->setValueNotifyingHost (true);
            
            const auto proportionalWidth = event.position.x / double (getWidth());
            const auto proportionalHeight = event.position.y / (double) getHeight();
            
            auto* frequencyParam = static_cast<AudioParameterFloat*> (state.getParameter (ParameterNames::frequency + "_band" + std::to_string (i)));
            auto* gainParam = static_cast<AudioParameterFloat*> (state.getParameter (ParameterNames::gain + "_band" + std::to_string (i)));
            
            frequencyParam->setValueNotifyingHost (proportionalWidth);
            gainParam->setValueNotifyingHost (1.0f - proportionalHeight);
            
            return;
        }
    }
}
