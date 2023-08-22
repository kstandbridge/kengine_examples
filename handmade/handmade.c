#define KENGINE_LIBRARY
#define KENGINE_OPENGL
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include <math.h>

#include "handmade.h"

internal void
AppOutputSound(app_state *AppState, sound_output_buffer *SoundBuffer, s32 ToneHz)
{
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

#if 0
        s32 WavePeriod = SoundBuffer->SamplesPerSecond/ToneHz;
        AppState->tSine += 2.0f*Pi32*1.0f/(f32)WavePeriod;
        if(AppState->tSine > 2.0f*Pi32)
        {
            AppState->tSine -= 2.0f*Pi32;
        }
#endif

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
DrawRectangle(offscreen_buffer *Buffer,
              f32 RealMinX, f32 RealMinY, f32 RealMaxX, f32 RealMaxY,
              f32 R, f32 G, f32 B)
{
    // TODO(kstandbridge): Color as float

    s32 MinX = RoundF32ToS32(RealMinX);
    s32 MinY = RoundF32ToS32(RealMinY);
    s32 MaxX = RoundF32ToS32(RealMaxX);
    s32 MaxY = RoundF32ToS32(RealMaxY);

    if(MinX < 0)
    {
        MinX = 0;
    }

    if(MinY < 0)
    {
        MinY = 0;
    }

    if(MaxX > Buffer->Width)
    {
        MaxX = Buffer->Width;
    }

    if(MaxY > Buffer->Height)
    {
        MaxY = Buffer->Height;
    }

    u32 Color = ((RoundF32ToU32(R * 255.0f) << 16) |    
                 (RoundF32ToU32(G * 255.0f) << 8) |
                 (RoundF32ToU32(B * 255.0f) << 0));

    u8 *Row = ((u8 *)Buffer->Memory + MinX*Buffer->BytesPerPixel + MinY*Buffer->Pitch);
    for(s32 Y = MinY;
        Y < MaxY;
        ++Y)
    {
        u32 *Pixel = (u32 *)Row;
        for(s32 X = MinX;
            X < MaxX;
            ++X)
        {            
            *Pixel++ = Color;
        }
        
        Row += Buffer->Pitch;
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
    }

    for(int ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ++ControllerIndex)
    {
        controller_input *Controller = GetController(Input, ControllerIndex);
        if(Controller->IsAnalog)
        {
            // NOTE(kstandbridge): Use analog movement tuning
        }
        else
        {
            // NOTE(kstandbridge): Use digital movement tuning
            f32 dPlayerX = 0.0f; // pixels/second
            f32 dPlayerY = 0.0f; // pixels/second
            
            if(Controller->MoveUp.EndedDown)
            {
                dPlayerY = -1.0f;
            }
            if(Controller->MoveDown.EndedDown)
            {
                dPlayerY = 1.0f;
            }
            if(Controller->MoveLeft.EndedDown)
            {
                dPlayerX = -1.0f;
            }
            if(Controller->MoveRight.EndedDown)
            {
                dPlayerX = 1.0f;
            }
            dPlayerX *= 64.0f;
            dPlayerY *= 64.0f;

            // TODO(kstandbridge): Diagonal will be faster!  Fix once we have vectors :)
            AppState->PlayerX += Input->dtForFrame*dPlayerX;
            AppState->PlayerY += Input->dtForFrame*dPlayerY;
        }
    }

    u32 TileMap[9][17] =
    {
        {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1, 1},
        {1, 1, 0, 0,  0, 1, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0, 1},
        {1, 1, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 1, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 0, 0, 1},
        {0, 0, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 0, 0, 0, 0},
        {1, 1, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 1, 0, 0, 1},
        {1, 0, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  1, 0, 0, 0, 1},
        {1, 1, 1, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0, 1},
        {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1, 1},
    };

    f32 UpperLeftX = -30;
    f32 UpperLeftY = 0;
    f32 TileWidth = 60;
    f32 TileHeight = 60;
    
    DrawRectangle(Buffer, 0.0f, 0.0f, (f32)Buffer->Width, (f32)Buffer->Height, 1.0f, 0.0f, 0.1f);
    for(int Row = 0;
        Row < 9;
        ++Row)
    {
        for(int Column = 0;
            Column < 17;
            ++Column)
        {
            u32 TileID = TileMap[Row][Column];
            f32 Gray = 0.5f;
            if(TileID == 1)
            {
                Gray = 1.0f;
            }

            f32 MinX = UpperLeftX + ((f32)Column)*TileWidth;
            f32 MinY = UpperLeftY + ((f32)Row)*TileHeight;
            f32 MaxX = MinX + TileWidth;
            f32 MaxY = MinY + TileHeight;
            DrawRectangle(Buffer, MinX, MinY, MaxX, MaxY, Gray, Gray, Gray);
        }
    }
    
    f32 PlayerR = 1.0f;
    f32 PlayerG = 1.0f;
    f32 PlayerB = 0.0f;
    f32 PlayerWidth = 0.75f*TileWidth;
    f32 PlayerHeight = TileHeight;
    f32 PlayerLeft = AppState->PlayerX - 0.5f*PlayerWidth;
    f32 PlayerTop = AppState->PlayerY - PlayerHeight;
    DrawRectangle(Buffer,
                  PlayerLeft, PlayerTop,
                  PlayerLeft + PlayerWidth,
                  PlayerTop + PlayerHeight,
                  PlayerR, PlayerG, PlayerB);
}

extern void
AppGetSoundSamples(app_memory *AppMemory, sound_output_buffer *SoundBuffer)
{
    app_state *AppState = AppMemory->AppState;
    if(AppState)
    {
        AppOutputSound(AppState, SoundBuffer, 400);
    }
    else
    {
        PlatformConsoleOut("WARN: SoundSamples requested before app state initialized!\n", 0);
    }
}