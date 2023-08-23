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

inline tile_map *
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

inline u32
GetTileValueUnchecked(tile_map *TileMap, s32 TileX, s32 TileY)
{
    u32 Result = TileMap->Tiles[TileY*TileMap->CountX + TileX];

    return Result; 
}

internal b32
IsTileMapPointEmpty(tile_map *TileMap, f32 TestX, f32 TestY)
{
    b32 Result = false;

    s32 PlayerTileX = FloorF32ToS32((TestX - TileMap->UpperLeftX) / TileMap->TileWidth);
    s32 PlayerTileY = FloorF32ToS32((TestY - TileMap->UpperLeftY) / TileMap->TileHeight);

    if((PlayerTileX >= 0) && (PlayerTileX < TileMap->CountX) &&
       (PlayerTileY >= 0) && (PlayerTileY < TileMap->CountY))
    {
        u32 TileMapValue = GetTileValueUnchecked(TileMap, PlayerTileX, PlayerTileY);
        Result = (TileMapValue == 0);
    }

    return Result;
}

internal b32
IsWorldPointEmpty(world *World, s32 TileMapX, s32 TileMapY, f32 TestX, f32 TestY)
{
    b32 Result = false;

    tile_map *TileMap = GetTileMap(World, TileMapX, TileMapY);
    if(TileMap)
    {
        s32 PlayerTileX = FloorF32ToS32((TestX - TileMap->UpperLeftX) / TileMap->TileWidth);
        s32 PlayerTileY = FloorF32ToS32((TestY - TileMap->UpperLeftY) / TileMap->TileHeight);

        if((PlayerTileX >= 0) && (PlayerTileX < TileMap->CountX) &&
           (PlayerTileY >= 0) && (PlayerTileY < TileMap->CountY))
        {
            u32 TileMapValue = GetTileValueUnchecked(TileMap, PlayerTileX, PlayerTileY);
            Result = (TileMapValue == 0);
        }
    }
    
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
    TileMaps[0][0].CountX = TILE_MAP_COUNT_X;
    TileMaps[0][0].CountY = TILE_MAP_COUNT_Y;
    
    TileMaps[0][0].UpperLeftX = -30;
    TileMaps[0][0].UpperLeftY = 0;
    TileMaps[0][0].TileWidth = 60;
    TileMaps[0][0].TileHeight = 60;

    TileMaps[0][0].Tiles = (u32 *)Tiles00;

    TileMaps[0][1] = TileMaps[0][0];
    TileMaps[0][1].Tiles = (u32 *)Tiles01;

    TileMaps[1][0] = TileMaps[0][0];
    TileMaps[1][0].Tiles = (u32 *)Tiles10;

    TileMaps[1][1] = TileMaps[0][0];
    TileMaps[1][1].Tiles = (u32 *)Tiles11;

    tile_map *TileMap = &TileMaps[0][0];

#if 0
    world World;
    World.TileMapCountX = 2;
    World.TileMapCountY = 2;
s
    World.TileMaps = (tile_map *)TileMaps;
#endif

    f32 PlayerWidth = 0.75f*TileMap->TileWidth;
    f32 PlayerHeight = TileMap->TileHeight;

    app_state *AppState = AppMemory->AppState;
    if(AppState == 0)
    {
        AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);

        AppState->PlayerX = 150;
        AppState->PlayerY = 150;
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
            f32 NewPlayerX = AppState->PlayerX + Input->dtForFrame*dPlayerX;
            f32 NewPlayerY = AppState->PlayerY + Input->dtForFrame*dPlayerY;

            if(IsTileMapPointEmpty(TileMap, NewPlayerX - 0.5f*PlayerWidth, NewPlayerY) &&
               IsTileMapPointEmpty(TileMap, NewPlayerX + 0.5f*PlayerWidth, NewPlayerY) &&
               IsTileMapPointEmpty(TileMap, NewPlayerX, NewPlayerY))
            {
                AppState->PlayerX = NewPlayerX;
                AppState->PlayerY = NewPlayerY;
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
            u32 TileID = GetTileValueUnchecked(TileMap, Column, Row);
            f32 Gray = 0.5f;
            if(TileID == 1)
            {
                Gray = 1.0f;
            }

            f32 MinX = TileMap->UpperLeftX + ((f32)Column)*TileMap->TileWidth;
            f32 MinY = TileMap->UpperLeftY + ((f32)Row)*TileMap->TileHeight;
            f32 MaxX = MinX + TileMap->TileWidth;
            f32 MaxY = MinY + TileMap->TileHeight;
            DrawRectangle(Buffer, MinX, MinY, MaxX, MaxY, Gray, Gray, Gray);
        }
    }
    
    f32 PlayerR = 1.0f;
    f32 PlayerG = 1.0f;
    f32 PlayerB = 0.0f;
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