#define KENGINE_LIBRARY 1
#define KENGINE_IMMEDIATE 1
#define KENGINE_IMPLEMENTATION 1
#include "kengine.h"

#include <math.h>

#include "handmade.h"
#include "handmade_world.c"
#include "handmade_random.h"

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
           s32 AlignX, s32 AlignY,
           f32 CAlpha)
{
    RealX -= (f32)AlignX;
    RealY -= (f32)AlignY;

    s32 MinX = RoundF32ToS32(RealX);
    s32 MinY = RoundF32ToS32(RealY);
    s32 MaxX = MinX + Bitmap->Width;
    s32 MaxY = MinY + Bitmap->Height;

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
            A *= CAlpha;

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

internal low_entity *
GetLowEntity(app_state *AppState, u32 Index)
{
    low_entity *Result = 0;

    if((Index > 0) && (Index < AppState->LowEntityCount))
    {
        Result = AppState->LowEntities + Index;
    }
    
    return Result;
}

internal high_entity *
MakeEntityHighFrequency(app_state *AppState, u32 LowIndex)
{
    high_entity *Result = 0;

    low_entity *EntityLow = AppState->LowEntities + LowIndex;
    if(EntityLow->HighEntityIndex)
    {
        Result = AppState->HighEntities_ + EntityLow->HighEntityIndex;
    }
    else 
    {
        if(AppState->HighEntityCount < ArrayCount(AppState->HighEntities_))
        {
            u32 HighIndex = AppState->HighEntityCount++;
            Result = AppState->HighEntities_ + HighIndex;

            // NOTE(Kstandbridge): Map the entity into camera space
            world_difference Diff = Subtract(AppState->World,
                                             &EntityLow->P, &AppState->CameraP);
            Result->P = Diff.dXY;
            Result->dP = V2Set1(0);
            Result->AbsTileZ = EntityLow->P.AbsTileZ;
            Result->FacingDirection = 0;
            Result->LowEntityIndex = LowIndex;

            EntityLow->HighEntityIndex = HighIndex;
        }
        else 
        {
            InvalidCodePath;
        }
    }

    return Result;
}

internal entity
GetHighEntity(app_state *AppState, u32 LowIndex)
{
    entity Result = {0};

    if((LowIndex > 0) && (LowIndex < AppState->LowEntityCount))
    {
        Result.LowIndex = LowIndex;
        Result.Low = AppState->LowEntities + LowIndex;
        Result.High = MakeEntityHighFrequency(AppState, LowIndex);
    }

    return Result;
}

internal void
MakeEntityLowFrequency(app_state *AppState, u32 LowIndex)
{
    low_entity *EntityLow = AppState->LowEntities + LowIndex;
    u32 HighIndex = EntityLow->HighEntityIndex;
    if(HighIndex)
    {
        u32 LastHighIndex = AppState->HighEntityCount - 1;
        if(HighIndex != LastHighIndex)
        {
            high_entity *LastEntity = AppState->HighEntities_ + LastHighIndex;
            high_entity *DelEntity = AppState->HighEntities_ + HighIndex;

            *DelEntity = *LastEntity;
            AppState->LowEntities[LastEntity->LowEntityIndex].HighEntityIndex = HighIndex;
        }
        --AppState->HighEntityCount;
        EntityLow->HighEntityIndex = 0;
    }
}

internal void
OffsetAndCheckFrequencyhByArena(app_state *AppState, v2 Offset, rectangle2 HighFrequencyBounds)
{
    for(u32 EntityIndex = 1;
        EntityIndex < AppState->HighEntityCount;
        )
    {
        high_entity *High = AppState->HighEntities_ + EntityIndex;

        High->P = V2Add(High->P, Offset);
        if(Rectangle2IsIn(HighFrequencyBounds, High->P))
        {
            ++EntityIndex;
        }
        else 
        {
            MakeEntityLowFrequency(AppState, High->LowEntityIndex);
        }
    }
}

internal u32
AddLowEntity(app_state *AppState, entity_type Type)
{
    Assert(AppState->LowEntityCount < ArrayCount(AppState->LowEntities));
    u32 Result = AppState->LowEntityCount++;

    ZeroStruct(AppState->LowEntities[Result]);
    AppState->LowEntities[Result].Type = Type;

    return Result;
}

internal u32
AddWall(app_state *AppState, u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ)
{
    u32 Result = AddLowEntity(AppState, EntityType_Wall);
    low_entity *EntityLow = GetLowEntity(AppState, Result);

    EntityLow->P.AbsTileX = AbsTileX;
    EntityLow->P.AbsTileY = AbsTileY;
    EntityLow->P.AbsTileZ = AbsTileZ;
    EntityLow->Height = AppState->World->TileSideInMeters;
    EntityLow->Width = EntityLow->Height;
    EntityLow->Collides = true;

    return Result;
}

internal u32
AddPlayer(app_state *AppState)
{
    u32 Result = AddLowEntity(AppState, EntityType_Hero);
    low_entity *EntityLow = GetLowEntity(AppState, Result);

    EntityLow->P = AppState->CameraP;
    EntityLow->P.Offset_.X = 0;
    EntityLow->P.Offset_.Y = 0;
    EntityLow->Height = 0.5f; // 1.4f;
    EntityLow->Width = 1.0f;
    EntityLow->Collides = true;

    if(AppState->CameraFollowingEntityIndex == 0)
    {
        AppState->CameraFollowingEntityIndex = Result;
    }

    return Result;
}

internal b32
TestWall(f32 WallX, f32 RelX, f32 RelY, f32 PlayerDeltaX, f32 PlayerDeltaY,
         f32 *tMin, f32 MinY, f32 MaxY)
{
    b32 Result = false;

    f32 tEpsilon = 0.001f;
    if(PlayerDeltaX != 0.0f)
    {
        f32 tResult = (WallX - RelX) / PlayerDeltaX;
        f32 Y = RelY + tResult*PlayerDeltaY;
        if((tResult >= 0.0f) && (*tMin > tResult))
        {
            if((Y >= MinY) && (Y <= MaxY))
            {
                *tMin = Maximum(0.0f, tResult - tEpsilon);
                Result = true;
            }
        }
    }

    return Result;
}

internal void
MovePlayer(app_state *AppState, entity Entity, f32 dt, v2 ddP)
{
    // world *World = AppState->World;

    f32 ddPLength = LengthSq(ddP);
    if((ddP.X != 0.0f) && (ddP.Y != 0.0f))
    {
        ddP = V2MultiplyScalar(ddP, 1.0f / SquareRoot(ddPLength));
    }

    f32 PlayerSpeed = 50.0f; // m/s^2
    ddP = V2MultiplyScalar(ddP, PlayerSpeed);

    // TODO(kstandbridge): ODE here!
    ddP = V2Add(ddP, V2MultiplyScalar(Entity.High->dP, -8.0f));
    
    // v2 OldPlayerP = Entity.High->P;
    v2 PlayerDelta = V2Add(V2MultiplyScalar(V2MultiplyScalar(ddP, 0.5f), Square(dt)),
                           V2MultiplyScalar(Entity.High->dP, dt));
    Entity.High->dP = V2Add(V2MultiplyScalar(ddP, dt), Entity.High->dP);
    // v2 NewPlayerP = V2Add(OldPlayerP, PlayerDelta);

/*
    u32 MinTileX = Minimum(OldPlayerP.AbsTileX, NewPlayerP.AbsTileX);
    u32 MinTileY = Minimum(OldPlayerP.AbsTileY, NewPlayerP.AbsTileY);
    u32 MaxTileX = Maximum(OldPlayerP.AbsTileX, NewPlayerP.AbsTileX);
    u32 MaxTileY = Maximum(OldPlayerP.AbsTileY, NewPlayerP.AbsTileY);

    u32 EntityTileWidth = CeilF32ToS32(Entity.High->Width / World->TileSideInMeters);
    u32 EntityTileHeight = CeilF32ToS32(Entity.High->Height / World->TileSideInMeters);

    MinTileX -= EntityTileWidth;
    MinTileY -= EntityTileHeight;
    MaxTileX += EntityTileWidth;
    MaxTileY += EntityTileHeight;

    u32 AbsTileZ = Entity.High->P.AbsTileZ;
*/
    
    for(u32 Iteration = 0;
        Iteration < 4;
        ++Iteration)
    {
        f32 tMin = 1.0f;
        v2 WallNormal = V2Set1(0);
        u32 HitHighEntityIndex = 0;

        v2 DesiredPosition = V2Add(Entity.High->P, PlayerDelta);

        for(u32 TestHighEntityIndex = 0;
            TestHighEntityIndex < AppState->HighEntityCount;
            ++TestHighEntityIndex)
        {
            if(TestHighEntityIndex != Entity.Low->HighEntityIndex)
            {
                entity TestEntity;
                TestEntity.High = AppState->HighEntities_ + TestHighEntityIndex;
                TestEntity.LowIndex = TestEntity.High->LowEntityIndex;
                TestEntity.Low = AppState->LowEntities + TestEntity.LowIndex;
                if(TestEntity.Low->Collides)
                {
                    f32 DiameterW = TestEntity.Low->Width + Entity.Low->Width;
                    f32 DiameterH = TestEntity.Low->Height + Entity.Low->Height;

                    v2 MinCorner = V2MultiplyScalar(V2(DiameterW, DiameterH), -0.5f);
                    v2 MaxCorner = V2MultiplyScalar(V2(DiameterW, DiameterH), 0.5f);

                    v2 Rel = V2Subtract(Entity.High->P, TestEntity.High->P);

                    if(TestWall(MinCorner.X, Rel.X, Rel.Y, PlayerDelta.X, PlayerDelta.Y,
                            &tMin, MinCorner.Y, MaxCorner.Y))
                    {
                        WallNormal = V2(-1, 0);
                        HitHighEntityIndex = TestHighEntityIndex;
                    }
                    if(TestWall(MaxCorner.X, Rel.X, Rel.Y, PlayerDelta.X, PlayerDelta.Y,
                            &tMin, MinCorner.Y, MaxCorner.Y))
                    {
                        WallNormal = V2(1, 0);
                        HitHighEntityIndex = TestHighEntityIndex;
                    }
                    if(TestWall(MinCorner.Y, Rel.Y, Rel.X, PlayerDelta.Y, PlayerDelta.X,
                            &tMin, MinCorner.X, MaxCorner.X))
                    {
                        WallNormal = V2(0, -1);
                        HitHighEntityIndex = TestHighEntityIndex;
                    }
                    if(TestWall(MaxCorner.Y, Rel.Y, Rel.X, PlayerDelta.Y, PlayerDelta.X,
                            &tMin, MinCorner.X, MaxCorner.X))
                    {
                        WallNormal = V2(0, 1);
                        HitHighEntityIndex = TestHighEntityIndex;
                    }
                }
            }
        }

        Entity.High->P = V2Add(Entity.High->P, V2MultiplyScalar(PlayerDelta, tMin));
        if(HitHighEntityIndex)
        {
            Entity.High->dP = V2Subtract(Entity.High->dP, V2MultiplyScalar(WallNormal, Inner(Entity.High->dP, WallNormal)));
            PlayerDelta = V2Subtract(DesiredPosition, Entity.High->P);
            PlayerDelta = V2Subtract(PlayerDelta, V2MultiplyScalar(WallNormal, Inner(PlayerDelta, WallNormal)));

            high_entity *HitHigh = AppState->HighEntities_ + HitHighEntityIndex;
            low_entity *HitLow = AppState->LowEntities + HitHigh->LowEntityIndex;
            Entity.High->AbsTileZ += HitLow->dAbsTileZ;
        }
        else 
        {
            break;
        }
    }

    // TODO(kstandbridge): Change to using the acceleration vector
    if((Entity.High->dP.X == 0.0f) && (Entity.High->dP.Y == 0.0f))
    {
        // NOTE(kstandbridge): Leave FacingDirection whatever it was
    }
    else if(AbsoluteValue(Entity.High->dP.X) > AbsoluteValue(Entity.High->dP.Y))
    {
        if(Entity.High->dP.X > 0)
        {
            Entity.High->FacingDirection = 0;
        }
        else 
        {
            Entity.High->FacingDirection = 2;
        }
    }
    else 
    {
        if(Entity.High->dP.Y > 0)
        {
            Entity.High->FacingDirection = 1;
        }
        else
        {
            Entity.High->FacingDirection = 3;
        }
    }

    Entity.Low->P = MapIntoTileSpace(AppState->World, AppState->CameraP, Entity.High->P);
}

internal void
SetCamera(app_state *AppState, world_position NewCameraP)
{
    world *World = AppState->World;

    world_difference dCameraP = Subtract(World, &NewCameraP, &AppState->CameraP);
    AppState->CameraP = NewCameraP;

    // TODO(kstandbridge): I am totally picking these numbers randomly!
    u32 TileSpanX = 17*3;
    u32 TileSpanY = 9*3;
    rectangle2 CameraBounds = Rectangle2CenterDim(V2Set1(0),
                                                  V2MultiplyScalar(V2(TileSpanX, TileSpanY), World->TileSideInMeters));
    v2 EntityOffsetForFrame = V2Invert(dCameraP.dXY);
    OffsetAndCheckFrequencyhByArena(AppState, EntityOffsetForFrame, CameraBounds);

    // TODO(kstandbridge): This needs to be accelerated, but man, this CPU is crazy fast!
    s32 MinTileX = NewCameraP.AbsTileX - TileSpanX/2;
    s32 MaxTileX = NewCameraP.AbsTileX + TileSpanX/2;
    s32 MinTileY = NewCameraP.AbsTileY - TileSpanY/2;
    s32 MaxTileY = NewCameraP.AbsTileY + TileSpanY/2;
    for(u32 EntityIndex = 1;
        EntityIndex < AppState->LowEntityCount;
        ++EntityIndex)
    {
        low_entity *Low = AppState->LowEntities + EntityIndex;
        if(Low->HighEntityIndex == 0)
        {
            if((Low->P.AbsTileZ == NewCameraP.AbsTileZ) &&
               (Low->P.AbsTileX >= MinTileX) &&
               (Low->P.AbsTileX <= MaxTileX) &&
               (Low->P.AbsTileY >= MinTileY) &&
               (Low->P.AbsTileY <= MaxTileY))
            {
                MakeEntityHighFrequency(AppState, EntityIndex);
            }
        }
    }
}

extern void 
AppUpdateAndRender(app_memory *AppMemory, app_input *Input, offscreen_buffer *Buffer)
{
    Platform = AppMemory->PlatformAPI;
    
    Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) ==
        (ArrayCount(Input->Controllers[0].Buttons)));

