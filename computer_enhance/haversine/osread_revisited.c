#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include <stdio.h>
#include <string.h>

typedef struct app_state
{
    memory_arena *Arena;
} app_state;

global memory_arena GlobalArena;

u8 *
AllocateMemory(u64 TotalSize)
{
    u8 *Result;

#if KENGINE_WIN32
    Result = (u8 *)VirtualAlloc(0, TotalSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
#elif KENGINE_LINUX
    Result = (u8 *)mmap(0, TotalSize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
#else
#error Unsupported platform
#endif

    return Result;
}

void
FreeMemory(u8 *Memory, u64 TotalSize)
{
#if KENGINE_WIN32
    VirtualFree(Memory, 0, MEM_RELEASE);
#elif KENGINE_LINUX
    munmap(Memory, TotalSize);
#else
#error Unsupported platform
#endif
}

typedef void file_process_func(repetition_tester *Tester, char *FileName, u64 TotalFileSize, u64 BufferSize, string Buffer);
typedef struct test_function
{
    char *Name;
    file_process_func *Func;
} test_function;

#define MIN_MEMORY_PAGE_SIZE 4096
internal void
AllocateAndTouch(repetition_tester *Tester, char *FileName, u64 TotalFileSize, u64 BufferSize, string Scratch)
{
    void *Memory = AllocateMemory(BufferSize);

    string Buffer = String_(BufferSize, Memory);
    u64 TouchCount = (Buffer.Size + MIN_MEMORY_PAGE_SIZE - 1)/MIN_MEMORY_PAGE_SIZE;
    for(u64 TouchIndex = 0; TouchIndex < TouchCount; ++TouchIndex)
    {
        Buffer.Data[MIN_MEMORY_PAGE_SIZE*TouchIndex] = 0;
    }

    RepetitionTestCountBytes(Tester, TotalFileSize);

    FreeMemory(Memory, BufferSize);
}

internal void
AllocateAndCopy(repetition_tester *Tester, char *FileName, u64 TotalFileSize, u64 BufferSize, string Scratch)
{
    void *Memory = AllocateMemory(BufferSize);
    
    string Buffer = String_(BufferSize, Memory);

    u8 *Source = Scratch.Data;
    umm SizeRemaining = TotalFileSize;
    while(SizeRemaining)
    {
        u64 ReadSize = Buffer.Size;
        if(ReadSize > SizeRemaining)
        {
            ReadSize = SizeRemaining;
        }

#if 0
        __movsb(Buffer.Data, Source, (u32)ReadSize);
#else
        memcpy(Buffer.Data, Source, (u32)ReadSize);
#endif

        RepetitionTestCountBytes(Tester, ReadSize);

        SizeRemaining -= ReadSize;
        Source += ReadSize;
    }

    FreeMemory(Memory, BufferSize);
}

#if KENGINE_WIN32
internal void
OpenAllocateAndRead(repetition_tester *Tester, char *FileName, u64 TotalFileSize, u64 BufferSize, string Scratch)
{
    HANDLE File = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    void *Memory = AllocateMemory(BufferSize);
    string Buffer = String_(BufferSize, Memory);

    if((File != INVALID_HANDLE_VALUE))
    {
        umm SizeRemaining = TotalFileSize;
        while(SizeRemaining)
        {
            u64 ReadSize = Buffer.Size;
            if(ReadSize > SizeRemaining)
            {
                ReadSize = SizeRemaining;
            }

            DWORD BytesRead = 0;
            BOOL Result = ReadFile(File, Buffer.Data, (u32)ReadSize, &BytesRead, 0);

            if(Result && (BytesRead == ReadSize))
            {
                RepetitionTestCountBytes(Tester, ReadSize);
            }
            else
            {
                RepetitionTestError(Tester, "ReadFile failed");
            }

            SizeRemaining -= ReadSize;
        }
    }

    FreeMemory(Memory, BufferSize);
    CloseHandle(File);
}

#elif KENGINE_LINUX
internal void
OpenAllocateAndRead(repetition_tester *Tester, char *FileName, u64 TotalFileSize, u64 BufferSize, string Scratch)
{
    s32 File = open(FileName, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    void *Memory = AllocateMemory(BufferSize);
    string Buffer = String_(BufferSize, Memory);

    if(File >= 0)
    {
        umm SizeRemaining = TotalFileSize;
        s64 Offset = 0;
        while(SizeRemaining)
        {
            u64 ReadSize = Buffer.Size;
            if(ReadSize > SizeRemaining)
            {
                ReadSize = SizeRemaining;
            }

            ssize_t BytesRead = pread(File, Buffer.Data, ReadSize, Offset);

            if(BytesRead == ReadSize)
            {
                RepetitionTestCountBytes(Tester, ReadSize);
            }
            else
            {
                RepetitionTestError(Tester, "ReadFile failed");
            }

            SizeRemaining -= ReadSize;
            Offset += ReadSize;
        }
    }

    FreeMemory(Memory, BufferSize);
    close(File);
}
#else
#error Unsupported platform
#endif

internal void
OpenAllocateAndFRead(repetition_tester *Tester, char *FileName, u64 TotalFileSize, u64 BufferSize, string Scratch)
{
    FILE *File = fopen(FileName, "rb");
    void *Memory = AllocateMemory(BufferSize);
    string Buffer = String_(BufferSize, Memory);

    if(File)
    {
        umm SizeRemaining = TotalFileSize;
        while(SizeRemaining)
        {
            u64 ReadSize = Buffer.Size;
            if(ReadSize > SizeRemaining)
            {
                ReadSize = SizeRemaining;
            }

            if(fread(Buffer.Data, ReadSize, 1, File) == 1)
            {
                RepetitionTestCountBytes(Tester, ReadSize);
            }
            else
            {
                RepetitionTestError(Tester, "fread failed");
            }

            SizeRemaining -= ReadSize;
        }
    }

    FreeMemory(Memory, BufferSize);
    fclose(File);
}

global test_function TestFunctions[] =
{
    { "AllocateAndTouch", AllocateAndTouch},
    { "AllocateAndCopy", AllocateAndCopy},
    { "OpenAllocateAndRead", OpenAllocateAndRead},
    { "OpenAllocateAndFRead", OpenAllocateAndFRead},
};

s32
MainLoop(app_memory *AppMemory)
{
    GlobalArena.MinimumBlockSize = Gigabytes(2);
    app_state *AppState = AppMemory->AppState = PushStruct(&GlobalArena, app_state);
    memory_arena *Arena = AppState->Arena = &GlobalArena;

    u64 CPUTimerFreq = EstimateCPUTimerFrequency();
    
    string_list *Args = PlatformGetCommandLineArgs(Arena);
    u32 ArgsCount = GetStringListCount(Args);
    if(ArgsCount == 1)
    {
        string FilePath = Args->Entry;
        char CPath[MAX_PATH];
        StringToCString(FilePath, sizeof(CPath), CPath);

        string Buffer = PlatformReadEntireFile(Arena, FilePath);
        repetition_test_series TestSeries = RepetitionTestAllocateTestSeries(Arena, ArrayCount(TestFunctions), 1024);

        u64 FileSize = Buffer.Size;

        RepetitionTestSetRowLabelLabel(&TestSeries, "ReadBufferSize");
        for(u64 ReadBufferSize = 256*1024; ReadBufferSize <= Buffer.Size; ReadBufferSize *= 2)
        {
            RepetitionTestSetRowLabel(&TestSeries, "%lluk", ReadBufferSize/1024);
            for(u32 FuncIndex = 0; FuncIndex < ArrayCount(TestFunctions); ++FuncIndex)
            {
                test_function Function = TestFunctions[FuncIndex];

                RepetitionTestSetColumnLabel(&TestSeries, "%s", Function.Name);

                repetition_tester Tester = {0};
                RepetitionTestNewTestWave(&TestSeries, &Tester, FileSize, CPUTimerFreq, 10);

                while(RepetitionTestIsTesting(&TestSeries, &Tester))
                {
                    RepetitionTestBeginTime(&Tester);
                    Function.Func(&Tester, CPath, FileSize, ReadBufferSize, Buffer);
                    RepetitionTestEndTime(&Tester);
                }

            }
        }

        PlatformConsoleOut("\n");
        ReptitionTestPrintCSV(&TestSeries, StatValue_GBPerSecond, 1.0f);
    }
    else
    {
        PlatformConsoleOut("Missing file path arg\n", 0);
    }
    
    return 0;

}
