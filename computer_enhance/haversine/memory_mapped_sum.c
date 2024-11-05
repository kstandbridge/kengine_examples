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

#if KENGINE_WIN32

typedef struct memory_mapped_file
{
    HANDLE File;
    HANDLE Mapping;

    string Buffer;

} memory_mapped_file;

internal memory_mapped_file
MemoryMappedFileOpen(char *FileName)
{
    memory_mapped_file Result = {0};

    Result.File = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    Result.Mapping = CreateFileMappingA(Result.File, 0, PAGE_READONLY, 0, 0, 0);

    return Result;
}

internal void
MemoryMappedFileSetRegion(memory_mapped_file *MappedFile, u64 Offset, u64 Size)
{
    if(StringIsValid(MappedFile->Buffer))
    {
        UnmapViewOfFile(MappedFile->Buffer.Data);
        ZeroStruct(MappedFile->Buffer);
    }

    if(Size)
    {
        DWORD OffsetHigh = (DWORD)(Offset >> 32);
        DWORD OffsetLow = (DWORD)(Offset & 0xffffffff);
        u8 *Data = (u8 *)MapViewOfFile(MappedFile->Mapping, FILE_MAP_READ, OffsetHigh, OffsetLow, Size);
        if(Data)
        {
            MappedFile->Buffer.Size = Size;
            MappedFile->Buffer.Data = Data;
        }
    }
}

internal b32
MemoryMappedFileIsValid(memory_mapped_file MappedFile)
{
    b32 Result = (MappedFile.Mapping != 0);
    return Result;
}

internal void
MemoryMappedFileClose(memory_mapped_file *MappedFile)
{
    MemoryMappedFileSetRegion(MappedFile, 0, 0);

    if(MappedFile->Mapping)
    {
        CloseHandle(MappedFile->Mapping);
    }

    if(MappedFile->File != INVALID_HANDLE_VALUE)
    {
        CloseHandle(MappedFile->File);
    }

    ZeroStruct(MappedFile);
}

typedef HANDLE thread_handle;
#define THREAD_ENTRY_POINT(Name, Parameter) internal DWORD WINAPI Name(void *Parameter)

internal thread_handle
CreateAndStartThread(LPTHREAD_START_ROUTINE ThreadFunction, void *ThreadParam)
{
    thread_handle Result = CreateThread(0, 0, ThreadFunction, ThreadParam, 0, 0);
    return Result;
}

#elif KENGINE_LINUX

#include <pthread.h>

typedef pthread_t thread_handle;
#define THREAD_ENTRY_POINT(Name, Parameter) internal void *Name(void *Parameter)

internal thread_handle
CreateAndStartThread(void *ThreadFunction, void *ThreadParam)
{
    pthread_attr_t Attr;
    pthread_t ThreadId;
    pthread_attr_init(&Attr);
    pthread_attr_setdetachstate(&Attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&ThreadId, &Attr, ThreadFunction, ThreadParam);
    pthread_attr_destroy(&Attr);

    thread_handle Result = ThreadId;
    return Result;
}

#else
#error Unsupported platform
#endif

internal b32
ThreadIsValid(thread_handle Handle)
{
    b32 Result = (Handle != 0);
    return Result;
}

internal u64
Sum64s(u64 DataSize, void *Data)
{
    u64 *Source = (u64 *)Data;
    u64 Sum0 = 0;
    u64 Sum1 = 0;
    u64 Sum2 = 0;
    u64 Sum3 = 0;
    u64 SumCount = DataSize / (4*8);
    while(SumCount--)
    {
        Sum0 += Source[0];
        Sum1 += Source[1];
        Sum2 += Source[2];
        Sum3 += Source[3];
        Source += 4;
    }

    u64 Result = Sum0 + Sum1 + Sum2 + Sum3;
    return Result;
}

typedef u64 file_process_func(repetition_tester *Tester, char *FileName, u64 TotalFileSize, u64 BufferSize);
typedef struct test_function
{
    char *Name;
    file_process_func *Func;
} test_function;

