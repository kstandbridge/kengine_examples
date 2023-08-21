#define KENGINE_LIBRARY
#define KENGINE_OPENGL
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include <math.h>

#include "handmade.h"

internal void
AppOutputSound(app_state *AppState, sound_output_buffer *SoundBuffer, s32 ToneHz)
{
    s32 WavePeriod = SoundBuffer->SamplesPerSecond/ToneHz;

    s16 *SampleOut = SoundBuffer->Samples;
    for(int SampleIndex = 0;
        SampleIndex < SoundBuffer->SampleCount;
        ++SampleIndex)
    {
#if 0
        s16 ToneVolume = 3000;
        f32 SineValue = sinf(AppState->tSine);
        s16 SampleValue = (s16)(SineValue * ToneVolume);
#else
        s16 SampleValue = 0;
#endif
        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;

        AppState->tSine += 2.0f*Pi32*1.0f/(f32)WavePeriod;
        if(AppState->tSine > 2.0f*Pi32)
        {
            AppState->tSine -= 2.0f*Pi32;
        }
    }
}

internal void
RenderWeirdGradient(offscreen_buffer *Buffer, s32 BlueOffset, s32 GreenOffset)
{
    u8 *Row = (u8 *)Buffer->Memory;    
    for(s32 Y = 0;
        Y < Buffer->Height;
        ++Y)
    {
        u32 *Pixel = (u32 *)Row;
        for(s32 X = 0;
            X < Buffer->Width;
            ++X)
        {
            u8 Blue = (u8)(X + BlueOffset);
            u8 Green = (u8)(Y + GreenOffset);

            *Pixel++ = ((Green << 16) | Blue);
        }
        
        Row += Buffer->Pitch;
    }
}

internal void
RenderPlayer(offscreen_buffer *Buffer, s32 PlayerX, s32 PlayerY)
{
    u8 *EndofBuffer = (u8 *)Buffer->Memory + Buffer->Pitch*Buffer->Height;

    u32 Color = 0xFFFFFFFF;
    s32 Top = PlayerY;
    s32 Bottom = PlayerY + 10;
    for(s32 X = PlayerX;
        X < PlayerX + 10;
        ++X)
    {
        u8 *Pixel = ((u8 *)Buffer->Memory +
                     X*Buffer->BytesPerPixel +
                     Top*Buffer->Pitch);
        for(s32 Y = Top;
            Y < Bottom;
            ++Y)
        {
            if((Pixel >= (u8 *)Buffer->Memory) &&
               ((Pixel + 4) <= EndofBuffer))
            {
                *(u32 *)Pixel = Color;
            }

            Pixel += Buffer->Pitch;
        }
    }
}

extern void 
AppUpdateAndRender(app_memory *AppMemory, app_input *Input, offscreen_buffer *Buffer)
{
    Platform = AppMemory->PlatformAPI;
    
    Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) ==
        (ArrayCount(Input->Controllers[0].Buttons)));

    app_state *AppState = AppMemory->AppState;
    if(AppState == 0)
    {
        AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);

        string FilePath = String(__FILE__);
        string File = PlatformReadEntireFile(&AppState->Arena, FilePath);
        if(File.Data)
        {
            PlatformWriteTextToFile(File, String("test.out"));
        }

        AppState->ToneHz = 512;
        AppState->tSine = 0.0f;

        AppState->PlayerX = 100;
        AppState->PlayerY = 100;
    }

    for(int ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ++ControllerIndex)
    {
        controller_input *Controller = GetController(Input, ControllerIndex);
        if(Controller->IsAnalog)
        {
            // NOTE(kstandbridge): Use analog movement tuning
            AppState->BlueOffset += (s32)4.0f*(Controller->StickAverageX);
            AppState->ToneHz = 512 + (s32)(128.0f*(Controller->StickAverageY));
        }
        else
        {
            // NOTE(kstandbridge): Use digital movement tuning
            if(Controller->MoveLeft.EndedDown)
            {
                AppState->BlueOffset -= 1;
            }
            
            if(Controller->MoveRight.EndedDown)
            {
                AppState->BlueOffset += 1;
            }
        }

        // Input.AButtonEndedDown;
        // Input.AButtonHalfTransitionCount;

        AppState->PlayerX += (s32)(4.0f*Controller->StickAverageX);
        AppState->PlayerY -= (s32)(4.0f*Controller->StickAverageY);
        if(AppState->tJump > 0)
        {
            AppState->PlayerY += (s32)(5.0f*sinf(0.5f*Pi32*AppState->tJump));
        }
        if(Controller->ActionDown.EndedDown)
        {
            AppState->tJump = 4.0f;
        }
        AppState->tJump -= 0.033f;
    }

    RenderWeirdGradient(Buffer, AppState->BlueOffset, AppState->GreenOffset);
    RenderPlayer(Buffer, AppState->PlayerX, AppState->PlayerY);

    RenderPlayer(Buffer, Input->MouseX, Input->MouseY);

    for(s32 ButtonIndex = 0;
        ButtonIndex < ArrayCount(Input->MouseButtons);
        ++ButtonIndex)
    {
        if(Input->MouseButtons[ButtonIndex].EndedDown)
        {
            RenderPlayer(Buffer, 10 + 20*ButtonIndex, 10);
        }
    }
}

extern void
AppGetSoundSamples(app_memory *AppMemory, sound_output_buffer *SoundBuffer)
{
    app_state *AppState = AppMemory->AppState;
    if(AppState)
    {
        AppOutputSound(AppState, SoundBuffer, AppState->ToneHz);
    }
    else
    {
        PlatformConsoleOut("WARN: SoundSamples requested before app state initialized!\n", 0);
    }
}