#include "JuceHeader.h"
#include "HandleArea.h"
#include "Utilities.h"

//==============================================================================
HandleArea::HandleArea (AudioProcessorValueTreeState& s)
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
            selectedBandId = handle->getBandId();
            resized();
        }       
    }
}

HandleArea::~HandleArea()
{
    for (auto i {0}; i < 8; ++i)
        state.removeParameterListener (ParameterNames::enabled + "_band" + std::to_string (i), this);
}

//==========================================================================
void HandleArea::parameterChanged (const String& parameterId, float newValue)
{
    const auto bandId = parameterId.getLastCharacters (1).getIntValue();
    
    if (newValue == true)
    {
        auto* handle = handles.add (new BandHandle (bandId, state));
        addAndMakeVisible (handle);
        handle->addMouseListener (this, true);
        selectedBandId = handle->getBandId();
        resized();
        sendChangeMessage();
    }
    else
    {
        handles.remove (bandId);
        resized();
    }
}

//==============================================================================
void HandleArea::mouseDown (const MouseEvent& event)
{
    if (auto* handle = dynamic_cast<BandHandle*> (event.originalComponent))
    {
        selectedBandId = handle->getBandId();
        sendChangeMessage();
    }
}

void HandleArea::mouseDoubleClick (const MouseEvent& event)
{
    if (event.originalComponent != this)
        return;
    
    for (auto i {0}; i < 8; ++i)
    {
        const auto bandToAdd = ParameterNames::enabled + "_band" + std::to_string (i);
        auto enabledParam = static_cast<AudioParameterBool*> (state.getParameter (bandToAdd));
        
        if (enabledParam == nullptr)
            return;
        
        if (*enabledParam == false)
        {
            enabledParam->setValueNotifyingHost (true);
            
            const auto proportionalWidth = event.position.x / (double) getWidth();
            const auto proportionalHeight = event.position.y / (double) getHeight();
            
            auto* frequencyParam = static_cast<AudioParameterFloat*> (state.getParameter (ParameterNames::frequency + "_band" + std::to_string (i)));
            auto* gainParam = static_cast<AudioParameterFloat*> (state.getParameter (ParameterNames::gain + "_band" + std::to_string (i)));
            
            frequencyParam->setValueNotifyingHost (proportionalWidth);
            gainParam->setValueNotifyingHost (1.0f - proportionalHeight);
            
            return;
        }
    }
}
