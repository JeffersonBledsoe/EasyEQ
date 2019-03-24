#include "JuceHeader.h"
#include "ControlPanelBand.h"
#include "Utilities.h"

std::unique_ptr<Drawable> getIconForId (const int id)
{
    switch (id)
    {
        case FilterShapes::Bell:            return std::unique_ptr<Drawable> (Drawable::createFromImageData (BinaryData::Bell_svg,
                                                                                                             BinaryData::Bell_svgSize));
        case FilterShapes::LowCut:          return std::unique_ptr<Drawable> (Drawable::createFromImageData (BinaryData::LowCut_svg,
                                                                                                             BinaryData::LowCut_svgSize));
        case FilterShapes::LowShelf:        return std::unique_ptr<Drawable> (Drawable::createFromImageData (BinaryData::LowShelf_svg,
                                                                                                             BinaryData::LowShelf_svgSize));
        case FilterShapes::HighShelf:       return std::unique_ptr<Drawable> (Drawable::createFromImageData (BinaryData::HighShelf_svg,
                                                                                                             BinaryData::HighShelf_svgSize));
        case FilterShapes::HighCut:         return std::unique_ptr<Drawable> (Drawable::createFromImageData (BinaryData::HighCut_svg,
                                                                                                             BinaryData::HighCut_svgSize));
        case FilterShapes::Notch:           return std::unique_ptr<Drawable> (Drawable::createFromImageData (BinaryData::Notch_svg,
                                                                                                             BinaryData::Notch_svgSize));
        case FilterShapes::BandPass:        return std::unique_ptr<Drawable> (Drawable::createFromImageData (BinaryData::BandPass_svg,
                                                                                                             BinaryData::BandPass_svgSize));
        default: break;
    }
    
    return nullptr;
}

//==============================================================================
using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;

class ShapeSelectPopupItem : public PopupMenu::CustomComponent
{
public:
    ShapeSelectPopupItem (const int itemId, const int bandId)
    : itemText (FilterShapes::getFilterShapeNameForId (itemId)),
      icon (getIconForId (itemId))
    {
        shapeLabel.setText (itemText, dontSendNotification);
        icon->replaceColour (Colours::black, BandColours::getColourForBand (bandId));
        
        addAndMakeVisible (shapeLabel);
        addAndMakeVisible (icon.get());
    }
    
    void getIdealSize (int& idealWidth, int& idealHeight) override
    {
        getLookAndFeel().getIdealPopupMenuItemSize (itemText,
                                                    false,
                                                    30, idealWidth, idealHeight);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().toFloat();
        
        icon->setTransformToFit (bounds.removeFromLeft (proportionOfWidth (0.2f)).reduced (3), RectanglePlacement::stretchToFit);
        
        shapeLabel.setBounds (bounds.toNearestInt());
    }
    
private:
    StringRef itemText;
    Label shapeLabel;
    std::unique_ptr<Drawable> icon;
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
    bandName.setColour (Label::textColourId, BandColours::getColourForBand (bandId));
    frequencySlider.setSliderSnapsToMousePosition (false);
    frequencySlider.setColour (Slider::trackColourId, BandColours::getColourForBand (bandId));
    gainSlider.setSliderSnapsToMousePosition (false);
    gainSlider.setColour (Slider::trackColourId, BandColours::getColourForBand (bandId));
    qSlider.setSliderSnapsToMousePosition (false);
    qSlider.setColour (Slider::trackColourId, BandColours::getColourForBand (bandId));
    shapeSelector.setText ("No band selected");
    for (auto i {0}; i < shapeParam->choices.size(); ++i)
    {
        PopupMenu::Item item;
        item.text = shapeParam->choices[i];
        item.itemID = i + 1;
        item.customComponent = new ShapeSelectPopupItem (i, bandId);
        shapeSelector.getRootMenu()->addItem (item);
    }
    bypassButton.setColour (ToggleButton::textColourId, BandColours::getColourForBand (bandId));
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