#if 0
    f32 LowerLeftX = -(f32)World.TileSideInPixels/2.0f;
    f32 LowerLeftY = (f32)Buffer->Height;
#endif

    app_state *AppState = AppMemory->AppState;
    if(AppState == 0)
    {
        AppState = AppMemory->AppState = BootstrapPushStruct(app_state, WorldArena);

        // NOTE(kstandbridge): Reserve entity slot 0 for the null entity
        AddLowEntity(AppState, EntityType_Null);
        AppState->HighEntityCount = 1;

        AppState->Backdrop = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_background.bmp")));
        AppState->Shadow = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_shadow.bmp")));

        hero_bitmaps *Bitmap = AppState->HeroBitmaps;

        Bitmap->Head = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_right_head.bmp")));
        Bitmap->Cape = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_right_cape.bmp")));
        Bitmap->Torso = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_right_torso.bmp")));
        Bitmap->AlignX = 72;
        Bitmap->AlignY = 182;
        ++Bitmap;

        Bitmap->Head = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_back_head.bmp")));
        Bitmap->Cape = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_back_cape.bmp")));
        Bitmap->Torso = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_back_torso.bmp")));
        Bitmap->AlignX = 72;
        Bitmap->AlignY = 182;
        ++Bitmap;

        Bitmap->Head = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_left_head.bmp")));
        Bitmap->Cape = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_left_cape.bmp")));
        Bitmap->Torso = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_left_torso.bmp")));
        Bitmap->AlignX = 72;
        Bitmap->AlignY = 182;
        ++Bitmap;

        Bitmap->Head = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_front_head.bmp")));
        Bitmap->Cape = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_front_cape.bmp")));
        Bitmap->Torso = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_front_torso.bmp")));
        Bitmap->AlignX = 72;
        Bitmap->AlignY = 182;
        ++Bitmap;

        AppState->World = PushStruct(&AppState->WorldArena, world);
        world *World = AppState->World;
        InitializeWorld(World, 1.4f);

        u32 RandomNumberIndex = 0;
        u32 TilesPerWidth = 17;
        u32 TilesPerHeight = 9;

        u32 ScreenBaseX = 0;
        u32 ScreenBaseY = 0;
        u32 ScreenBaseZ = 0;
        u32 ScreenX = ScreenBaseX;
        u32 ScreenY = ScreenBaseY;
        u32 AbsTileZ = ScreenBaseZ;

        // TODO(kstandbridge): Replace all this with real world generation!
        b32 DoorLeft = false;
        b32 DoorRight = false;
        b32 DoorTop = false;
        b32 DoorBottom = false;
        b32 DoorUp = false;
        b32 DoorDown = false;
        for(u32 ScreenIndex = 0;
            ScreenIndex < 2;
            ++ScreenIndex)
        {
            // TODO(kstandbridge): Random number generator!
            Assert(RandomNumberIndex < ArrayCount(RandomNumberTable));

            u32 RandomChoice;
            // if(DoorUp || DoorDown)
            {
                RandomChoice = RandomNumberTable[RandomNumberIndex++] % 2;
            }
#if 0
            else 
            {
                RandomChoice = RandomNumberTable[RandomNumberIndex++] % 3;
            }
#endif

            b32 CreatedZDoor = false;
            if(RandomChoice == 2)
            {
                CreatedZDoor = true;
                if(AbsTileZ == ScreenBaseZ)
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

                    if(TileValue == 2)
                    {
                        AddWall(AppState, AbsTileX, AbsTileY, AbsTileZ);
                    }
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
                if(AbsTileZ == ScreenBaseZ)
                {
                    AbsTileZ = ScreenBaseZ + 1;
                }
                else
                {
                    AbsTileZ = ScreenBaseZ;
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
#if 0 
        while(AppState->LowEntityCount < (ArrayCount(AppState->LowEntities) - 16))
        {
            u32 Coordinate = 1024 + AppState->LowEntityCount;
            AddWall(AppState, Coordinate, Coordinate, Coordinate);
        }
#endif

        world_position NewCameraP =
        {
            .AbsTileX = ScreenBaseX*TilesPerWidth + 17/2,
            .AbsTileY = ScreenBaseY*TilesPerHeight + 9/2,
            .AbsTileZ = ScreenBaseZ,
        };
        SetCamera(AppState, NewCameraP);
    }

    world *World = AppState->World;

    s32 TileSideInPixels = 60;
    f32 MetersToPixels = (f32)TileSideInPixels / (f32)World->TileSideInMeters;

    // f32 LowerLeftX = -(f32)TileSideInPixels/2;
    // f32 LowerLeftY = (f32)Buffer->Height;
    // tile_map_position OldPlayerP = AppState->PlayerP;
 
    for(int ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ++ControllerIndex)
    {
        controller_input *Controller = GetController(Input, ControllerIndex);
        u32 LowIndex = AppState->PlayerIndexForController[ControllerIndex];

        if(LowIndex == 0)
        {
            if(Controller->Start.EndedDown)
            {
                u32 EntityIndex = AddPlayer(AppState);
                AppState->PlayerIndexForController[ControllerIndex] = EntityIndex;
            }
        }
        else 
        {
            entity ControllingEntity = GetHighEntity(AppState, LowIndex);
            v2 ddP = V2Set1(0);

            if(Controller->IsAnalog)
            {
                // NOTE(kstandbridge): Use analog movement tuning
                ddP = V2(Controller->StickAverageX, Controller->StickAverageY);
            }
            else
            {
                // NOTE(kstandbridge): Use digital movement tuning
                if(Controller->MoveUp.EndedDown)
                {
                    ddP.Y = 1.0f;
                }
                if(Controller->MoveDown.EndedDown)
                {
                    ddP.Y = -1.0f;
                }
                if(Controller->MoveLeft.EndedDown)
                {
                    ddP.X = -1.0f;
                }
                if(Controller->MoveRight.EndedDown)
                {
                    ddP.X = 1.0f;
                }
            }

            if(Controller->ActionUp.EndedDown)
            {
                ControllingEntity.High->dZ = 3.0f;
            }

            MovePlayer(AppState, ControllingEntity, Input->dtForFrame, ddP);
        }
    }

    v2 EntityOffsetForFrame = V2Set1(0);
    entity CameraFollowingEntity = GetHighEntity(AppState, AppState->CameraFollowingEntityIndex);
    if(CameraFollowingEntity.High)
    {
        world_position NewCameraP = AppState->CameraP;

        NewCameraP.AbsTileZ = CameraFollowingEntity.Low->P.AbsTileZ;

#if 0
        if(CameraFollowingEntity.High->P.X > (9.0f*World->TileSideInMeters))
        {
            AppState->CameraP.AbsTileX += 17;
        }
        if(CameraFollowingEntity.High->P.X < -(9.0f*World->TileSideInMeters))
        {
            AppState->CameraP.AbsTileX -= 17;
        }
        if(CameraFollowingEntity.High->P.Y > (5.0f*World->TileSideInMeters))
        {
            AppState->CameraP.AbsTileY += 9;
        }
        if(CameraFollowingEntity.High->P.Y < -(5.0f*World->TileSideInMeters))
        {
            AppState->CameraP.AbsTileY -= 9;
        }
#else
        NewCameraP = CameraFollowingEntity.Low->P;
#endif
        
        // TODO(kstandbridge): Map new entities in and old entities out!!!
        // TODO(kstandbridge): Mapping tiles and stairs into the entity set!
        
        SetCamera(AppState, NewCameraP);
    }
    
    // 
    // NOTE(kstandbridge): Render 
    //
    DrawBitmap(Buffer, &AppState->Backdrop, 0, 0, 0, 0, 1);

    f32 ScreenCenterX = 0.5f*(f32)Buffer->Width;
    f32 ScreenCenterY = 0.5f*(f32)Buffer->Height;

#if 0
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
            u32 TileID = GetTileValue_(World, Column, Row, AppState->CameraP.AbsTileZ);

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
                v2 Cen = V2(ScreenCenterX - MetersToPixels*AppState->CameraP.Offset_.X + ((f32)RelColumn)*TileSideInPixels,
                            ScreenCenterY + MetersToPixels*AppState->CameraP.Offset_.Y - ((f32)RelRow)*TileSideInPixels);
                v2 Min = V2Subtract(Cen, V2MultiplyScalar(TileSide, 0.9f));
                v2 Max = V2Add(Cen, V2MultiplyScalar(TileSide, 0.9f));
                DrawRectangle(Buffer, Min, Max, Gray, Gray, Gray);
            }
        }
    }
