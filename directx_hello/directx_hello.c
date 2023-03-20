#define KENGINE_WIN32
#define KENGINE_DIRECTX
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

typedef struct app_state
{
    memory_arena Arena;
} app_state;

void
InitApp(app_memory *AppMemory)
{
    AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
}

LRESULT
MainWindowCallback(app_memory *AppMemory, HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    UNREFERENCED_PARAMETER(AppMemory);
    
    LRESULT Result = DefWindowProcA(Window, Message, WParam, LParam);
    
    return Result;
}
