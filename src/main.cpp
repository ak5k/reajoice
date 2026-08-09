#include <WDL/wdltypes.h>
#define REAPERAPI_IMPLEMENT
#include <reaper_plugin_functions.h>
//
#include "MainWindow.h"

MainWindow* mainWindow = nullptr;

int command_id = 0;
custom_action_register_t custom_action =
    { //
        0,
        "AK5K_HelloWorldDemo",
        "Hello World Demo",
        nullptr
};

void messagePump();

bool OnAction(KbdSectionInfo* sec, int command, int val, int valhw, int relmode, HWND hwnd)
{
    if (command != command_id)
        return false;

    if (!mainWindow)
        return true;

    mainWindow->setVisible(!mainWindow->isVisible());

    return true;
}

void messagePump()
{
    if (mainWindow == nullptr)
        return;

    auto* messageManager = juce::MessageManager::getInstanceWithoutCreating();

    if (messageManager == nullptr)
        return;

    if (!messageManager->isThisTheMessageThread())
    {
        jassertfalse;
        return;
    }

    messageManager->runDispatchLoopUntil(2);
}

extern "C"
{
    REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t* rec)
    {
        if (rec != nullptr && REAPERAPI_LoadAPI(rec->GetFunc) == 0)
        {
            juce::initialiseJuce_GUI();

            auto* messageManager = juce::MessageManager::getInstanceWithoutCreating();

            if (messageManager == nullptr)
            {
                plugin_register("-timer", (void*)messagePump);
                plugin_register("-hookcommand2", (void*)OnAction);
                plugin_register("-custom_action", (void*)&custom_action);
                command_id = 0;

                if (mainWindow != nullptr)
                {
                    delete mainWindow;
                    mainWindow = nullptr;
                }

                juce::shutdownJuce_GUI();
                return 0;
            }

            messageManager->setCurrentThreadAsMessageThread();

            plugin_register("timer", (void*)messagePump);

            mainWindow = new MainWindow("Hello World Demo", new HelloWorldDemo());

            command_id = plugin_register("custom_action", (void*)&custom_action);

            if (command_id == 0)
            {
                plugin_register("-timer", (void*)messagePump);
                plugin_register("-hookcommand2", (void*)OnAction);
                plugin_register("-custom_action", (void*)&custom_action);
                command_id = 0;

                if (mainWindow != nullptr)
                {
                    delete mainWindow;
                    mainWindow = nullptr;
                }

                juce::shutdownJuce_GUI();
                return 0;
            }

            plugin_register("hookcommand2", (void*)OnAction);

            return 1;
        }
        else
        {
            plugin_register("-timer", (void*)messagePump);
            plugin_register("-hookcommand2", (void*)OnAction);
            plugin_register("-custom_action", (void*)&custom_action);
            command_id = 0;

            if (mainWindow != nullptr)
            {
                delete mainWindow;
                mainWindow = nullptr;
            }

            juce::shutdownJuce_GUI();
            return 0;
        }
    }
};
