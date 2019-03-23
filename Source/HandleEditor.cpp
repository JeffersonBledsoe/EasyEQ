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
        
        auto enabledParam = static_cast<AudioParameterBool*> (state.getParameter (bandEnabledParameterName));
        
        if (*enabledParam == true)
        {
            auto* handle = handles.add (new BandHandle (i, state));
            addAndMakeVisible (handle);
            handle->addMouseListener (this, true);
            resized();
        }
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
//    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId) );
//    g.fillRect (getLocalBounds());
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
        handles.remove (bandId);
        resized();
    }
}

//==============================================================================
void HandleEditor::mouseDown (const MouseEvent& event)
{
    mouseDownPosition = event.getEventRelativeTo (this).position;
}

void HandleEditor::mouseDoubleClick (const MouseEvent& event)
{
    if (event.originalComponent != this)
        return;
    
    for (auto i {0}; i < 8; ++i)
    {
        const auto enabledParamName = ParameterNames::enabled + "_band" + std::to_string (i);
        auto enabledParam = dynamic_cast<AudioParameterBool*> (state.getParameter (enabledParamName));
        
        if (enabledParam == nullptr)
        {
            DBG ("Invalid enabled parameter");
            return;
        }
        
        if (*enabledParam == false)
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
