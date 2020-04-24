#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utilities.h"

//==============================================================================
inline String gainToFloat (float value)
{
    auto val = String (Decibels::gainToDecibels (value));
    
    return val.upToFirstOccurrenceOf (".", true, true) + val.fromFirstOccurrenceOf (".", false, true).substring (0, 2);
}

StringArray getFilterShapeNames()
{
    StringArray shapes;
    
    for (auto i {0}; i < FilterShapes::numOfShapes; ++i)
        shapes.add (FilterShapes::getFilterShapeNameForId (i));
    
    return shapes;
}

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout createParameters()
{
    AudioProcessorValueTreeState::ParameterLayout parameters;
    
    for (auto i {0}; i < 8; ++i)
    {
        const auto bandId = std::to_string (i);
        auto frequency = std::make_unique<AudioParameterFloat> (ParameterNames::frequency + "_band" + bandId, "Band " + bandId + " Frequency",
                                                                NormalisableRange<float> (20.0f,
                                                                                          20000.0f,
                                                                                          [] (float min, float max, float normalisedValue)
                                                                                          {
                                                                                              return normalisedValueToFrequency (normalisedValue, min, max);
                                                                                          },
                                                                                          [] (float min, float max, float value)
                                                                                          {
                                                                                              return valueToNormalisedFrequency (value, min, max);
                                                                                          }),
                                                                1000.0f,
                                                                "Hz", AudioProcessorParameter::genericParameter,
                                                                [] (float value, int) { return String (normalisedValueToFrequency (value), 0)
                                                                                                      .substring (0, 6).trimCharactersAtEnd ("."); },
                                                                [] (String text) { return valueToNormalisedFrequency (text.getFloatValue()); });
        auto gain = std::make_unique<AudioParameterFloat> (ParameterNames::gain + "_band" + bandId, "Band " + bandId + " Gain",
                                                           NormalisableRange<float> (-24.0f, 24.0f), 0.0f,
                                                           "dB", AudioProcessorParameter::genericParameter,
                                                           [] (float value, int) { return gainToFloat (value); },
                                                           [] (const String& text) { return Decibels::decibelsToGain (text.getFloatValue()); });
        auto q = std::make_unique<AudioParameterFloat> (ParameterNames::q + "_band" + bandId, "Band " + bandId + " Q",
                                                        NormalisableRange<float> (0.05f, 30.0f), 1.0f,
                                                        String(), AudioProcessorParameter::genericParameter);
        auto shape = std::make_unique<AudioParameterChoice> (ParameterNames::shape + "_band" + bandId, "Band " + bandId + " Shape",
                                                             getFilterShapeNames(), 0);
        auto bypass = std::make_unique<AudioParameterBool> (ParameterNames::bypass + "_band" + bandId, "Band " + bandId + " Bypass",
                                                            false, String(),
                                                            [] (bool bypassed, int) { return bypassed ? "Bypassed" : "Active"; });
        auto enabled = std::make_unique<AudioParameterBool> (ParameterNames::enabled + "_band" + bandId, "Band " + bandId + " Enabled",
                                                            false, String(),
                                                            [] (bool enabled, int) { return enabled ? "Disabled" : "Enabled"; });
        
        auto group = std::make_unique<AudioProcessorParameterGroup> ("band" + bandId, "Band"  + bandId,
                                                                     "|",
                                                                     std::move (frequency),
                                                                     std::move (gain),
                                                                     std::move (q),
                                                                     std::move (shape),
                                                                     std::move (bypass),
                                                                     std::move (enabled));
        
        parameters.add (std::move (group));
    }
    
    return parameters;
}

std::array<EasyEqAudioProcessor::Band, 8> createBandDefaults()
{
    std::array<EasyEqAudioProcessor::Band, 8> defaults
    {
        EasyEqAudioProcessor::Band (0),
        EasyEqAudioProcessor::Band (1),
        EasyEqAudioProcessor::Band (2),
        EasyEqAudioProcessor::Band (3),
        EasyEqAudioProcessor::Band (4),
        EasyEqAudioProcessor::Band (5),
        EasyEqAudioProcessor::Band (6),
        EasyEqAudioProcessor::Band (7)
    };
    
    return defaults;
}

