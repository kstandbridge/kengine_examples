#define KENGINE_LIBRARY
#define KENGINE_OPENGL
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include <math.h>

#include "handmade.h"
#include "handmade_tile.c"

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

    f32 PlayerHeight = 1.4f;
    f32 PlayerWidth = 0.75f*PlayerHeight;

#if 0
    f32 LowerLeftX = -(f32)World.TileSideInPixels/2.0f;
    f32 LowerLeftY = (f32)Buffer->Height;
#endif

    app_state *AppState = AppMemory->AppState;
    if(AppState == 0)
    {
        AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);

        // TODO(kstandbridge): Make random?
        AppState->RandomState.Value = 1234;

        AppState->PlayerP.AbsTileX = 1;
        AppState->PlayerP.AbsTileY = 3;
        AppState->PlayerP.TileRelX = 5.0f;
        AppState->PlayerP.TileRelY = 5.0f;

        AppState->World = PushStruct(&AppState->Arena, world);
        world *World = AppState->World;
        World->TileMap = PushStruct(&AppState->Arena, tile_map);

        tile_map *TileMap = World->TileMap;

        TileMap->ChunkShift = 4;
        TileMap->ChunkMask = (1 << TileMap->ChunkShift) - 1;
        TileMap->ChunkDim = (1 << TileMap->ChunkShift);

        TileMap->TileChunkCountX = 128;
        TileMap->TileChunkCountY = 128;
        TileMap->TileChunkCountZ = 2;
        TileMap->TileChunks = PushArray(&AppState->Arena,
                                         TileMap->TileChunkCountX*
                                         TileMap->TileChunkCountY*
                                         TileMap->TileChunkCountZ,
                                         tile_chunk);

        TileMap->TileSideInMeters = 1.4f;

        u32 TilesPerWidth = 17;
        u32 TilesPerHeight = 9;
        u32 ScreenX = 0;
        u32 ScreenY = 0;
        u32 AbsTileZ = 0;

        // TODO(kstandbridge): Replace all this with real world generation!
        b32 DoorLeft = false;
        b32 DoorRight = false;
        b32 DoorTop = false;
        b32 DoorBottom = false;
        b32 DoorUp = false;
        b32 DoorDown = false;
        for(u32 ScreenIndex = 0;
            ScreenIndex < 100;
            ++ScreenIndex)
        {
            u32 RandomChoice = RandomU32(&AppState->RandomState);
            if(DoorUp || DoorDown)
            {
                RandomChoice %= 2;
            }
            else 
            {
                RandomChoice %= 3;
            }

            if(RandomChoice == 2)
            {
                if(AbsTileZ == 0)
                {
                    DoorUp = true;
                }
                else 
                {
                    DoorDown = true;
                }
            }
            else if(RandomChoice == 1)
            {
                DoorRight = true;
            }
            else 
            {
                DoorTop = true;
            }

            for(u32 TileY = 0;
                TileY < TilesPerHeight;
                ++TileY)
            {
                for(u32 TileX = 0;
                    TileX < TilesPerWidth;
                    ++TileX)
                {
                    u32 AbsTileX = ScreenX*TilesPerWidth + TileX;
                    u32 AbsTileY = ScreenY*TilesPerHeight + TileY;

                    u32 TileValue = 1;
                    if((TileX == 0) && (!DoorLeft || (TileY != (TilesPerHeight/2))))
                    {
                        TileValue = 2;
                    }

                    if((TileX == (TilesPerWidth - 1) && (!DoorRight || (TileY != (TilesPerHeight/2)))))
                    {
                        TileValue = 2;
                    }

                    if((TileY == 0) && (!DoorBottom || (TileX != (TilesPerWidth/2))))
                    {
                        TileValue = 2;
                    }

                    if((TileY == (TilesPerHeight - 1) && (!DoorTop || (TileX != (TilesPerWidth/2)))))
                    {
                        TileValue = 2;
                    }

                    if((TileX == 10) && (TileY == 6))
                    {
                        if(DoorUp)
                        {
                            TileValue = 3;
                        }

                        if(DoorDown)
                        {
                            TileValue = 4;
                        }
                    }

                    SetTileValue(&AppState->Arena, World->TileMap, AbsTileX, AbsTileY, AbsTileZ,
                                 TileValue);
                }
            }

            DoorLeft = DoorRight;
            DoorBottom = DoorTop;

            if(DoorUp)
            {
                DoorDown = true;
                DoorUp = false;
            }
            else if(DoorDown)
            {
                DoorUp = true;
                DoorDown = false;
            }
            else
            {
                DoorUp = false;
                DoorDown = false;
            }

            DoorRight = false;
            DoorTop = false;

            if(RandomChoice == 2)
            {
                if(AbsTileZ == 0)
                {
                    AbsTileZ = 1;
                }
                else
                {
                    AbsTileZ = 0;
                }                
            }
            else if(RandomChoice == 1)
            {
                ScreenX += 1;
            }
            else
            {
                ScreenY += 1;
            }
        }
    }

    world *World = AppState->World;
    tile_map *TileMap = World->TileMap;

    s32 TileSideInPixels = 60;
    f32 MetersToPixels = (f32)TileSideInPixels / (f32)TileMap->TileSideInMeters;

    // f32 LowerLeftX = -(f32)TileSideInPixels/2;
    // f32 LowerLeftY = (f32)Buffer->Height;

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
                dPlayerY = 1.0f;
            }
            if(Controller->MoveDown.EndedDown)
            {
                dPlayerY = -1.0f;
            }
            if(Controller->MoveLeft.EndedDown)
            {
                dPlayerX = -1.0f;
            }
            if(Controller->MoveRight.EndedDown)
            {
                dPlayerX = 1.0f;
            }
            f32 PlayerSpeed = 2.0f;
            if(Controller->ActionUp.EndedDown)
            {
                PlayerSpeed = 10.0f;
            }
            dPlayerX *= PlayerSpeed;
            dPlayerY *= PlayerSpeed;

            // TODO(kstandbridge): Diagonal will be faster!  Fix once we have vectors :)
            tile_map_position NewPlayerP = AppState->PlayerP;
            NewPlayerP.TileRelX += Input->dtForFrame*dPlayerX;
            NewPlayerP.TileRelY += Input->dtForFrame*dPlayerY;
            NewPlayerP = RecanonicalizePosition(TileMap, NewPlayerP);
            // TODO(kstandbridge): Delta function that auto recanonicalizes

            tile_map_position PlayerLeft = NewPlayerP;
            PlayerLeft.TileRelX -= 0.5f*PlayerWidth;
            PlayerLeft = RecanonicalizePosition(TileMap, PlayerLeft);

            tile_map_position PlayerRight = NewPlayerP;
            PlayerRight.TileRelX += 0.5f*PlayerWidth;
            PlayerRight = RecanonicalizePosition(TileMap, PlayerRight);

            if(IsTileMapPointEmpty(TileMap, NewPlayerP),
               IsTileMapPointEmpty(TileMap, PlayerLeft),
               IsTileMapPointEmpty(TileMap, PlayerRight))
            {
                AppState->PlayerP = NewPlayerP;
            }

        }
    }
    
    DrawRectangle(Buffer, 0.0f, 0.0f, (f32)Buffer->Width, (f32)Buffer->Height,
                  1.0f, 0.0f, 0.1f);

    f32 ScreenCenterX = 0.5f*(f32)Buffer->Width;
    f32 ScreenCenterY = 0.5f*(f32)Buffer->Height;

    for(s32 RelRow = -10;
        RelRow < 10;
        ++RelRow)
    {
        for(s32 RelColumn = -20;
            RelColumn < 20;
            ++RelColumn)
        {
            u32 Column = AppState->PlayerP.AbsTileX + RelColumn;
            u32 Row = AppState->PlayerP.AbsTileY + RelRow;
            u32 TileID = GetTileValue(TileMap, Column, Row, AppState->PlayerP.AbsTileZ);

            if(TileID > 0)
            {
                f32 Gray = 0.5f;
                if(TileID == 2)
                {
                    Gray = 1.0f;
                }

                if(TileID > 2)
                {
                    Gray = 0.25f;
                }

                if((Column == AppState->PlayerP.AbsTileX) &&
                   (Row == AppState->PlayerP.AbsTileY))
                {
                    Gray = 0.0f;
                }

                f32 CenX = ScreenCenterX - MetersToPixels*AppState->PlayerP.TileRelX + ((f32)RelColumn)*TileSideInPixels;
                f32 CenY = ScreenCenterY + MetersToPixels*AppState->PlayerP.TileRelY - ((f32)RelRow)*TileSideInPixels;
                f32 MinX = CenX - 0.5f*TileSideInPixels;
                f32 MinY = CenY - 0.5f*TileSideInPixels;
                f32 MaxX = CenX + 0.5f*TileSideInPixels;
                f32 MaxY = CenY + 0.5f*TileSideInPixels;
                DrawRectangle(Buffer, MinX, MinY, MaxX, MaxY, Gray, Gray, Gray);
            }
        }
    }
    
    f32 PlayerR = 1.0f;
    f32 PlayerG = 1.0f;
    f32 PlayerB = 0.0f;
    f32 PlayerLeft = ScreenCenterX - 0.5f*MetersToPixels*PlayerWidth;
    f32 PlayerTop = ScreenCenterY - MetersToPixels*PlayerHeight;
    DrawRectangle(Buffer,
                  PlayerLeft, PlayerTop,
                  PlayerLeft + MetersToPixels*PlayerWidth,
                  PlayerTop + MetersToPixels*PlayerHeight,
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