internal u64
MapAndSum(repetition_tester *Tester, char *FileName, u64 TotalFileSize, u64 BufferSize)
{
    u64 Result = 0;

    memory_mapped_file MappedFile = MemoryMappedFileOpen(FileName);
    if(MemoryMappedFileIsValid(MappedFile))
    {
        u64 AtOffset = 0;
        while(AtOffset < TotalFileSize)
        {
            u64 SizeRemaining = TotalFileSize - AtOffset;
            u64 ReadSize = BufferSize;
            if(ReadSize > SizeRemaining)
            {
                ReadSize = SizeRemaining;
            }

            MemoryMappedFileSetRegion(&MappedFile, AtOffset, ReadSize);
            if(StringIsValid(MappedFile.Buffer))
            {
                Result += Sum64s(MappedFile.Buffer.Size, MappedFile.Buffer.Data);
                RepetitionTestCountBytes(Tester, ReadSize);
            }
            else
            {
                RepetitionTestError(Tester, "Memory map failed");
            }

            AtOffset += ReadSize;
        }
    }
    else
    {
        RepetitionTestError(Tester, "Couldn't open file for mapping");
    }

    MemoryMappedFileClose(&MappedFile);

    return Result;
}

typedef struct threaded_mapped_io
{
    string Buffer;
    volatile b32 ThreadFinished;
} threaded_mapped_io;

THREAD_ENTRY_POINT(MappedIOThreadRoutine, Parameter)
{
    threaded_mapped_io *ThreadedIO = (threaded_mapped_io *)Parameter;

    u32 PreventOptimization = 0;
    string Buffer = ThreadedIO->Buffer;
    for(u64 TouchOffset = 0; TouchOffset < Buffer.Size; TouchOffset += 4096)
    {
        PreventOptimization += Buffer.Data[TouchOffset];
    }

    ThreadedIO->ThreadFinished = true;

    return PreventOptimization;
}

internal u64
MapAndSumOverlapped(repetition_tester *Tester, char *FileName, u64 TotalFileSize, u64 BufferSize)
{
    u64 Result = 0;

    memory_mapped_file MappedFile = MemoryMappedFileOpen(FileName);
    if(MemoryMappedFileIsValid(MappedFile))
    {
        MemoryMappedFileSetRegion(&MappedFile, 0, TotalFileSize);
        if(StringIsValid(MappedFile.Buffer))
        {
            threaded_mapped_io ThreadedIO = {0};
            ThreadedIO.Buffer = MappedFile.Buffer;

            thread_handle IOThread = CreateAndStartThread(MappedIOThreadRoutine, &ThreadedIO);
            if(ThreadIsValid(IOThread))
            {
                Result = Sum64s(MappedFile.Buffer.Size, MappedFile.Buffer.Data);
                RepetitionTestCountBytes(Tester, MappedFile.Buffer.Size);

                while(!ThreadedIO.ThreadFinished)
                {
                    _mm_pause();
                }
            }
            else
            {
                RepetitionTestError(Tester, "Couldn't start touch thread");
            }
        }
        else
        {
            RepetitionTestError(Tester, "Couldn't map file memory");
        }
    }
    else
    {
        RepetitionTestError(Tester, "Couldn't open file for mapping");
    }

    MemoryMappedFileClose(&MappedFile);

    return Result;
}

internal u64
OpenAllocateAndSum(repetition_tester *Tester, char *FileName, u64 TotalFileSize, u64 BufferSize)
{
    u64 Result = 0;

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
                Result += Sum64s(ReadSize, Buffer.Data);
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

    return Result;
}

typedef enum overlapped_buffer_state
{
    Buffer_Unused,
    Buffer_ReadCompleted,
} overlapped_buffer_state;

typedef struct overlapped_buffer
{
    string Value;
    volatile u64 ReadSize;
    volatile overlapped_buffer_state State;
} overlapped_buffer;

typedef struct threaded_io
{
    overlapped_buffer Buffers[2];
    u64 TotalFileSize;
    FILE *File;
    b32 ReadError;
} threaded_io;

