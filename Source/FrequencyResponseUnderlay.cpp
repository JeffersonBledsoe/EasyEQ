#include "JuceHeader.h"
#include "FrequencyResponseUnderlay.h"

//==============================================================================
FrequencyResponseUnderlay::FrequencyResponseUnderlay()
{
    setInterceptsMouseClicks (false, false);
}

void FrequencyResponseUnderlay::paint (Graphics& g)
{
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId).brighter());
    g.fillRect (getLocalBounds());
}

void FrequencyResponseUnderlay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
