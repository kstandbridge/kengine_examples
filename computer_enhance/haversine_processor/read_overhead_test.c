#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include <stdio.h>
#include <fcntl.h>

typedef struct app_state
{
    memory_arena Arena;
} app_state;

internal void
ReadViaFRead(repetition_tester *Tester, string *Output, char *FilePath)
{
    while(RepetitionTestIsTesting(Tester))
    {
        FILE *File = fopen(FilePath, "rb");
        if(File)
        {
            RepetitionTestBeginTime(Tester);
            size_t Result = fread(Output->Data, Output->Size, 1, File);
            RepetitionTestEndTime(Tester);

            if(Result == 1)
            {
                RepetitionTestCountBytes(Tester, Output->Size);
            }
            else
            {
                RepetitionTestError(Tester, "fread failed");
            }

            fclose(File);
        }
        else
        {
            RepetitionTestError(Tester, "fopen failed");
        }
    }
}

#if KENGINE_LINUX
internal void
ReadViaRead(repetition_tester *Tester, string *Output, char *FilePath)
{
    while(RepetitionTestIsTesting(Tester))
    {
        s32 FileHandle = open(FilePath, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(FileHandle >= 0)
        {
            RepetitionTestBeginTime(Tester);
            s64 Offset = 0;
            ssize_t BytesRead = pread(FileHandle, Output->Data, Output->Size, Offset);
            RepetitionTestEndTime(Tester);
        
            if(Output->Size == (umm)BytesRead)
            {
                RepetitionTestCountBytes(Tester, Output->Size);
            }
            else
            {
                RepetitionTestError(Tester, "pread failed");
            }


            close(FileHandle);
        }
        else
        {
            RepetitionTestError(Tester, "open failed");
        }
    }
}
#elif KENGINE_WIN32
internal void
ReadViaRead(repetition_tester *Tester, string *Output, char *FilePath)
{
    while(RepetitionTestIsTesting(Tester))
    {
        HANDLE FileHandle = CreateFileA(FilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if(FileHandle != INVALID_HANDLE_VALUE)
        {
            RepetitionTestBeginTime(Tester);
            u32 BytesRead = 0;
            ReadFile(FileHandle, Output->Data, (u32)Output->Size, (LPDWORD)&BytesRead, 0);

            RepetitionTestEndTime(Tester);
        
            if(Output->Size == (umm)BytesRead)
            {
                RepetitionTestCountBytes(Tester, Output->Size);
            }
            else
            {
                RepetitionTestError(Tester, "pread failed");
            }


            CloseHandle(FileHandle);
        }
        else
        {
            RepetitionTestError(Tester, "open failed");
        }
    }
}
#else
#error Platform not supported
#endif

s32
MainLoop(app_memory *AppMemory)
{
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    memory_arena *Arena = &AppState->Arena;

    u64 CPUTimerFreq = EstimateCPUTimerFrequency();
    
    string_list *Args = PlatformGetCommandLineArgs(Arena);
    u32 ArgsCount = GetStringListCount(Args);
    if(ArgsCount == 1)
    {
        string FilePath = Args->Entry;
        char CPath[MAX_PATH];
        StringToCString(FilePath, sizeof(CPath), CPath);
        string FileCacheHack = PlatformReadEntireFile(Arena, FilePath);

        string Buffer = 
        {
            .Size = FileCacheHack.Size,
            .Data = PushSize(Arena, FileCacheHack.Size),
        };

        {
            repetition_tester Tester = {0};
            PlatformConsoleOut("\n--- ReadViaFRead ---\n");
            RepetitionTestNewTestWave(&Tester, Buffer.Size, CPUTimerFreq, 10);
            ReadViaFRead(&Tester, &Buffer, CPath);
        }
        {
            repetition_tester Tester = {0};
            PlatformConsoleOut("\n--- ReadViaRead ---\n");
            RepetitionTestNewTestWave(&Tester, Buffer.Size, CPUTimerFreq, 10);
            ReadViaRead(&Tester, &Buffer, CPath);
        }
    }
    else
    {
        PlatformConsoleOut("Missing file path arg\n");
    }
    
    return 0;
}
