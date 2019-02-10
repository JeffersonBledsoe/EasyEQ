#pragma once

#include "JuceHeader.h"

//==============================================================================
class EasyEqAudioProcessor : public AudioProcessor,
                             public AudioProcessorValueTreeState::Listener,
                             public ChangeBroadcaster
{
public:
    //==============================================================================
    EasyEqAudioProcessor();
    ~EasyEqAudioProcessor() = default;
    
    //==============================================================================
    const std::vector<double>& getMagnitudes() const
    { return magnitudes; }
    
    const std::vector<double>& getFrequencies() const
    { return frequencies; }

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
    void releaseResources() override;
    
    //==============================================================================
    void parameterChanged (const String& parameterID, float newValue) override;

    //==============================================================================
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    //==============================================================================
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    

private:
    //==============================================================================
    AudioProcessorValueTreeState state;
    using EqBand = dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>>;
    dsp::ProcessorChain<EqBand, EqBand, EqBand, EqBand, EqBand, EqBand, EqBand, EqBand> equaliser;
    void updateBand (int bandId);
    
    //==============================================================================
    double currentSampleRate { 0 };
    
    //==============================================================================
    std::vector<double> frequencies;
    std::vector<double> magnitudes;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyEqAudioProcessor)
};
