#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
using FilterShape = EasyEqAudioProcessor::FilterShape;

//==============================================================================
String gainToFloat (float value)
{
    auto val = String (Decibels::gainToDecibels (value));
    
    return val.upToFirstOccurrenceOf (".", true, true) + val.fromFirstOccurrenceOf (".", false, true).substring (0, 2);
}

String getFilterShapeNameForId (int id)
{
    switch (id)
    {
        case FilterShape::NoFilter:        return "No Filter";
        case FilterShape::Bell:            return "Bell";
        case FilterShape::LowCut:          return "Low Cut";
        case FilterShape::LowShelf:        return "Low Shelf";
        case FilterShape::HighShelf:       return "High Shelf";
        case FilterShape::HighCut:         return "High Cut";
        case FilterShape::Notch:           return "Notch";
        case FilterShape::BandPass:        return "Band Pass";
        default: break;
    }
    
    return {};
}

StringArray getFilterShapeNames()
{
    StringArray shapes;
    
    for (auto i {0}; i < FilterShape::numOfShapes; ++i)
        shapes.add (getFilterShapeNameForId (i));
    
    return shapes;
}

AudioProcessorValueTreeState::ParameterLayout createParameters()
{
    AudioProcessorValueTreeState::ParameterLayout parameters;
    
    for (auto i {0}; i < 8; ++i)
    {
        auto frequency = std::make_unique<AudioParameterFloat> ("frequency_band" + std::to_string (i), "Frequency",
                                                                NormalisableRange<float> (20.0f, 20000.0f), 1000.0f,
                                                                "Hz", AudioProcessorParameter::genericParameter,
                                                                [] (float value, int) { return String (value, 0).substring (0, 6).trimCharactersAtEnd ("."); },
                                                                [] (String text) { return text.substring (0, 6).getFloatValue(); });
        auto gain = std::make_unique<AudioParameterFloat> ("gain_band" + std::to_string (i), "Gain",
                                                           NormalisableRange<float> (0.0001f, 1.0f), 0.501f,
                                                           "dB", AudioProcessorParameter::genericParameter,
                                                           [] (float value, int) { return gainToFloat (value); },
                                                           [] (const String& text) { return Decibels::decibelsToGain (text.getFloatValue()); });
        auto q = std::make_unique<AudioParameterFloat> ("q_band" + std::to_string (i), "Q",
                                                        NormalisableRange<float> (0.05f, 30.0f), 0.707f,
                                                        String(), AudioProcessorParameter::genericParameter);
        auto shape = std::make_unique<AudioParameterChoice> ("shape_band" + std::to_string (i), "Shape",
                                                             getFilterShapeNames(), 1);
        auto bypass = std::make_unique<AudioParameterBool> ("bypass_band" + std::to_string (i), "Bypass",
                                                            false, String(),
                                                            [] (bool bypassed, int) { return bypassed ? "Bypassed" : "Active"; });
        
        auto group = std::make_unique<AudioProcessorParameterGroup> ("band" + std::to_string (i), "Band" + std::to_string (i),
                                                                     "|",
                                                                     std::move (frequency),
                                                                     std::move (gain),
                                                                     std::move (q),
                                                                     std::move (shape),
                                                                     std::move (bypass));
        
        parameters.add (std::move (group));
    }
    
    return parameters;
}

//==============================================================================
EasyEqAudioProcessor::EasyEqAudioProcessor()
: AudioProcessor (BusesProperties()
                  .withInput  ("Input",  AudioChannelSet::mono(), true)
                  .withOutput ("Output", AudioChannelSet::mono(), true)
                  ),
  state (*this, nullptr, "STATE", createParameters())
