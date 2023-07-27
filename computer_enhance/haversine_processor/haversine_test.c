#define KENGINE_TEST
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "kengine_profiler.c"

#include "haversine.h"
#include "haversine.c"

internal void
RunParseJsonPointsTests(memory_arena *Arena)
{
    string Json = String(""
    "{"
        "\"pairs\":"
            "["
                "{ \"x0\": 29.734523, \"y0\": 85.771987, \"x1\": 28.578426, \"y1\": 81.358619 },"
                "{ \"x0\": 39.792438, \"y0\": 33.789546, \"x1\": 38.385242, \"y1\": 41.918724 }"
            "]"
    "}");

    point *Actual = ParseJsonPoints(Arena, Json);

    AssertEqualF32(29.734523f, Actual->X0);
    AssertEqualF32(85.771987f, Actual->Y0);
    AssertEqualF32(28.578426f, Actual->X1);
    AssertEqualF32(81.358619f, Actual->Y1);
    
    Actual = Actual->Next;
    AssertEqualF32(39.792438f, Actual->X0);
    AssertEqualF32(33.789546f, Actual->Y0);
    AssertEqualF32(38.385242f, Actual->X1);
    AssertEqualF32(41.918724f, Actual->Y1);
}

void
RunAllTests(memory_arena *Arena)
{
    RunParseJsonPointsTests(Arena);
}