#define KENGINE_WIN32
#define KENGINE_HEADLESS
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#define TIMER_HANDLE 1234
#define TIMER_FREQUENCY 50

typedef struct app_state
{
    memory_arena Arena;
} app_state;

void
InitApp(app_memory *AppMemory)
{
    AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    
    SetTimer(AppMemory->Window, TIMER_HANDLE, TIMER_FREQUENCY, 0);
}

LRESULT
MainWindowCallback(app_memory *AppMemory, HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    UNREFERENCED_PARAMETER(AppMemory);
    
    LRESULT Result = 0;
    
    if(Message == WM_TIMER)
    {
        if(WParam == TIMER_HANDLE)
        {
            KillTimer(AppMemory->Window, TIMER_HANDLE);
            
            s32 MessageResult = MessageBoxA(AppMemory->Window, "Hello, world!", "Kengine", MB_OK);
            
            PostQuitMessage(MessageResult);
        }
    }
    else
    {
        Result = DefWindowProcA(Window, Message, WParam, LParam);
    }
    
    return Result;
}