//         {
//           std::make_unique<AudioParameterFloat> ("frequency", "Frequency",
//                                                  NormalisableRange<float> (20.0f, 20000.0f), 1000.0f,
//                                                  "Hz", AudioProcessorParameter::genericParameter,
//                                                  [] (float value, int) { return String (value, 0).substring (0, 6).trimCharactersAtEnd ("."); },
//                                                  [] (String text) { return text.substring (0, 6).getFloatValue(); }),
//           std::make_unique<AudioParameterFloat> ("gain", "Gain",
//                                                  NormalisableRange<float> (0.0001f, 1.0f), 0.501f,
//                                                  "dB", AudioProcessorParameter::genericParameter,
//                                                  [] (float value, int) { return gainToFloat (value); },
//                                                  [] (const String& text) { return Decibels::decibelsToGain (text.getFloatValue()); }),
//           std::make_unique<AudioParameterFloat> ("q", "Q",
//                                                  NormalisableRange<float> (0.05f, 30.0f), 0.707f,
//                                                  String(), AudioProcessorParameter::genericParameter),
//           std::make_unique<AudioParameterChoice> ("shape", "Shape",
//                                                   getFilterShapeNames(), 1),
//           std::make_unique<AudioParameterBool> ("bypass", "Bypass",
//                                                 false, String(),
//                                                 [] (bool bypassed, int) { return bypassed ? "Bypassed" : "Active"; })
//         })
{
    state.addParameterListener ("frequency", this);
    state.addParameterListener ("gain", this);
    state.addParameterListener ("q", this);
    state.addParameterListener ("bypass", this);
    state.addParameterListener ("shape", this);
}

//==============================================================================
const String EasyEqAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

//==============================================================================
void EasyEqAudioProcessor::parameterChanged (const String& parameterId, float newValue)
{
    const auto bandId = static_cast<int> (parameterId.getLastCharacter());
    
//    if (parameterId.containsIgnoreCase ("bypass"))
//    {
//        equaliser.setBypassed<bandId> (newValue > 0.5 ? true : false);
//    }
    
    updateBand (bandId);
}

void EasyEqAudioProcessor::updateBand (int bandId)
{
    if (currentSampleRate < 1)
        return;
    
    dsp::IIR::Coefficients<float>::Ptr newCoeffs;
    
    if (! isPositiveAndBelow (bandId, 8))
        return;
    
    const auto frequency = *state.getRawParameterValue ("frequency_band" + std::to_string (bandId));
    const auto gain = *state.getRawParameterValue ("gain_band" + std::to_string (bandId));
    const auto q = *state.getRawParameterValue ("q_band" + std::to_string (bandId));
    const auto shape = *state.getRawParameterValue ("shape_band" + std::to_string (bandId));
    
    switch (static_cast<int> (shape))
    {
        case FilterShape::NoFilter:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeAllPass (currentSampleRate, frequency);
            break;
        }
        case FilterShape::Bell:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makePeakFilter (currentSampleRate, frequency, q, gain);
            break;
        }
        case FilterShape::LowCut:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeHighPass (currentSampleRate, frequency, q);
            break;
        }
        case FilterShape::LowShelf:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeLowShelf (currentSampleRate, frequency, q, gain);
            break;
        }
        case FilterShape::HighShelf:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeHighShelf (currentSampleRate, frequency, q, gain);
            break;
        }
        case FilterShape::HighCut:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeLowPass (currentSampleRate, frequency, q);
            break;
        }
        case FilterShape::Notch:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeNotch (currentSampleRate, frequency, q);
            break;
        }
        case FilterShape::BandPass:
        {
            newCoeffs = dsp::IIR::Coefficients<float>::makeBandPass (currentSampleRate, frequency, q);
            break;
        }
    }
    
    auto& band1 = equaliser.get<0>();
    *band1.state = *newCoeffs;
}

//==============================================================================
void EasyEqAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    for (auto i {0}; i < 8; ++i)
        updateBand (i);
    
    dsp::ProcessSpec spec { sampleRate,
                            static_cast<uint32> (samplesPerBlock),
                            static_cast<uint32> (jmin (getTotalNumInputChannels(),
                                                       getTotalNumOutputChannels()))
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
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
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
    if (auto xml = std::unique_ptr<XmlElement> (getXmlFromBinary (data, sizeInBytes)))
        if (xml->hasTagName (state.state.getType()))
            state.replaceState (ValueTree::fromXml (*xml));
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
