#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utilities.h"

//==============================================================================
EasyEqAudioProcessorEditor::EasyEqAudioProcessorEditor (EasyEqAudioProcessor& p,
                                                        AudioProcessorValueTreeState& s)
//: AudioProcessorEditor (&p), processor (p), state (s), handleControl (s), controlPanel (s)
: AudioProcessorEditor (&p), processor (p), state (s), controlPanel (s)
{
    addAndMakeVisible (frequencyResponse);
//    addAndMakeVisible (handleControl);
    addAndMakeVisible (controlPanel);
    
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
    
    frequencyResponse.addMouseListener (this, false);
//    handleControl.addChangeListener (this);
    
    setSize (1000, 600);
    
    processor.addChangeListener (this);
}

EasyEqAudioProcessorEditor::~EasyEqAudioProcessorEditor()
{
    for (auto i {0}; i < 8; ++i)
        state.removeParameterListener (ParameterNames::enabled + "_band" + std::to_string (i), this);
}

//==============================================================================
void EasyEqAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    auto handleArea = bounds.removeFromTop (proportionOfHeight (0.8f));
    
    frequencyResponse.setBounds (handleArea);
//    handleControl.setBounds (handleArea);
    
    controlPanel.setBounds (bounds);
}

//==============================================================================
void EasyEqAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster* broadcaster)
{
    DBG ("change receive");
    
//    if (auto* h = dynamic_cast<HandleArea*> (broadcaster))
//    {
//        controlPanel.setSelectedBand (h->selectedBandId);
//
//        frequencyResponse.updatePlot (processor.getFrequencies(), processor.getMagnitudes());
//    }
    if (auto* p = dynamic_cast<EasyEqAudioProcessor*> (broadcaster))
    {
        DBG ("processor change");
        frequencyResponse.updatePlot (processor.getFrequencies(), processor.getMagnitudes());
    }
}

void EasyEqAudioProcessorEditor::parameterChanged (const String& parameterId, float newValue)
{
    const auto bandId = parameterId.getLastCharacters (1).getIntValue();
    
    if (newValue == true)
    {
        auto* handle = handles.add (new BandHandle (bandId, state));
        addAndMakeVisible (handle);
        handle->addMouseListener (this, true);
        selectedBandId = handle->getBandId();
        resized();
    }
    else
    {
        handles.remove (bandId);
        resized();
    }
}

//==============================================================================
void EasyEqAudioProcessorEditor::mouseDown (const MouseEvent& event)
{
    if (auto* handle = dynamic_cast<BandHandle*> (event.originalComponent))
    {
        selectedBandId = handle->getBandId();
        controlPanel.setSelectedBand (selectedBandId);
        frequencyResponse.updatePlot (processor.getFrequencies(), processor.getMagnitudes());
    }
    else
        DBG ("Original componen: " + event.originalComponent->getName());
}

void EasyEqAudioProcessorEditor::mouseDoubleClick (const MouseEvent& event)
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