//==============================================================================
EasyEqAudioProcessor::EasyEqAudioProcessor()
: AudioProcessor (BusesProperties()
                  .withInput  ("Input",  AudioChannelSet::mono(), true)
                  .withOutput ("Output", AudioChannelSet::mono(), true)
                  ),
  state (*this, nullptr, JucePlugin_Name, createParameters()), bands (createBandDefaults())
{
    frequencies.resize (1000);
    for (auto i {0}; i < frequencies.size(); ++i)
        frequencies[i] = 20.0 * std::pow (2.0, i / 100.0);
    totalMagnitudes.resize (frequencies.size());
    std::fill (totalMagnitudes.begin(), totalMagnitudes.end(), 1.0f);
    
    createBandDefaults();
    
    for (auto i {0}; i < bands.size(); ++i)
    {
        bands[i].magnitudes.resize (frequencies.size(), 1.0);
        
        const auto bandId = std::to_string (bands[i].bandId);
        state.addParameterListener (ParameterNames::frequency + "_band" + bandId, this);
        state.addParameterListener (ParameterNames::gain + "_band" + bandId, this);
        state.addParameterListener (ParameterNames::q + "_band" + bandId, this);
        state.addParameterListener (ParameterNames::shape + "_band" + bandId, this);
        state.addParameterListener (ParameterNames::bypass + "_band" + bandId, this);
        state.addParameterListener (ParameterNames::enabled + "_band" + bandId, this);
    }
}

//==============================================================================
const String EasyEqAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

//==============================================================================
void EasyEqAudioProcessor::parameterChanged (const String& parameterId, float newValue)
{
    if (currentSampleRate < 1)
        return;
    
    const auto bandId = parameterId.getLastCharacters (1).getIntValue();
    
    if (parameterId.containsIgnoreCase (ParameterNames::bypass))
    {
        bands[bandId].bypassed = newValue > 0.5f ? true : false;
        
        equaliser.setBypassed<0> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (0)));
        equaliser.setBypassed<1> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (1)));
        equaliser.setBypassed<2> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (2)));
        equaliser.setBypassed<3> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (3)));
        equaliser.setBypassed<4> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (4)));
        equaliser.setBypassed<5> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (5)));
        equaliser.setBypassed<6> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (6)));
        equaliser.setBypassed<7> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (7)));
        
        updateFrequencyResponse();
    }
    else if (parameterId.containsIgnoreCase (ParameterNames::enabled))
    {
        bands[bandId].isEnabled = newValue > 0.5f ? true : false;
        
        equaliser.setBypassed<0> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (0)));
        equaliser.setBypassed<1> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (1)));
        equaliser.setBypassed<2> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (2)));
        equaliser.setBypassed<3> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (3)));
        equaliser.setBypassed<4> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (4)));
        equaliser.setBypassed<5> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (5)));
        equaliser.setBypassed<6> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (6)));
        equaliser.setBypassed<7> (*state.getRawParameterValue (ParameterNames::bypass + "_band" + std::to_string (7)));
        
        updateFrequencyResponse();
    }
    else
    {
        if (! isPositiveAndBelow (bandId, bands.size()))
            return;
        
        updateBand (bandId);
    }
}

