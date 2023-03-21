#define KENGINE_WIN32
#define KENGINE_TEST
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

void
RunAllTests(memory_arena *Arena)
{
    AssertTrue(Arena);
    
    {
        string Expected = String("Hello,");
        string Actual = String("world!");
        AssertEqualString(Expected, Actual);
    }
    
}
