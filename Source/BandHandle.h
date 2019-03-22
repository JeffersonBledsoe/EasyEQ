#pragma once

#include "JuceHeader.h"

//==============================================================================
class BandHandle : public Component,
                   public AudioProcessorValueTreeState::Listener,
                   public Timer
{
public:
    //==========================================================================
    BandHandle (const int bandId, AudioProcessorValueTreeState& state);
    ~BandHandle();
    
    //==============================================================================
    void parameterChanged (const String& parameterID, float newValue) override;
    void paint (Graphics& g) override;
    
    const int getBandId() const noexcept
    { return bandId; }
    
private:
    //==============================================================================
    const int bandId;
    AudioProcessorValueTreeState& state;
    Colour colour;
    
    //==========================================================================
    Point<float> mouseDownPosition;
    void mouseDoubleClick (const MouseEvent& event) override;
    void mouseDown (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    
    //==========================================================================
    AudioParameterFloat* frequencyParam;
    AudioParameterFloat* gainParam;
    AudioParameterFloat* qParam;
    AudioParameterChoice* shapeParam;
    AudioParameterBool* bypassParam;
    AudioParameterBool* enabledParam;
    
    //==========================================================================
    std::atomic<bool> needsUpdate { false };
    void update();
    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandHandle)
};
