#include "EasyEqLookAndFeel.h"
//==============================================================================
EasyEqLookAndFeel::EasyEqLookAndFeel()
{
    setColour (DocumentWindow::backgroundColourId, Colour::fromRGB (44, 44, 44));
    setColour (Slider::rotarySliderOutlineColourId, Colours::black.brighter (0.25f));
    setColour (Slider::rotarySliderFillColourId, Colours::white.darker (0.1f));
    setColour (ComboBox::backgroundColourId, Colours::black.brighter (0.25f));
}

//==============================================================================
void EasyEqLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                       float sliderPos,
                                       float minSliderPos,
                                       float maxSliderPos,
                                       const Slider::SliderStyle style, Slider& slider)
{
    const auto bounds = Rectangle<int> { x, y, width, height };
    
    g.setColour (Colours::white);
    g.drawText (String (slider.getValue()), bounds, Justification::centred);
}

void EasyEqLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                                          const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour (Slider::rotarySliderFillColourId);
    
    auto bounds = Rectangle<int> (x, y, width, height).toFloat().reduced (10);
    
    auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const auto lineW = 3.0f;
    auto arcRadius = radius - lineW * 0.5f;
    
    Path backgroundArc;
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);
    
    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));
    
    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);
        
        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));
    }
    
    g.setColour (Colours::white);
    g.drawText (String (slider.getValue()), bounds.reduced (lineW), Justification::centred);
}

//==============================================================================
void EasyEqLookAndFeel::drawComboBox (Graphics& g, int width, int height, bool,
                                      int, int, int, int, ComboBox& box)
{
    auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    Rectangle<int> boxBounds (0, 0, width, height);
    
    g.setColour (box.findColour (ComboBox::backgroundColourId));
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);
    
    Rectangle<float> arrowZone (width - 15, height * 0.4f, 10, height * 0.2f);
    Path path;
    path.startNewSubPath (arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
    path.addTriangle (arrowZone.getTopLeft(),
                      { arrowZone.getCentreX(), arrowZone.getBottom() },
                      arrowZone.getTopRight());
    
    g.setColour (box.findColour (ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
    g.fillPath (path);
}