THREAD_ENTRY_POINT(IOThreadRoutine, Parameter)
{
    threaded_io *ThreadedIO = (threaded_io *)Parameter;

    FILE *File = ThreadedIO->File;
    u32 BufferIndex = 0;
    u64 SizeRemaining = ThreadedIO->TotalFileSize;
    while(SizeRemaining)
    {
        overlapped_buffer *Buffer = &ThreadedIO->Buffers[BufferIndex++ & 1];
        u64 ReadSize = Buffer->Value.Size;
        if(ReadSize > SizeRemaining)
        {
            ReadSize = SizeRemaining;
        }

        while(Buffer->State != Buffer_Unused)
        {
            _mm_pause();
        }

        _mm_mfence();
        
        if(fread(Buffer->Value.Data, ReadSize, 1, File) != 1)
        {
            ThreadedIO->ReadError = true;
        }

        Buffer->ReadSize = ReadSize;

        _mm_mfence();

        Buffer->State = Buffer_ReadCompleted;

        SizeRemaining -= ReadSize;
    }

    return 0;
}

internal u64
OpenAllocateAndSumOverlapped(repetition_tester *Tester, char *FileName, u64 TotalFileSize, u64 BufferSize)
{
    u64 Result = 0;

    threaded_io ThreadedIO = 
    {
        .File = fopen(FileName, "rb"),
        .TotalFileSize = TotalFileSize,
        .Buffers = 
        {
            { .Value = String_(BufferSize, AllocateMemory(BufferSize)) },
            { .Value = String_(BufferSize, AllocateMemory(BufferSize)) },
        }
    };

    thread_handle IOThread = {0};
    if(ThreadedIO.File &&
       StringIsValid(ThreadedIO.Buffers[0].Value) &&
       StringIsValid(ThreadedIO.Buffers[1].Value))
    {
        IOThread = CreateAndStartThread(IOThreadRoutine, &ThreadedIO);
    }

    if(ThreadIsValid(IOThread))
    {
        u64 BufferIndex = 0;
        u64 SizeRemaining = TotalFileSize;
        while(SizeRemaining)
        {
            overlapped_buffer *Buffer = &ThreadedIO.Buffers[BufferIndex++ & 1];

            while(Buffer->State != Buffer_ReadCompleted)
            {
                _mm_pause();
            }

            _mm_mfence();

            u64 ReadSize = Buffer->ReadSize;
            Result += Sum64s(ReadSize, Buffer->Value.Data);
            RepetitionTestCountBytes(Tester, ReadSize);

            _mm_mfence();

            Buffer->State = Buffer_Unused;

            SizeRemaining -= ReadSize;
        }

        if(ThreadedIO.ReadError)
        {
            RepetitionTestError(Tester, "fread failed");
        }
    }
    else
    {
        RepetitionTestError(Tester, "Couldn't acquire resources");
    }

    FreeMemory(ThreadedIO.Buffers[0].Value.Data, BufferSize);
    FreeMemory(ThreadedIO.Buffers[1].Value.Data, BufferSize);
    fclose(ThreadedIO.File);

    return Result;
}

global test_function TestFunctions[] =
{
    { "MapAndSum", MapAndSum},
    { "MapAndSumOverlapped", MapAndSumOverlapped},
    { "OpenAllocateAndSum", OpenAllocateAndSum},
    { "OpenAllocateAndSumOverlapped", OpenAllocateAndSumOverlapped},
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
        u64 ReferenceSum = Sum64s(Buffer.Size, Buffer.Data);

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

                b32 Passed = true;
                while(RepetitionTestIsTesting(&TestSeries, &Tester))
                {
                    RepetitionTestBeginTime(&Tester);
                    u64 Check = Function.Func(&Tester, CPath, FileSize, ReadBufferSize);
                    if(Check != ReferenceSum)
                    {
                        Passed = false;
                    }
                    RepetitionTestEndTime(&Tester);

                    if(!Passed)
                    {
                        PlatformConsoleOut("WARN: Checksum mismatch\n");
                    }
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
