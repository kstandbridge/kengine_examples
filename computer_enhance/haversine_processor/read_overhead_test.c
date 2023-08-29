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

s32
MainLoop(app_memory *AppMemory)
{
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    memory_arena *Arena = &AppState->Arena;

    u64 CPUTimerFreq = EstimateCPUTimerFrequency();
    
    char *FilePath = "/home/eowhat/Sources/kengine_examples/bin/output.json";
    string FileCacheHack = PlatformReadEntireFile(Arena, String_(GetNullTerminiatedStringLength(FilePath), (u8 *)FilePath));

    string Buffer = 
    {
        .Size = FileCacheHack.Size,
        .Data = PushSize(Arena, FileCacheHack.Size),
    };

    {
        repetition_tester Tester = {0};
        PlatformConsoleOut("\n--- ReadViaFRead ---\n");
        RepetitionTestNewTestWave(&Tester, Buffer.Size, CPUTimerFreq, 10);
        ReadViaFRead(&Tester, &Buffer, FilePath);
    }
    {
        repetition_tester Tester = {0};
        PlatformConsoleOut("\n--- ReadViaRead ---\n");
        RepetitionTestNewTestWave(&Tester, Buffer.Size, CPUTimerFreq, 10);
        ReadViaRead(&Tester, &Buffer, FilePath);
    }

    return 0;
}
