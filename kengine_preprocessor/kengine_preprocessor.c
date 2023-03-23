#define KENGINE_WIN32
#define KENGINE_PREPROCESSOR
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

void
GenerateCodeFor(memory_arena *Arena, c_struct Struct, string_list *Options)
{
    Arena;
    
    for(string_list *Option = Options;
        Option;
        Option = Option->Next)
    {
        PlatformConsoleOut("\n#error %S option %S", Struct.Name, Option->Entry);
    }
}