#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utilities.h"

//==============================================================================
EasyEqAudioProcessorEditor::EasyEqAudioProcessorEditor (EasyEqAudioProcessor& p,
                                                        AudioProcessorValueTreeState& s)
: AudioProcessorEditor (&p), processor (p), state (s), controlPanel (s)
{
    setLookAndFeel (&laf);
    
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
    
    setSize (1000, 600);
    
    processor.addChangeListener (this);
    updatePlot (processor.getFrequencies(), processor.getMagnitudes());
    addChildComponent (controlPanel);
}

EasyEqAudioProcessorEditor::~EasyEqAudioProcessorEditor()
{
    processor.removeChangeListener (this);
    for (auto i {0}; i < 8; ++i)
        state.removeParameterListener (ParameterNames::enabled + "_band" + std::to_string (i), this);
    
    setLookAndFeel (nullptr);
}

//==============================================================================
void EasyEqAudioProcessorEditor::paint (Graphics& g)
{
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId) );
    g.fillRect (getLocalBounds());
    
    g.setColour (Colour::fromRGB (235, 235, 235));
    g.fillPath (frequencyResponsePlotPath);
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

void EasyEqAudioProcessorEditor::updateHandle (int bandId, bool shouldAdd)
{
    if (shouldAdd == true)
    {
        auto* handle = handles.add (new BandHandle (bandId, state));
        addAndMakeVisible (handle);
        handle->addMouseListener (this, true);
        selectedHandle = handle;
        updateControlPanelPosition (handle->getBoundsInParent().getCentre().toFloat());
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
    mouseDownPosition = event.getEventRelativeTo (this).position;
    
    if (auto* handle = dynamic_cast<BandHandle*> (event.originalComponent))
    {
        selectedHandle = handle;
        controlPanel.setSelectedBand (selectedHandle->getBandId());
        updateControlPanelPosition (handle->getBoundsInParent().getCentre().toFloat());
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
            
            const auto proportionalWidth = event.position.x / double (getWidth());
            const auto proportionalHeight = event.position.y / (double) getHeight();
            
            auto* frequencyParam = static_cast<AudioParameterFloat*> (state.getParameter (ParameterNames::frequency + "_band" + std::to_string (i)));
            auto* gainParam = static_cast<AudioParameterFloat*> (state.getParameter (ParameterNames::gain + "_band" + std::to_string (i)));
            
            frequencyParam->setValueNotifyingHost (valueToNormalisedFrequency (normalisedValueToFrequency (proportionalWidth)));
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
    else if (event.originalComponent == selectedHandle)
        updateControlPanelPosition (mouseDownPosition + event.getOffsetFromDragStart().toFloat());
}

//==============================================================================
void EasyEqAudioProcessorEditor::updateControlPanelPosition (const Point<float> position)
{
    controlPanel.setVisible (true);
    
    const auto controlPanelTop = getHeight() - proportionOfHeight (0.15f);
    const auto controlPanelRadius = controlPanel.getWidth() / 2.0f;
    
    const auto controlPanelX = jlimit (controlPanelRadius, getWidth() - controlPanelRadius, position.x);
    
    controlPanel.setCentrePosition (controlPanelX, controlPanelTop);
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