void EasyEqAudioProcessor::updateBand (const int bandId)
{
    dsp::IIR::Coefficients<float>::Ptr newCoeffs;
    
    const auto bandNumber = std::to_string (bandId);
    const auto frequency = state.getRawParameterValue (ParameterNames::frequency + "_band" + bandNumber)->load();
    const auto gain = Decibels::decibelsToGain (state.getRawParameterValue (ParameterNames::gain + "_band" + bandNumber)->load());
    const auto q = state.getRawParameterValue (ParameterNames::q + "_band" + bandNumber)->load();
    const auto shape = state.getRawParameterValue (ParameterNames::shape + "_band" + bandNumber)->load();
    
    switch (static_cast<int> (shape))
    {
        case FilterShapes::Bell:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makePeakFilter (currentSampleRate, frequency, q, gain);
            break;
        }
        case FilterShapes::LowCut:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeHighPass (currentSampleRate, frequency, q);
            break;
        }
        case FilterShapes::LowShelf:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeLowShelf (currentSampleRate, frequency, q, gain);
            break;
        }
        case FilterShapes::HighShelf:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeHighShelf (currentSampleRate, frequency, q, gain);
            break;
        }
        case FilterShapes::HighCut:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeLowPass (currentSampleRate, frequency, q);
            break;
        }
        case FilterShapes::Notch:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeNotch (currentSampleRate, frequency, q);
            break;
        }
        case FilterShapes::BandPass:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeBandPass (currentSampleRate, frequency, q);
            break;
        }
    }
    
    if (bandId == 0)    *equaliser.get<0>().state = *newCoeffs;
    else if (bandId == 1)    *equaliser.get<1>().state = *newCoeffs;
    else if (bandId == 2)    *equaliser.get<2>().state = *newCoeffs;
    else if (bandId == 3)    *equaliser.get<3>().state = *newCoeffs;
    else if (bandId == 4)    *equaliser.get<4>().state = *newCoeffs;
    else if (bandId == 5)    *equaliser.get<5>().state = *newCoeffs;
    else if (bandId == 6)    *equaliser.get<6>().state = *newCoeffs;
    else if (bandId == 7)    *equaliser.get<7>().state = *newCoeffs;
    
    newCoeffs->getMagnitudeForFrequencyArray (frequencies.data(),
                                              bands[bandId].magnitudes.data(),
                                              frequencies.size(), currentSampleRate);
    
    updateFrequencyResponse();
}

void EasyEqAudioProcessor::updateFrequencyResponse()
{
    std::fill (totalMagnitudes.begin(), totalMagnitudes.end(), 1.0f);
    
    for (auto i {0}; i < bands.size(); ++i)
    {
        if (bands[i].isEnabled && ! bands[i].bypassed)
            FloatVectorOperations::multiply (totalMagnitudes.data(), bands[i].magnitudes.data(), static_cast<int> (totalMagnitudes.size()));
    }
    
    if (getActiveEditor() != nullptr)
        sendChangeMessage();
}

//==============================================================================
void EasyEqAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    for (auto i {0}; i < 8; ++i)
        updateBand (i);
    
    dsp::ProcessSpec spec { sampleRate,
                            static_cast<uint32> (samplesPerBlock),
                            static_cast<uint32> (jmin (getMainBusNumInputChannels(),
                                                       getMainBusNumOutputChannels()))
                          };
    equaliser.prepare (spec);
}

void EasyEqAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    
    dsp::AudioBlock<float> block (buffer);
    dsp::ProcessContextReplacing<float> context (block);
    
    equaliser.process (context);
}

void EasyEqAudioProcessor::releaseResources()
{
    equaliser.reset();
}

bool EasyEqAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() == AudioChannelSet::mono())
    {
        if (layouts.getMainInputChannelSet() == AudioChannelSet::mono())
            return true;
    }
    else if (layouts.getMainOutputChannelSet() == AudioChannelSet::stereo())
    {
        // Uncomment to enable mono->stereo support in the bus layouts
//        if (layouts.getMainInputChannelSet() == AudioChannelSet::mono())
//            return true;
        
        if (layouts.getMainInputChannelSet() == AudioChannelSet::stereo())
            return true;
    }
    
    return false;
}

//==============================================================================
bool EasyEqAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* EasyEqAudioProcessor::createEditor()
{
    return new EasyEqAudioProcessorEditor (*this, state);
}

//==============================================================================
void EasyEqAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    if (auto xml = std::unique_ptr<XmlElement> (state.state.createXml()))
        copyXmlToBinary (*xml, destData);
}

void EasyEqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
//    if (auto xml = std::unique_ptr<XmlElement> (getXmlFromBinary (data, sizeInBytes)))
//        if (xml->hasTagName (state.state.getType()))
//            state.replaceState (ValueTree::fromXml (*xml));
}

bool EasyEqAudioProcessor::acceptsMidi() const
{
    return false;
}

bool EasyEqAudioProcessor::producesMidi() const
{
    return false;
}

bool EasyEqAudioProcessor::isMidiEffect() const
{
    return false;
}

double EasyEqAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EasyEqAudioProcessor::getNumPrograms()
{
    return 1;
}

int EasyEqAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EasyEqAudioProcessor::setCurrentProgram (int index)
{
}

const String EasyEqAudioProcessor::getProgramName (int index)
{
    return {};
}

void EasyEqAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EasyEqAudioProcessor();
}
