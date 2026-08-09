#define REAPERAPI_IMPLEMENT
#include <reaper_plugin_functions.h>
//
#include "MainWindow.h"
#include <juce_core/juce_core.h>

struct State
{
    State()
    {
        juce::initialiseJuce_GUI();

        auto* messageManager = juce::MessageManager::getInstanceWithoutCreating();
        messageManager->setCurrentThreadAsMessageThread();

        mainWindow = new MainWindow("Hello World Demo", new HelloWorldDemo());
    }

    ~State()
    {
        delete mainWindow;

        juce::shutdownJuce_GUI();
    }

    MainWindow* mainWindow = nullptr;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(State)
};

State* state = nullptr;

int command_id = 0;
custom_action_register_t custom_action =
    { //
        0,
        "AK5K_HelloWorldDemo",
        "Hello World Demo",
        nullptr
};

bool OnAction(KbdSectionInfo* sec, int command, int val, int valhw, int relmode, HWND hwnd)
{
    if (command != command_id)
        return false;

    state->mainWindow->setVisible(!state->mainWindow->isVisible());

    return true;
}

void messagePump()
{
    juce::MessageManager::getInstanceWithoutCreating()->runDispatchLoopUntil(1);
}

extern "C"
{
    REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t* rec)
    {
        if (rec != nullptr && REAPERAPI_LoadAPI(rec->GetFunc) == 0)
        {
            state = new State();

#if JUCE_LINUX
            plugin_register("timer", (void*)messagePump);
#endif

            command_id = plugin_register("custom_action", (void*)&custom_action);

            plugin_register("hookcommand2", (void*)OnAction);

            return 1;
        }
        else
        {
#if JUCE_LINUX
            plugin_register("-timer", (void*)messagePump);
#endif
            plugin_register("-hookcommand2", (void*)OnAction);
            plugin_register("-custom_action", (void*)&custom_action);
            command_id = 0;

            delete state;

            return 0;
        }
    }
};
