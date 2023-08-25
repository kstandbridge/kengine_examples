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

internal tile_map *
GetTileMap(world *World, s32 TileMapX, s32 TileMapY)
{
    tile_map *Result = 0;

    if((TileMapX >= 0) && (TileMapX < World->TileMapCountX) &&
       (TileMapY >= 0) && (TileMapY < World->TileMapCountY))
    {
        Result = &World->TileMaps[TileMapY*World->TileMapCountX + TileMapX];
    }

    return Result;
}

internal u32
GetTileValueUnchecked(world *World, tile_map *TileMap, s32 TileX, s32 TileY)
{
    Assert(TileMap);
    Assert((TileX >= 0) && (TileX < World->CountX) &&
           (TileY >= 0) && (TileY < World->CountY));

    u32 Result = TileMap->Tiles[TileY*World->CountX + TileX];

    return Result; 
}

internal b32
IsTileMapPointEmpty(world *World, tile_map *TileMap, f32 TestTileX, f32 TestTileY)
{
    b32 Result = false;

    if(TileMap)
    {
        if((TestTileX >= 0) && (TestTileX < World->CountX) &&
           (TestTileY >= 0) && (TestTileY < World->CountY))
        {
            u32 TileMapValue = GetTileValueUnchecked(World, TileMap, TestTileX, TestTileY);
            Result = (TileMapValue == 0);
        }
    }

    return Result;
}

internal void
RecanonicalizeCoord(world *World, s32 TileCount, s32 *TileMap, s32 *Tile, f32 *TileRel)
{
    s32 Offset = FloorF32ToS32(*TileRel / World->TileSideInMeters);

    *Tile += Offset;
    *TileRel -= Offset*World->TileSideInMeters;

    Assert(*TileRel >= 0);
    // TODO(kstandbridge): Fix floating point math so this can be <
    Assert(*TileRel <= World->TileSideInMeters);

    if(*Tile < 0)
    {
        *Tile = TileCount + *Tile;
        --*TileMap;
    }

    if(*Tile >= TileCount)
    {
        *Tile = *Tile - TileCount;
        ++*TileMap;
    }
}

internal canonical_position
RecanonicalizePosition(world *World, canonical_position Pos)
{
    canonical_position Result = Pos;

    RecanonicalizeCoord(World, World->CountX, &Result.TileMapX, &Result.TileX, &Result.TileRelX);
    RecanonicalizeCoord(World, World->CountY, &Result.TileMapY, &Result.TileY, &Result.TileRelY);

    return Result;
}

internal b32
IsWorldPointEmpty(world *World, canonical_position CanPos)
{
    b32 Result = false;

    tile_map *TileMap = GetTileMap(World, CanPos.TileMapX, CanPos.TileMapY);
    Result = IsTileMapPointEmpty(World, TileMap, CanPos.TileX, CanPos.TileY);
    
    return Result;
}

extern void 
AppUpdateAndRender(app_memory *AppMemory, app_input *Input, offscreen_buffer *Buffer)
{
    Platform = AppMemory->PlatformAPI;
    
    Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) ==
        (ArrayCount(Input->Controllers[0].Buttons)));

