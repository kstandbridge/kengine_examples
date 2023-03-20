#define KENGINE_WIN32
#define KENGINE_WINDOW
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
    
    HINSTANCE Instance = (HINSTANCE)GetWindowLongPtrA(AppMemory->Window, GWLP_HINSTANCE);
    
    DWORD Style = (WS_CHILD | WS_VISIBLE);
    
    CreateWindowExA(0, WC_STATIC, "Hello, world!", Style, 0, 0, 640, 480, AppMemory->Window, 0, Instance, 0);
}

LRESULT
MainWindowCallback(app_memory *AppMemory, HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    UNREFERENCED_PARAMETER(AppMemory);
    
    LRESULT Result = DefWindowProcA(Window, Message, WParam, LParam);
    
    return Result;
}
