#include "JuceHeader.h"
#include "ControlPanelBand.h"
#include "Utilities.h"

//==============================================================================
using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

class ShapeSelectPopupItem : public PopupMenu::CustomComponent
{
public:
    ShapeSelectPopupItem (StringRef itemLabel)
    : itemText (itemLabel)
    {
        shapeLabel.setText (itemText, dontSendNotification);
        addAndMakeVisible (shapeLabel);
    }
    
    void getIdealSize (int& idealWidth, int& idealHeight) override
    {
        getLookAndFeel().getIdealPopupMenuItemSize (itemText,
                                                    false,
                                                    30, idealWidth, idealHeight);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        shapeLabel.setBounds (bounds);
    }
    
private:
    Label shapeLabel;
    StringRef itemText;
};

//==============================================================================
ControlPanelBand::ControlPanelBand (const int id, AudioProcessorValueTreeState& s)
: bandId (id), state (s)
{
    const auto bandIdString = String (bandId);
    setName ("Control_Panel_Band_" + String (bandId));
    
    shapeParam = dynamic_cast<AudioParameterChoice*> (state.getParameter (ParameterNames::shape + "_band" + bandIdString));
    bypassParam = dynamic_cast<AudioParameterBool*> (state.getParameter (ParameterNames::bypass + "_band" + bandIdString));
    enabledParam = dynamic_cast<AudioParameterBool*> (state.getParameter (ParameterNames::enabled + "_band" + bandIdString));
    state.addParameterListener ("shape_band" + bandIdString, this);
    state.addParameterListener ("bypass_band" + bandIdString, this);
    state.addParameterListener ("enabled_band" + bandIdString, this);
    
    bandName.setText ("Band " + bandIdString, dontSendNotification);
    bandName.setJustificationType (Justification::centred);
    frequencySlider.setSliderSnapsToMousePosition (false);
    gainSlider.setSliderSnapsToMousePosition (false);
    qSlider.setSliderSnapsToMousePosition (false);
    shapeSelector.setText ("No band selected");
    
    for (auto i {0}; i < shapeParam->choices.size(); ++i)
    {
        PopupMenu::Item item;
        item.text = shapeParam->choices[i];
        item.itemID = i + 1;
        item.customComponent = new ShapeSelectPopupItem (shapeParam->choices[i]);;
        shapeSelector.getRootMenu()->addItem (item);
    }
    
    bypassButton.setButtonText ("Bypass");
    
    frequencyAttachment.reset (new SliderAttachment (state, "frequency_band" + bandIdString, frequencySlider));
    gainAttachment.reset (new SliderAttachment (state, "gain_band" + bandIdString, gainSlider));
    qAttachment = std::make_unique<SliderAttachment> (state, "q_band" + bandIdString, qSlider);
    bypassAttachment = std::make_unique<ButtonAttachment> (state, "bypass_band" + bandIdString, bypassButton);
    shapeAttachment = std::make_unique<ComboBoxAttachment> (state, "shape_band" + bandIdString, shapeSelector);
    
    addAndMakeVisible (bandName);
    addAndMakeVisible (shapeSelector);
    addAndMakeVisible (frequencySlider);
    addAndMakeVisible (gainSlider);
    addAndMakeVisible (qSlider);
    addAndMakeVisible (bypassButton);
}

ControlPanelBand::~ControlPanelBand()
{
    state.removeParameterListener ("shape_band" + std::to_string (bandId), this);
}

//==============================================================================
void ControlPanelBand::paint (Graphics& g)
{
    g.setColour (BandColours::getColourForBand (bandId));
    g.fillRect (getLocalBounds());
}

void ControlPanelBand::resized()
{
    Grid grid;
    using Track = Grid::TrackInfo;
    grid.templateColumns = { Track (1_fr) };
    grid.autoRows = Track (1_fr);
    
    grid.autoFlow = Grid::AutoFlow::row;
    
    for (auto* child : getChildren())
        grid.items.add (*child);
    
    grid.performLayout (getLocalBounds());
}

//==============================================================================
void ControlPanelBand::parameterChanged (const String& parameterId, float newValue)
{
    DBG ("Parameter Of Changed Control Panel Band: " + parameterId);
    DBG (newValue);
    if (MessageManager::getInstance()->isThisTheMessageThread())
        update();
    else
        MessageManager::callAsync ([this] { update(); });
}

void ControlPanelBand::update()
{
    const auto shapeParam = dynamic_cast<AudioParameterChoice*> (state.getParameter (ParameterNames::shape + "_band" + String (bandId)));
    
    if (*shapeParam == FilterShapes::Bell)              gainSlider.setEnabled (true);
    else if (*shapeParam == FilterShapes::LowShelf)     gainSlider.setEnabled (true);
    else if (*shapeParam == FilterShapes::HighShelf)    gainSlider.setEnabled (true);
    else
        gainSlider.setEnabled (false);
    
    if (*bypassParam == true)
        setAlpha (0.3f);
    else
        setAlpha (1.0f);
    
    if (*enabledParam == true)
        setVisible (true);
    else
        setVisible (false);
}
