#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utilities.h"

//==============================================================================
EasyEqAudioProcessorEditor::EasyEqAudioProcessorEditor (EasyEqAudioProcessor& p,
                                                        AudioProcessorValueTreeState& s)
: AudioProcessorEditor (&p), processor (p), state (s), controlPanel (s)
{
    auto* laf = dynamic_cast<LookAndFeel_V4*> (&getLookAndFeel());
    laf->setColour (DocumentWindow::backgroundColourId, Colour::fromRGB (44, 44, 44));
    lookAndFeelChanged();

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
    
    setSize (1000, 600);
    
    processor.addChangeListener (this);
    updatePlot (processor.getFrequencies(), processor.getMagnitudes());
}

EasyEqAudioProcessorEditor::~EasyEqAudioProcessorEditor()
{
    for (auto i {0}; i < 8; ++i)
        state.removeParameterListener (ParameterNames::enabled + "_band" + std::to_string (i), this);
}

//==============================================================================
void EasyEqAudioProcessorEditor::paint (Graphics& g)
{
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId) );
    g.fillRect (getLocalBounds());
    
    g.setColour (Colour::fromRGB (235, 235, 235));
    g.fillPath (frequencyResponsePlotPath);
}

void EasyEqAudioProcessorEditor::resized()
{    
    controlPanel.setBounds (getLocalBounds().removeFromBottom (proportionOfHeight (0.2f)));
}

//==============================================================================
void EasyEqAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster* broadcaster)
{
    if (auto* p = dynamic_cast<EasyEqAudioProcessor*> (broadcaster))
        updatePlot (processor.getFrequencies(), processor.getMagnitudes());
    else
        DBG ("change received from unknown source");
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
        updatePlot (processor.getFrequencies(), processor.getMagnitudes());
    }
    else
    {
        mouseDownPosition = event.position;
        DBG ("Original component: " + event.originalComponent->getName());
    }
}

void EasyEqAudioProcessorEditor::mouseDoubleClick (const MouseEvent& event)
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
            
            const auto proportionalWidth = event.position.x / (double) getWidth();
            const auto proportionalHeight = event.position.y / (double) getHeight();
            
            auto* frequencyParam = static_cast<AudioParameterFloat*> (state.getParameter (ParameterNames::frequency + "_band" + std::to_string (i)));
            auto* gainParam = static_cast<AudioParameterFloat*> (state.getParameter (ParameterNames::gain + "_band" + std::to_string (i)));
            
            frequencyParam->setValueNotifyingHost (proportionalWidth);
            gainParam->setValueNotifyingHost (1.0f - proportionalHeight);
            
            controlPanel.setSelectedBand (i);
            
            return;
        }
    }
}

void EasyEqAudioProcessorEditor::mouseDrag (const MouseEvent& event)
{
    if (frequencyResponseHitPath.contains (mouseDownPosition))
    {
        DBG ("Dragging curve");
    }
}

//==============================================================================
void EasyEqAudioProcessorEditor::updatePlot (const std::vector<double>& frequencies, const std::vector<double>& magnitudes)
{
    if (frequencies.size() < 1 || magnitudes.size() < 1)
        return;
    
    frequencyResponsePlotPath.clear();
    
    const auto yFactor = getHeight() / Decibels::gainToDecibels (2.0);
    const auto xFactor = getWidth() / frequencies.size();
    
    frequencyResponsePlotPath.startNewSubPath (getX(),
                              getLocalBounds().getCentreY() - yFactor * std::log (magnitudes[0]) / std::log (2));
    
    for (auto frequency {1}; frequency < frequencies.size(); ++frequency)
    {
        frequencyResponsePlotPath.lineTo (getX() + frequency * xFactor,
                         getLocalBounds().getCentreY() - yFactor * std::log (magnitudes[frequency]) / std::log (2));
    }
    
    PathStrokeType frequencyResponseHitPathStroke (6.0f);
    frequencyResponseHitPathStroke.createStrokedPath (frequencyResponseHitPath, frequencyResponsePlotPath);
    
    PathStrokeType frequencyResponsePlotPathStroke (3.0f);
    frequencyResponsePlotPathStroke.createStrokedPath (frequencyResponsePlotPath, frequencyResponsePlotPath);
    
    repaint();
}