#endif

    for(u32 HighEntityIndex = 1;
        HighEntityIndex < AppState->HighEntityCount;
        ++HighEntityIndex)
    {
        high_entity *HighEntity = AppState->HighEntities_ + HighEntityIndex;
        low_entity *LowEntity = AppState->LowEntities + HighEntity->LowEntityIndex;

        HighEntity->P = V2Add(HighEntity->P, EntityOffsetForFrame);

        f32 dt = Input->dtForFrame;
        f32 ddZ = -9.8f;
        HighEntity->Z = 0.5f*ddZ*Square(dt) + HighEntity->dZ*dt + HighEntity->Z;
        HighEntity->dZ = ddZ*dt + HighEntity->dZ;
        if(HighEntity->Z < 0)
        {
            HighEntity->Z = 0;
        }
        f32 CAlpha = 1.0f - 0.5f*HighEntity->Z;
        if(CAlpha < 0)
        {
            CAlpha = 0.0f;
        }

        f32 PlayerR = 1.0f;
        f32 PlayerG = 1.0f;
        f32 PlayerB = 0.0f;
        f32 PlayerGroundPointX = ScreenCenterX + MetersToPixels*HighEntity->P.X;
        f32 PlayerGroundPointY = ScreenCenterY - MetersToPixels*HighEntity->P.Y;
        f32 Z = -MetersToPixels*HighEntity->Z;
        v2 PlayerLeftTop = V2(PlayerGroundPointX - 0.5f*MetersToPixels*LowEntity->Width,
                              PlayerGroundPointY - 0.5f*MetersToPixels*LowEntity->Height);
        v2 EntityWidthHeight = V2(LowEntity->Width, LowEntity->Height);

        if(LowEntity->Type == EntityType_Hero)
        {
            hero_bitmaps *HeroBitmaps = &AppState->HeroBitmaps[HighEntity->FacingDirection];

            DrawBitmap(Buffer, &AppState->Shadow, PlayerGroundPointX, PlayerGroundPointY, HeroBitmaps->AlignX, HeroBitmaps->AlignY, CAlpha);
            DrawBitmap(Buffer, &HeroBitmaps->Torso, PlayerGroundPointX, PlayerGroundPointY + Z, HeroBitmaps->AlignX, HeroBitmaps->AlignY, 1.0f);
            DrawBitmap(Buffer, &HeroBitmaps->Cape, PlayerGroundPointX, PlayerGroundPointY + Z, HeroBitmaps->AlignX, HeroBitmaps->AlignY, 1.0f);
            DrawBitmap(Buffer, &HeroBitmaps->Head, PlayerGroundPointX, PlayerGroundPointY + Z, HeroBitmaps->AlignX, HeroBitmaps->AlignY, 1.0f);
        }
        else 
        {
            DrawRectangle(Buffer,
                          PlayerLeftTop,
                          V2Add(PlayerLeftTop, V2MultiplyScalar(EntityWidthHeight, MetersToPixels)),
                          PlayerR, PlayerG, PlayerB);
        }
    }
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
