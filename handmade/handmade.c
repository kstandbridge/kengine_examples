#define KENGINE_LIBRARY 1
#define KENGINE_IMMEDIATE 1
#define KENGINE_IMPLEMENTATION 1
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
DrawRectangle(offscreen_buffer *Buffer, v2 vMin, v2 vMax, f32 R, f32 G, f32 B)
{
    // TODO(kstandbridge): Color as float

    s32 MinX = RoundF32ToS32(vMin.X);
    s32 MinY = RoundF32ToS32(vMin.Y);
    s32 MaxX = RoundF32ToS32(vMax.X);
    s32 MaxY = RoundF32ToS32(vMax.Y);

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

internal void
DrawBitmap(offscreen_buffer *Buffer, loaded_bitmap *Bitmap,
           f32 RealX, f32 RealY,
           s32 AlignX, s32 AlignY)
{
    RealX -= (f32)AlignX;
    RealY -= (f32)AlignY;

    s32 MinX = RoundF32ToS32(RealX);
    s32 MinY = RoundF32ToS32(RealY);
    s32 MaxX = RoundF32ToS32(RealX + (f32)Bitmap->Width);
    s32 MaxY = RoundF32ToS32(RealY + (f32)Bitmap->Height);

    s32 SourceOffsetX = 0;
    if(MinX < 0)
    {
        SourceOffsetX = -MinX;
        MinX = 0;
    }

    s32 SourceOffsetY = 0;
    if(MinY < 0)
    {
        SourceOffsetY = -MinY;
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

    // TODO(kstandbridge): SourceRow needs to be changed based on clipping.
    u32 *SourceRow = Bitmap->Memory + Bitmap->Width*(Bitmap->Height - 1);
    SourceRow += -SourceOffsetY*Bitmap->Width + SourceOffsetX;
    u8 *DestRow = ((u8 *)Buffer->Memory +
                      MinX*Buffer->BytesPerPixel +
                      MinY*Buffer->Pitch);
    for(s32 Y = MinY;
        Y < MaxY;
        ++Y)
    {
        u32 *Dest = (u32 *)DestRow;
        u32 *Source = SourceRow;
        for(int X = MinX;
            X < MaxX;
            ++X)
        {            
            f32 A = (f32)((*Source >> 24) & 0xFF) / 255.0f;
            f32 SR = (f32)((*Source >> 16) & 0xFF);
            f32 SG = (f32)((*Source >> 8) & 0xFF);
            f32 SB = (f32)((*Source >> 0) & 0xFF);

            f32 DR = (f32)((*Dest >> 16) & 0xFF);
            f32 DG = (f32)((*Dest >> 8) & 0xFF);
            f32 DB = (f32)((*Dest >> 0) & 0xFF);

            // TODO(kstandbridge): Someday, we need to talk about premultiplied alpha!
            // (this is not premultiplied alpha)
            f32 R = (1.0f-A)*DR + A*SR;
            f32 G = (1.0f-A)*DG + A*SG;
            f32 B = (1.0f-A)*DB + A*SB;

            *Dest = (((u32)(R + 0.5f) << 16) |
                     ((u32)(G + 0.5f) << 8) |
                     ((u32)(B + 0.5f) << 0));
            
            ++Dest;
            ++Source;
        }

        DestRow += Buffer->Pitch;
        SourceRow -= Bitmap->Width;
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

        AppState->Backdrop = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_background.bmp")));

        hero_bitmaps *Bitmap = AppState->HeroBitmaps;

        Bitmap->Head = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_hero_right_head.bmp")));
        Bitmap->Cape = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_hero_right_cape.bmp")));
        Bitmap->Torso = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_hero_right_torso.bmp")));
        Bitmap->AlignX = 72;
        Bitmap->AlignY = 182;
        ++Bitmap;

        Bitmap->Head = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_hero_back_head.bmp")));
        Bitmap->Cape = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_hero_back_cape.bmp")));
        Bitmap->Torso = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_hero_back_torso.bmp")));
        Bitmap->AlignX = 72;
        Bitmap->AlignY = 182;
        ++Bitmap;

        Bitmap->Head = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_hero_left_head.bmp")));
        Bitmap->Cape = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_hero_left_cape.bmp")));
        Bitmap->Torso = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_hero_left_torso.bmp")));
        Bitmap->AlignX = 72;
        Bitmap->AlignY = 182;
        ++Bitmap;

        Bitmap->Head = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_hero_front_head.bmp")));
        Bitmap->Cape = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_hero_front_cape.bmp")));
        Bitmap->Torso = LoadBMP(PlatformReadEntireFile(&AppState->Arena, String("test/test_hero_front_torso.bmp")));
        Bitmap->AlignX = 72;
        Bitmap->AlignY = 182;
        ++Bitmap;

        AppState->CameraP.AbsTileX = 17/2;
        AppState->CameraP.AbsTileY = 9/2;

        // TODO(kstandbridge): Make random?
        AppState->RandomState.Value = 1234;

        AppState->PlayerP.AbsTileX = 1;
        AppState->PlayerP.AbsTileY = 3;
        AppState->PlayerP.Offset.X = 5.0f;
        AppState->PlayerP.Offset.Y = 5.0f;

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

            b32 CreatedZDoor = false;
            if(RandomChoice == 2)
            {
                CreatedZDoor = true;
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

            if(CreatedZDoor)
            {
                DoorDown = !DoorDown;
                DoorUp = !DoorUp;
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
            v2 ddPlayer = V2Set1(0);
            
            if(Controller->MoveUp.EndedDown)
            {
                AppState->HeroFacingDirection = 1;
                ddPlayer.Y = 1.0f;
            }
            if(Controller->MoveDown.EndedDown)
            {
                AppState->HeroFacingDirection = 3;
                ddPlayer.Y = -1.0f;
            }
            if(Controller->MoveLeft.EndedDown)
            {
                AppState->HeroFacingDirection = 2;
                ddPlayer.X = -1.0f;
            }
            if(Controller->MoveRight.EndedDown)
            {
                AppState->HeroFacingDirection = 0;
                ddPlayer.X = 1.0f;
            }
            
            if((ddPlayer.X != 0.0f) && (ddPlayer.Y != 0.0f))
            {
                ddPlayer = V2MultiplyScalar(ddPlayer, 0.707106781187f);
            }

            f32 PlayerSpeed = 10.0f;
            if(Controller->ActionUp.EndedDown)
            {
                PlayerSpeed = 50.0f;
            }
            ddPlayer = V2MultiplyScalar(ddPlayer, PlayerSpeed);

            // TODO(kstandbridge): ODE here!
            ddPlayer = V2Add(ddPlayer, V2MultiplyScalar(AppState->dPlayerP, -1.5f));
            
            // TODO(kstandbridge): Diagonal will be faster!  Fix once we have vectors :)
            tile_map_position NewPlayerP = AppState->PlayerP;
            NewPlayerP.Offset = (V2Add(V2MultiplyScalar(V2MultiplyScalar(ddPlayer, 0.5f), Square(Input->dtForFrame)),
                                       V2Add(V2MultiplyScalar(AppState->dPlayerP, Input->dtForFrame),
                                             NewPlayerP.Offset)));
            AppState->dPlayerP = V2Add(V2MultiplyScalar(ddPlayer, Input->dtForFrame), AppState->dPlayerP);

            NewPlayerP = RecanonicalizePosition(TileMap, NewPlayerP);
            // TODO(kstandbridge): Delta function that auto recanonicalizes

            tile_map_position PlayerLeft = NewPlayerP;
            PlayerLeft.Offset.X -= 0.5f*PlayerWidth;
            PlayerLeft = RecanonicalizePosition(TileMap, PlayerLeft);

            tile_map_position PlayerRight = NewPlayerP;
            PlayerRight.Offset.X += 0.5f*PlayerWidth;
            PlayerRight = RecanonicalizePosition(TileMap, PlayerRight);

            b32 Collided = false;
            tile_map_position ColP = {0};
            if(!IsTileMapPointEmpty(TileMap, NewPlayerP))
            {
                ColP = NewPlayerP;
                Collided = true;
            }
            if(!IsTileMapPointEmpty(TileMap, PlayerLeft))
            {
                ColP = PlayerLeft;
                Collided = true;
            }
            if(!IsTileMapPointEmpty(TileMap, PlayerRight))
            {
                ColP = PlayerRight;
                Collided = true;
            }

            if(Collided)
            {
                v2 r = V2Set1(0);
                if(ColP.AbsTileX < AppState->PlayerP.AbsTileX)
                {
                    r = V2(1, 0);
                }
                if(ColP.AbsTileX > AppState->PlayerP.AbsTileX)
                {
                    r = V2(-1, 0);
                }
                if(ColP.AbsTileY < AppState->PlayerP.AbsTileY)
                {
                    r = V2(0, 1);
                }
                if(ColP.AbsTileY > AppState->PlayerP.AbsTileY)
                {
                    r = V2(0, -1);
                }

                AppState->dPlayerP = V2Subtract(AppState->dPlayerP, V2MultiplyScalar(r, Inner(AppState->dPlayerP, r)));
            }
            else
            {
                if(!AreOnSameTile(&AppState->PlayerP, &NewPlayerP))
                {
                    u32 NewTileValue = GetTileValue(TileMap, NewPlayerP);

                    if(NewTileValue == 3)
                    {
                        ++NewPlayerP.AbsTileZ;
                    }
                    else if(NewTileValue == 4)
                    {
                        --NewPlayerP.AbsTileZ;
                    }
                }

                AppState->PlayerP = NewPlayerP;
            }

            AppState->CameraP.AbsTileZ = AppState->PlayerP.AbsTileZ;

            tile_map_difference Diff = Subtract(TileMap, &AppState->PlayerP, &AppState->CameraP);
            if(Diff.dXY.X > (9.0f*TileMap->TileSideInMeters))
            {
                AppState->CameraP.AbsTileX += 17;
            }
            if(Diff.dXY.X < -(9.0f*TileMap->TileSideInMeters))
            {
                AppState->CameraP.AbsTileX -= 17;
            }
            if(Diff.dXY.Y > (5.0f*TileMap->TileSideInMeters))
            {
                AppState->CameraP.AbsTileY += 9;
            }
            if(Diff.dXY.Y < -(5.0f*TileMap->TileSideInMeters))
            {
                AppState->CameraP.AbsTileY -= 9;
            }
        }
    }
    
    DrawBitmap(Buffer, &AppState->Backdrop, 0, 0, 0, 0);

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
            u32 Column = AppState->CameraP.AbsTileX + RelColumn;
            u32 Row = AppState->CameraP.AbsTileY + RelRow;
            u32 TileID = GetTileValue_(TileMap, Column, Row, AppState->CameraP.AbsTileZ);

            if(TileID > 1)
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

                if((Column == AppState->CameraP.AbsTileX) &&
                   (Row == AppState->CameraP.AbsTileY))
                {
                    Gray = 0.0f;
                }

                v2 TileSide = V2Set1(0.5f*TileSideInPixels);
                v2 Cen = V2(ScreenCenterX - MetersToPixels*AppState->CameraP.Offset.X + ((f32)RelColumn)*TileSideInPixels,
                            ScreenCenterY + MetersToPixels*AppState->CameraP.Offset.Y - ((f32)RelRow)*TileSideInPixels);
                v2 Min = V2Subtract(Cen, TileSide);
                v2 Max = V2Add(Cen, TileSide);
                DrawRectangle(Buffer, Min, Max, Gray, Gray, Gray);
            }
        }
    }

    tile_map_difference Diff = Subtract(TileMap, &AppState->PlayerP, &AppState->CameraP);
    
    f32 PlayerR = 1.0f;
    f32 PlayerG = 1.0f;
    f32 PlayerB = 0.0f;
    f32 PlayerGroundPointX = ScreenCenterX + MetersToPixels*Diff.dXY.X;
    f32 PlayerGroundPointY = ScreenCenterY - MetersToPixels*Diff.dXY.Y;
    v2 PlayerLeftTop = V2(PlayerGroundPointX - 0.5f*MetersToPixels*PlayerWidth,
                           PlayerGroundPointY - MetersToPixels*PlayerHeight);
    v2 PlayerWidthHeight = V2(PlayerWidth, PlayerHeight);
    DrawRectangle(Buffer,
                  PlayerLeftTop,
                  V2Add(PlayerLeftTop, V2MultiplyScalar(PlayerWidthHeight, MetersToPixels)),
                  PlayerR, PlayerG, PlayerB);

    hero_bitmaps *HeroBitmaps = &AppState->HeroBitmaps[AppState->HeroFacingDirection];
    DrawBitmap(Buffer, &HeroBitmaps->Torso, PlayerGroundPointX, PlayerGroundPointY, HeroBitmaps->AlignX, HeroBitmaps->AlignY);
    DrawBitmap(Buffer, &HeroBitmaps->Cape, PlayerGroundPointX, PlayerGroundPointY, HeroBitmaps->AlignX, HeroBitmaps->AlignY);
    DrawBitmap(Buffer, &HeroBitmaps->Head, PlayerGroundPointX, PlayerGroundPointY, HeroBitmaps->AlignX, HeroBitmaps->AlignY);
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
