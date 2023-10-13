#define _GNU_SOURCE

#define KENGINE_TEST
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

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
    
    {
        json_element *DocElement = ParseJsonDocument(Arena, Json, String("/dev/null"));
        AssertTrue(DocElement);
        json_element *PairsElement = GetJsonElement(DocElement, String("pairs"));
        AssertTrue(PairsElement);
        AssertEqualU32(JsonElement_Array, PairsElement->Type);
        
        json_element *PairElement = PairsElement->Children;
        AssertEqualU32(JsonElement_Object, PairElement->Type);

        json_element *X0Element = GetJsonElement(PairElement, String("x0"));
        json_element *Y0Element = GetJsonElement(PairElement, String("y0"));
        json_element *X1Element = GetJsonElement(PairElement, String("x1"));
        json_element *Y1Element = GetJsonElement(PairElement, String("y1"));
            
        point Point = 
        {
            .X0 = F64FromString(X0Element->Value),
            .Y0 = F64FromString(Y0Element->Value),
            .X1 = F64FromString(X1Element->Value),
            .Y1 = F64FromString(Y1Element->Value)
        };
        AssertEqualF32(29.734523f, Point.X0);
        AssertEqualF32(85.771987f, Point.Y0);
        AssertEqualF32(28.578426f, Point.X1);
        AssertEqualF32(81.358619f, Point.Y1);

        PairElement = PairElement->Next;
        AssertEqualU32(JsonElement_Object, PairElement->Type);

        X0Element = GetJsonElement(PairElement, String("x0"));
        Y0Element = GetJsonElement(PairElement, String("y0"));
        X1Element = GetJsonElement(PairElement, String("x1"));
        Y1Element = GetJsonElement(PairElement, String("y1"));
            
        Point.X0 = F64FromString(X0Element->Value);
        Point.Y0 = F64FromString(Y0Element->Value);
        Point.X1 = F64FromString(X1Element->Value);
        Point.Y1 = F64FromString(Y1Element->Value);
        
        AssertEqualF32(39.792438f, Point.X0);
        AssertEqualF32(33.789546f, Point.Y0);
        AssertEqualF32(38.385242f, Point.X1);
        AssertEqualF32(41.918724f, Point.Y1);
    }
    
    {
        point *Points = ParseJsonPoints(Arena, Json, String("/dev/null"));
        AssertEqualF32(29.734523f, Points->X0);
        AssertEqualF32(85.771987f, Points->Y0);
        AssertEqualF32(28.578426f, Points->X1);
        AssertEqualF32(81.358619f, Points->Y1);

        AssertTrue(Points->Next);
        Points = Points->Next;

        AssertEqualF32(39.792438f, Points->X0);
        AssertEqualF32(33.789546f, Points->Y0);
        AssertEqualF32(38.385242f, Points->X1);
        AssertEqualF32(41.918724f, Points->Y1);
    }
}

void
RunAllTests(memory_arena *Arena)
{
    RunParseJsonPointsTests(Arena);
}