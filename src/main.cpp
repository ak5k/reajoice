#include <WDL/wdltypes.h>
#define REAPERAPI_IMPLEMENT
#include <reaper_plugin_functions.h>
//
#include "MainWindow.h"
MainWindow* mainWindow = nullptr;

void messagePump()
{
    juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
}

extern "C"
{
    REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t* rec)
    {
        if (rec)
        {
            if (rec->caller_version != REAPER_PLUGIN_VERSION || !rec->GetFunc)
                return 0;

            juce::MessageManager::getInstance()->setCurrentThreadAsMessageThread();
            rec->Register("timer", (void*)messagePump);
            mainWindow = new MainWindow("Hello World Demo", new HelloWorldDemo());

            return 1;
        }
        else
        {
            if (mainWindow)
            {
                delete mainWindow;
                mainWindow = nullptr;
            }

            juce::shutdownJuce_GUI();
            return 0;
        }
    }
};