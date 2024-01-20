#define _GNU_SOURCE

// #define KENGINE_PROFILER
#define KENGINE_TEST
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "kengine_test.h"

void
RunAllTests(platform_work_queue *WorkQueue)
{
    Win32AddWorkEntry(WorkQueue, RunGetBitsTests, 0);
    Win32AddWorkEntry(WorkQueue, RunUnpackTests, 0);

    Win32AddWorkEntry(WorkQueue, RunFormatStringHexTests, 0);
    Win32AddWorkEntry(WorkQueue, RunStringsAreEqualTests, 0);
    Win32AddWorkEntry(WorkQueue, RunStringBeginsWithTests, 0);
    Win32AddWorkEntry(WorkQueue, RunStringContainsTests, 0);
    Win32AddWorkEntry(WorkQueue, RunFormatStringSignedDecimalIntegerTests, 0);
    Win32AddWorkEntry(WorkQueue, RunFormatStringUnsignedDecimalIntegerTests, 0);
    Win32AddWorkEntry(WorkQueue, RunFormatStringDecimalFloatingPoint, 0);
    Win32AddWorkEntry(WorkQueue, RunFormatStringStringOfCharactersTests, 0);
    Win32AddWorkEntry(WorkQueue, RunFormatStringStringTypeTests, 0);
    Win32AddWorkEntry(WorkQueue, RunFormatStringPercentTests, 0);
    Win32AddWorkEntry(WorkQueue, RunFormatStringDateTests, 0);
    Win32AddWorkEntry(WorkQueue, RunFormatStringBinaryTests, 0);
    Win32AddWorkEntry(WorkQueue, RunFormatStringWithoutArenaTests, 0);
    
    Win32AddWorkEntry(WorkQueue, RunV2Tests, 0);
    
    Win32AddWorkEntry(WorkQueue, RunUpperCamelCaseTests, 0);
    
    Win32AddWorkEntry(WorkQueue, RunRadixSortTests, 0);
    
    Win32AddWorkEntry(WorkQueue, RunParseFromStringTests, 0);
    
    Win32AddWorkEntry(WorkQueue, RunSha512Tests, 0);
    
    Win32AddWorkEntry(WorkQueue, RunEdDSATests, 0);
    
    Win32AddWorkEntry(WorkQueue, RunLinkedListMergeSortTests, 0);
    Win32AddWorkEntry(WorkQueue, RunNodeGetCountTests, 0);
    Win32AddWorkEntry(WorkQueue, RunGetNodeByIndexTests, 0);
    Win32AddWorkEntry(WorkQueue, RunGetIndexOfNodeTests, 0);
    Win32AddWorkEntry(WorkQueue, RunGetNodeTests, 0);
    
    Win32AddWorkEntry(WorkQueue, RunParseHtmlTest, 0);
    Win32AddWorkEntry(WorkQueue, RunParseXmlTest, 0);
    Win32AddWorkEntry(WorkQueue, ParseJsonTests, 0);

    Win32AddWorkEntry(WorkQueue, RunF32FromStringTests, 0);

}