#define TILE_MAP_COUNT_X 17
#define TILE_MAP_COUNT_Y 9
    u32 Tiles00[TILE_MAP_COUNT_Y][TILE_MAP_COUNT_X] =
    {
        {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, 1},
        {1, 1, 0, 0,  0, 1, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0, 1},
        {1, 1, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 1, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 0, 0, 0, 0},
        {1, 1, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 1, 0, 0, 1},
        {1, 0, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  1, 0, 0, 0, 1},
        {1, 1, 1, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0, 1},
        {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1, 1},
    };
    
    u32 Tiles01[TILE_MAP_COUNT_Y][TILE_MAP_COUNT_X] =
    {
        {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, 1},
    };
    
    u32 Tiles10[TILE_MAP_COUNT_Y][TILE_MAP_COUNT_X] =
    {
        {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1, 1},
    };
    
    u32 Tiles11[TILE_MAP_COUNT_Y][TILE_MAP_COUNT_X] =
    {
        {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
        {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, 1},
    };

    tile_map TileMaps[2][2];
    TileMaps[0][0].Tiles = (u32 *)Tiles00;
    TileMaps[0][1].Tiles = (u32 *)Tiles10;
    TileMaps[1][0].Tiles = (u32 *)Tiles01;
    TileMaps[1][1].Tiles = (u32 *)Tiles11;

    world World =
    {
        .TileMapCountX = 2,
        .TileMapCountY = 2,
        .TileMaps = (tile_map *)TileMaps,
        .CountX = TILE_MAP_COUNT_X,
        .CountY = TILE_MAP_COUNT_Y,

        // TODO(kstandbridge): Begin using tile side in meters
        .TileSideInMeters = 1.4f,
        .TileSideInPixels = 60,

        .UpperLeftY = 0,
   };
   World.UpperLeftX = -(f32)World.TileSideInPixels/2.0f;
   World.MetersToPixels = (f32)World.TileSideInPixels / (f32)World.TileSideInMeters;

    f32 PlayerHeight = 1.4f;
    f32 PlayerWidth = 0.75f*PlayerHeight;

    app_state *AppState = AppMemory->AppState;
    if(AppState == 0)
    {
        AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);

        AppState->PlayerP.TileMapX = 0;
        AppState->PlayerP.TileMapY = 0;
        AppState->PlayerP.TileX = 3;
        AppState->PlayerP.TileY = 3;
        AppState->PlayerP.TileRelX = 5.0f;
        AppState->PlayerP.TileRelY = 5.0f;
    }

    tile_map *TileMap = GetTileMap(&World, AppState->PlayerP.TileMapX, AppState->PlayerP.TileMapY);
    Assert(TileMap);

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
            dPlayerX *= 2.0f;
            dPlayerY *= 2.0f;

            // TODO(kstandbridge): Diagonal will be faster!  Fix once we have vectors :)
            canonical_position NewPlayerP = AppState->PlayerP;
            NewPlayerP.TileRelX += Input->dtForFrame*dPlayerX;
            NewPlayerP.TileRelY += Input->dtForFrame*dPlayerY;
            NewPlayerP = RecanonicalizePosition(&World, NewPlayerP);
            // TODO(kstandbridge): Delta function that auto recanonicalizes

            canonical_position PlayerLeft = NewPlayerP;
            PlayerLeft.TileRelX -= 0.5f*PlayerWidth;
            PlayerLeft = RecanonicalizePosition(&World, PlayerLeft);

            canonical_position PlayerRight = NewPlayerP;
            PlayerRight.TileRelX += 0.5f*PlayerWidth;
            PlayerRight = RecanonicalizePosition(&World, PlayerRight);

            if(IsWorldPointEmpty(&World, NewPlayerP),
               IsWorldPointEmpty(&World, PlayerLeft),
               IsWorldPointEmpty(&World, PlayerRight))
            {
                AppState->PlayerP = NewPlayerP;
            }

        }
    }
    
    DrawRectangle(Buffer, 0.0f, 0.0f, (f32)Buffer->Width, (f32)Buffer->Height,
                  1.0f, 0.0f, 0.1f);
    for(s32 Row = 0;
        Row < 9;
        ++Row)
    {
        for(s32 Column = 0;
            Column < 17;
            ++Column)
        {
            u32 TileID = GetTileValueUnchecked(&World, TileMap, Column, Row);
            f32 Gray = 0.5f;
            if(TileID == 1)
            {
                Gray = 1.0f;
            }

            if((Column == AppState->PlayerP.TileX) &&
               (Row == AppState->PlayerP.TileY))
            {
                Gray = 0.0f;
            }

            f32 MinX = World.UpperLeftX + ((f32)Column)*World.TileSideInPixels;
            f32 MinY = World.UpperLeftY + ((f32)Row)*World.TileSideInPixels;
            f32 MaxX = MinX + World.TileSideInPixels;
            f32 MaxY = MinY + World.TileSideInPixels;
            DrawRectangle(Buffer, MinX, MinY, MaxX, MaxY, Gray, Gray, Gray);
        }
    }
    
    f32 PlayerR = 1.0f;
    f32 PlayerG = 1.0f;
    f32 PlayerB = 0.0f;
    f32 PlayerLeft = World.UpperLeftX + World.TileSideInPixels*AppState->PlayerP.TileX +
        World.MetersToPixels*AppState->PlayerP.TileRelX - 0.5f*World.MetersToPixels*PlayerWidth;
    f32 PlayerTop = World.UpperLeftY + World.TileSideInPixels*AppState->PlayerP.TileY +
        World.MetersToPixels*AppState->PlayerP.TileRelY - 0.5f*World.MetersToPixels*PlayerHeight;
    DrawRectangle(Buffer,
                  PlayerLeft, PlayerTop,
                  PlayerLeft + World.MetersToPixels*PlayerWidth,
                  PlayerTop + World.MetersToPixels*PlayerHeight,
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