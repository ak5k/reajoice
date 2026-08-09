#include "HelloWorldDemo.h"

#include <juce_audio_utils/juce_audio_utils.h>

class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow(const juce::String& name, juce::Component* c)
        : DocumentWindow(
              name,
              juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
              juce::DocumentWindow::allButtons
          )
    {
        // Juce boilerplate code to set up the main window
        setUsingNativeTitleBar(true);
        setContentOwned(c, true);

        setResizable(true, false);
        setResizeLimits(300, 250, 10000, 10000);
        centreWithSize(getWidth(), getHeight());

        // setVisible(true);

        juce::Timer::callAfterDelay(
            1000,
            [this]()
            {
                // This is just an example of how to use a timer to do something after a delay
                // You can remove this if you don't need it
                setVisible(true);
                this->toFront(false);

                juce::Logger::writeToLog("MainWindow is now visible!");

                DBG("MainWindow is now visible!");
            }
        );
    }

    void closeButtonPressed() override
    {
        setVisible(false);
        // or shutdown/delete/free etc
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};