#define _GNU_SOURCE

#define KENGINE_TEST
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

void
RunAllTests(memory_arena *Arena)
{
    AssertTrue(Arena);
    
    string Expected = String("Hello, world!");
    string Actual = String("Hello, wOrld!");
    
    AssertEqualString(Expected, Actual);
}
