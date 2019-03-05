#pragma once

#include "JuceHeader.h"

//==============================================================================
class BandHandle : public Component,
                   public AudioProcessorValueTreeState::Listener
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
    
    //==========================================================================
    Point<float> mouseDownPosition, newPosition;
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
    void update();
    SafePointer<BandHandle> thisComponent = this;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandHandle)
};
