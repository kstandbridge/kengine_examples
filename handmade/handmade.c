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
           f32 RealX, f32 RealY, f32 CAlpha)
{
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

internal v2 
GetCameraSpaceP(app_state *AppState, low_entity *EntityLow)
{
    // NOTE(kstandbridge): Map the entity into camera space
    world_difference Diff = Subtract(AppState->World, &EntityLow->P, &AppState->CameraP);
    v2 Result = Diff.dXY;

    return Result;
}

internal high_entity *
MakeEntityHighFrequency_(app_state *AppState, low_entity *EntityLow, u32 LowIndex, v2 CameraSpaceP)
{
    high_entity *Result = 0;

    Assert(EntityLow->HighEntityIndex == 0);

    if(EntityLow->HighEntityIndex == 0)
    {
        if(AppState->HighEntityCount < ArrayCount(AppState->HighEntities_))
        {
            u32 HighIndex = AppState->HighEntityCount++;
            Result = AppState->HighEntities_ + HighIndex;

            Result->P = CameraSpaceP;
            Result->dP = V2Set1(0);
            Result->ChunkZ = EntityLow->P.ChunkZ;
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
        v2 CameraSpaceP = GetCameraSpaceP(AppState, EntityLow);
        Result = MakeEntityHighFrequency_(AppState, EntityLow, LowIndex, CameraSpaceP);
    }

    return Result;
}

internal entity
ForceEnmtityIntoHigh(app_state *AppState, u32 LowIndex)
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

internal b32 
ValidateEntityPairs(app_state *AppState)
{
    b32 Result = true;

    for(u32 HighEntityIndex = 1;
        HighEntityIndex < AppState->HighEntityCount;
        ++HighEntityIndex)
    {
        high_entity *High = AppState->HighEntities_ + HighEntityIndex;
        Result = Result &&
                 (AppState->LowEntities[High->LowEntityIndex].HighEntityIndex == HighEntityIndex);
    }

    return Result;
}

internal void
OffsetAndCheckFrequencyhByArena(app_state *AppState, v2 Offset, rectangle2 HighFrequencyBounds)
{
    for(u32 HighEntityIndex = 1;
        HighEntityIndex < AppState->HighEntityCount;
        )
    {
        high_entity *High = AppState->HighEntities_ + HighEntityIndex;

        High->P = V2Add(High->P, Offset);
        if(Rectangle2IsIn(HighFrequencyBounds, High->P))
        {
            ++HighEntityIndex;
        }
        else 
        {
            Assert(AppState->LowEntities[High->LowEntityIndex].HighEntityIndex == HighEntityIndex);
            MakeEntityLowFrequency(AppState, High->LowEntityIndex);
        }
    }
}

typedef struct add_low_entity_result
{
    low_entity *Low;
    u32 LowIndex;
} add_low_entity_result;

internal add_low_entity_result
AddLowEntity(app_state *AppState, entity_type Type, world_position *P)
{
    Assert(AppState->LowEntityCount < ArrayCount(AppState->LowEntities));
    u32 EntityIndex = AppState->LowEntityCount++;

    low_entity *EntityLow = AppState->LowEntities + EntityIndex;
    ZeroStruct(*EntityLow);
    EntityLow->Type = Type;

    ChangeEntityLocation(&AppState->WorldArena, AppState->World, EntityIndex, EntityLow, 0, P);

    add_low_entity_result Result = 
    {
        .Low = EntityLow,
        .LowIndex = EntityIndex,
    };

    // TODO(kstandbridge): Do we need to have a begin/end paradigm for adding
    // entities so that they can be brought into the high set when they 
    // are added anmd are in the camera region?

    return Result;
}

internal add_low_entity_result
AddWall(app_state *AppState, u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ)
{
    world_position P = ChunkPositionFromTilePosition(AppState->World, AbsTileX, AbsTileY, AbsTileZ);
    add_low_entity_result Result = AddLowEntity(AppState, EntityType_Wall, &P);

    Result.Low->Height = AppState->World->TileSideInMeters;
    Result.Low->Width = Result.Low->Height;
    Result.Low->Collides = true;

    return Result;
}

internal void
InitHitPoints(low_entity *EntityLow, u32 HitPointCount)
{
    Assert(HitPointCount <= ArrayCount(EntityLow->HitPoint));
    EntityLow->HitPointMax = HitPointCount;
    for(u32 HitPointIndex = 0;
        HitPointIndex < EntityLow->HitPointMax;
        ++HitPointIndex)
    {
        hit_point *HitPoint = EntityLow->HitPoint + HitPointIndex;
        HitPoint->Flags = 0;
        HitPoint->FilledAmount = HIT_POINT_SUB_COUNT;
    }
}

internal add_low_entity_result
AddSword(app_state *AppState)
{
    add_low_entity_result Result = AddLowEntity(AppState, EntityType_Sword, 0);

    Result.Low->Height = 0.5f;
    Result.Low->Width = 1.0f;
    Result.Low->Collides = false;

    return Result;
}

internal add_low_entity_result
AddPlayer(app_state *AppState)
{
    world_position P = AppState->CameraP;
    add_low_entity_result Result = AddLowEntity(AppState, EntityType_Hero, &P);

    Result.Low->Height = 0.5f;
    Result.Low->Width = 1.0f;
    Result.Low->Collides = true;

    InitHitPoints(Result.Low, 3);

    add_low_entity_result Sword = AddSword(AppState);
    Result.Low->SwordLowIndex = Sword.LowIndex;

    if(AppState->CameraFollowingEntityIndex == 0)
    {
        AppState->CameraFollowingEntityIndex = Result.LowIndex;
    }

    return Result;
}

internal add_low_entity_result
AddMonstar(app_state *AppState, u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ)
{
    world_position P = ChunkPositionFromTilePosition(AppState->World, AbsTileX, AbsTileY, AbsTileZ);
    add_low_entity_result Result = AddLowEntity(AppState, EntityType_Monstar, &P);

    Result.Low->Height = 0.5f;
    Result.Low->Width = 1.0f;
    Result.Low->Collides = true;

    InitHitPoints(Result.Low, 3);

    return Result;
}

internal add_low_entity_result
AddFamiliar(app_state *AppState, u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ)
{
    world_position P = ChunkPositionFromTilePosition(AppState->World, AbsTileX, AbsTileY, AbsTileZ);
    add_low_entity_result Result = AddLowEntity(AppState, EntityType_Familiar, &P);

    Result.Low->Height = 0.5f;
    Result.Low->Width = 1.0f;
    Result.Low->Collides = true;

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
MoveEntity(app_state *AppState, entity Entity, f32 dt, v2 ddP)
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

            // high_entity *HitHigh = AppState->HighEntities_ + HitHighEntityIndex;
            // low_entity *HitLow = AppState->LowEntities + HitHigh->LowEntityIndex;
            // TODO(kstandbridge): Stairs
            // Entity.High->AbsTileZ += HitLow->dAbsTileZ;
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

    world_position NewP = MapIntoChunkSpace(AppState->World, AppState->CameraP, Entity.High->P);

    // TODO(kstandbridge): Bundle these together as the position update?
    ChangeEntityLocation(&AppState->WorldArena, AppState->World, Entity.LowIndex,
                         Entity.Low, &Entity.Low->P, &NewP);
    Entity.Low->P = NewP;
}

global u32 TestHighEntityIndex = 0;
internal void
SetCamera(app_state *AppState, world_position NewCameraP)
{
    world *World = AppState->World;

    Assert(ValidateEntityPairs(AppState));

    world_difference dCameraP = Subtract(World, &NewCameraP, &AppState->CameraP);
    AppState->CameraP = NewCameraP;

    // TODO(kstandbridge): I am totally picking these numbers randomly!
    u32 TileSpanX = 17*3;
    u32 TileSpanY = 9*3;
    rectangle2 CameraBounds = Rectangle2CenterDim(V2Set1(0),
                                                  V2MultiplyScalar(V2(TileSpanX, TileSpanY), World->TileSideInMeters));
    v2 EntityOffsetForFrame = V2Invert(dCameraP.dXY);
    OffsetAndCheckFrequencyhByArena(AppState, EntityOffsetForFrame, CameraBounds);

    Assert(ValidateEntityPairs(AppState));

    // TODO(kstandbridge): Do this in terms of tile chunks!
    world_position MinChunkP = MapIntoChunkSpace(World, NewCameraP, CameraBounds.Min);
    world_position MaxChunkP = MapIntoChunkSpace(World, NewCameraP, CameraBounds.Max);
    for(s32 ChunkY = MinChunkP.ChunkY;
        ChunkY <= MaxChunkP.ChunkY;
        ++ChunkY)
    {
        for(s32 ChunkX = MinChunkP.ChunkX;
            ChunkX <= MaxChunkP.ChunkX;
            ++ChunkX)
        {
            world_chunk *Chunk = GetWorldChunk(World, ChunkX, ChunkY, NewCameraP.ChunkZ, 0);
            if(Chunk)
            {
                for(world_entity_block *Block = &Chunk->FirstBlock;
                    Block;
                    Block = Block->Next)
                {
                    for(u32 EntityIndexIndex = 0;
                        EntityIndexIndex < Block->EntityCount;
                        ++EntityIndexIndex)
                    {
                        u32 LowEntityIndex = Block->LowEntityIndex[EntityIndexIndex];
                        if(LowEntityIndex == TestHighEntityIndex)
                        {
                            // int Foo = 5;
                        }
                        low_entity *Low = AppState->LowEntities + LowEntityIndex;
                        if(Low->HighEntityIndex == 0)
                        {
                            v2 CameraSpaceP = GetCameraSpaceP(AppState, Low);
                            if(Rectangle2IsIn(CameraBounds, CameraSpaceP))
                            {
                                MakeEntityHighFrequency_(AppState, Low, LowEntityIndex, CameraSpaceP);
                            }
                        }
                    }
                }
            }
        }
    }

    Assert(ValidateEntityPairs(AppState));
}

internal void
PushPiece(entity_visible_piece_group *Group, loaded_bitmap *Bitmap,
          v2 Offset, f32 OffsetZ, v2 Align, v2 Dim, v4 Color, f32 EntityZC)
{
    Assert(Group->PieceCount < ArrayCount(Group->Pieces));
    entity_visible_piece *Piece = Group->Pieces + Group->PieceCount++;
    Piece->Bitmap = Bitmap;
    Piece->Offset = V2Subtract(V2MultiplyScalar(V2(Offset.X, -Offset.Y), Group->AppState->MetersToPixels), Align);
    Piece->OffsetZ = Group->AppState->MetersToPixels*OffsetZ;
    Piece->EntityZC = EntityZC;
    Piece->R = Color.R;
    Piece->G = Color.G;
    Piece->B = Color.B;
    Piece->A = Color.A;
    Piece->Dim = Dim;
}

internal void
PushBitmap(entity_visible_piece_group *Group, loaded_bitmap *Bitmap,
           v2 Offset, f32 OffsetZ, v2 Align, f32 Alpha, f32 EntityZC)
{
    PushPiece(Group, Bitmap, Offset, OffsetZ, Align, V2Set1(0.0f), V4(1.0f, 1.0f, 1.0f, Alpha), EntityZC);
}

internal void
PushRect(entity_visible_piece_group *Group, v2 Offset, f32 OffsetZ,
         v2 Dim, v4 Color, f32 EntityZC)
{
    PushPiece(Group, 0, Offset, OffsetZ, V2Set1(0.0f), Dim, Color, EntityZC);
}

internal entity
EntityFromHighIndex(app_state *AppState, u32 HighEntityIndex)
{
    entity Result = {0};

    if(HighEntityIndex)
    {
        Assert(HighEntityIndex < ArrayCount(AppState->HighEntities_));
        Result.High = AppState->HighEntities_ + HighEntityIndex;
        Result.LowIndex = Result.High->LowEntityIndex;
        Result.Low = AppState->LowEntities + Result.LowIndex;
    }

    return Result;
}

internal void
UpdateFamiliar(app_state *AppState, entity Entity, f32 dt)
{
    entity ClosestHero = {0};
    f32 ClosestHeroDSq = Square(10.0f); // NOTE(kstandbridge): Ten meter maximum search!
    for(u32 HighEntityIndex = 1;
        HighEntityIndex < AppState->HighEntityCount;
        ++HighEntityIndex)
    {
        entity TestEntity = EntityFromHighIndex(AppState, HighEntityIndex);

        if(TestEntity.Low->Type == EntityType_Hero)
        {
            f32 TestDSq = LengthSq(V2Subtract(TestEntity.High->P, Entity.High->P));
            if(TestEntity.Low->Type == EntityType_Hero)
            {
                TestDSq *= 0.75f;
            }

            if(ClosestHeroDSq > TestDSq)
            {
                ClosestHero = TestEntity;
                ClosestHeroDSq = TestDSq;
            }
        }
    }

    v2 ddP = {0};
    if(ClosestHero.High && (ClosestHeroDSq > Square(3.0f)))
    {
        // TODO(kstandbridge): PULL SPEED OUT OF MOVE ENTITY 
        f32 Acceleration = 0.5f;
        f32 OneOverLength = Acceleration / SquareRoot(ClosestHeroDSq);
        ddP = V2MultiplyScalar(V2Subtract(ClosestHero.High->P, Entity.High->P), OneOverLength);
    }

    MoveEntity(AppState, Entity, dt, ddP);
}

internal void
UpdateMonstar(app_state *AppState, entity Entity, f32 dt)
{
}

internal void
DrawHitpoints(low_entity *LowEntity, entity_visible_piece_group *PieceGroup)
{
    if(LowEntity->HitPointMax >= 1)
    {
        v2 HealthDim = V2Set1(0.2f);
        f32 SpacingX = 1.5f*HealthDim.X;
        v2 HitP = V2(-0.5f*(LowEntity->HitPointMax - 1)*SpacingX, -0.25f);
        v2 dHitP = V2(SpacingX, 0.0f);
        for(u32 HealthIndex = 0;
            HealthIndex < LowEntity->HitPointMax;
            ++HealthIndex)
        {
            hit_point *HitPoint = LowEntity->HitPoint + HealthIndex;
            v4 Color = V4(1.0f, 0.0f, 0.0f, 1.0f);
            if(HitPoint->FilledAmount == 0)
            {
                Color = V4(0.2f, 0.2f, 0.2f, 1.0f);
            }

            PushRect(PieceGroup, HitP, 0, HealthDim, Color, 0.0f);
            HitP = V2Add(HitP, dHitP);
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
        AddLowEntity(AppState, EntityType_Null, 0);
        AppState->HighEntityCount = 1;

        AppState->Backdrop = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_background.bmp")));
        AppState->Shadow = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_shadow.bmp")));
        AppState->Tree = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test2/tree00.bmp")));
        AppState->Sword = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test2/rock03.bmp")));

        hero_bitmaps *Bitmap = AppState->HeroBitmaps;

        Bitmap->Head = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_right_head.bmp")));
        Bitmap->Cape = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_right_cape.bmp")));
        Bitmap->Torso = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_right_torso.bmp")));
        Bitmap->Align = V2(72, 182);
        ++Bitmap;

        Bitmap->Head = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_back_head.bmp")));
        Bitmap->Cape = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_back_cape.bmp")));
        Bitmap->Torso = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_back_torso.bmp")));
        Bitmap->Align = V2(72, 182);
        ++Bitmap;

        Bitmap->Head = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_left_head.bmp")));
        Bitmap->Cape = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_left_cape.bmp")));
        Bitmap->Torso = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_left_torso.bmp")));
        Bitmap->Align = V2(72, 182);
        ++Bitmap;

        Bitmap->Head = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_front_head.bmp")));
        Bitmap->Cape = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_front_cape.bmp")));
        Bitmap->Torso = LoadBMP(PlatformReadEntireFile(&AppState->WorldArena, String("test/test_hero_front_torso.bmp")));
        Bitmap->Align = V2(72, 182);
        ++Bitmap;

        AppState->World = PushStruct(&AppState->WorldArena, world);
        world *World = AppState->World;
        InitializeWorld(World, 1.4f);

        s32 TileSideInPixels = 60;
        AppState->MetersToPixels = (f32)TileSideInPixels / (f32)World->TileSideInMeters;

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
            ScreenIndex < 2000;
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

        u32 CameraTileX = ScreenBaseX*TilesPerWidth + 17/2;
        u32 CameraTileY = ScreenBaseY*TilesPerHeight + 9/2;
        u32 CameraTileZ = ScreenBaseZ;
        world_position NewCameraP = ChunkPositionFromTilePosition(AppState->World,
                                                                  CameraTileX,
                                                                  CameraTileY,
                                                                  CameraTileZ);
        AddMonstar(AppState, CameraTileX + 2, CameraTileY + 2, CameraTileZ);
        for(s32 FamiliarIndex = 0;
            FamiliarIndex < 1;
            ++FamiliarIndex)
        {
            s32 FamiliarOffsetX = (RandomNumberTable[RandomNumberIndex++] % 10) - 7;
            s32 FamiliarOffsetY = (RandomNumberTable[RandomNumberIndex++] % 10) - 3;
            if((FamiliarOffsetX != 0) ||
               (FamiliarOffsetY != 0))
            {
                AddFamiliar(AppState, CameraTileX + FamiliarOffsetX, CameraTileY + FamiliarOffsetY, 
                            CameraTileZ);
            }
        }
        SetCamera(AppState, NewCameraP);
    }

    // world *World = AppState->World;

    f32 MetersToPixels = AppState->MetersToPixels;
 
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
                u32 EntityIndex = AddPlayer(AppState).LowIndex;
                AppState->PlayerIndexForController[ControllerIndex] = EntityIndex;
            }
        }
        else 
        {
            entity ControllingEntity = ForceEnmtityIntoHigh(AppState, LowIndex);
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

            if(Controller->Start.EndedDown)
            {
                ControllingEntity.High->dZ = 3.0f;
            }

            v2 dSword = {0};
            if(Controller->ActionUp.EndedDown)
            {
                dSword = V2(0.0f, 1.0f);
            }
            if(Controller->ActionDown.EndedDown)
            {
                dSword = V2(0.0f, -1.0f);
            }
            if(Controller->ActionLeft.EndedDown)
            {
                dSword = V2(-1.0f, 0.0f);
            }
            if(Controller->ActionRight.EndedDown)
            {
                dSword = V2(1.0f, 0.0f);;
            }

            MoveEntity(AppState, ControllingEntity, Input->dtForFrame, ddP);
            if((dSword.X != 0.0f) || (dSword.Y != 0.0f))
            {
                low_entity *Sword = GetLowEntity(AppState, ControllingEntity.Low->SwordLowIndex);
                if(Sword && !IsValid(Sword->P))
                {
                    world_position SwordP = ControllingEntity.Low->P;
                    ChangeEntityLocation(&AppState->WorldArena, AppState->World,
                                         ControllingEntity.Low->SwordLowIndex, Sword, 0, &SwordP);
                }
            }
        }
    }

    entity CameraFollowingEntity = ForceEnmtityIntoHigh(AppState, AppState->CameraFollowingEntityIndex);
    if(CameraFollowingEntity.High)
    {
        world_position NewCameraP = AppState->CameraP;

        NewCameraP.ChunkZ = CameraFollowingEntity.Low->P.ChunkZ;

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
#if 1 
    DrawRectangle(Buffer, V2Set1(0), V2(Buffer->Width, Buffer->Height), 0.5f, 0.5f, 0.5f);
#else
    DrawBitmap(Buffer, &AppState->Backdrop, 0, 0, 0, 0, 1);
#endif 

    f32 ScreenCenterX = 0.5f*(f32)Buffer->Width;
    f32 ScreenCenterY = 0.5f*(f32)Buffer->Height;

    entity_visible_piece_group PieceGroup =
    {
        .AppState = AppState,
    };
    for(u32 HighEntityIndex = 1;
        HighEntityIndex < AppState->HighEntityCount;
        ++HighEntityIndex)
    {
        PieceGroup.PieceCount = 0;

        high_entity *HighEntity = AppState->HighEntities_ + HighEntityIndex;
        low_entity *LowEntity = AppState->LowEntities + HighEntity->LowEntityIndex;

        entity Entity =
        {
            .LowIndex = HighEntity->LowEntityIndex,
            .Low = LowEntity,
            .High = HighEntity,
        };

        f32 dt = Input->dtForFrame;

        // TODO(kstandbridge): This is incorrect, should be computed after update!!!
        f32 ShadowAlpha = 1.0f - 0.5f*HighEntity->Z;
        if(ShadowAlpha < 0.0f)
        {
            ShadowAlpha = 0.0f;
        }

        hero_bitmaps *HeroBitmaps = &AppState->HeroBitmaps[HighEntity->FacingDirection];
        switch(LowEntity->Type)
        {
            case EntityType_Hero:
            {
                // TODO(kstandbridge): Z!!!
                PushBitmap(&PieceGroup, &AppState->Shadow, V2Set1(0.0f), 0, HeroBitmaps->Align, ShadowAlpha, 0.0f);
                PushBitmap(&PieceGroup, &HeroBitmaps->Torso, V2Set1(0.0f), 0, HeroBitmaps->Align, 1.0f, 1.0f);
                PushBitmap(&PieceGroup, &HeroBitmaps->Cape, V2Set1(0.0f), 0, HeroBitmaps->Align, 1.0f, 1.0f);
                PushBitmap(&PieceGroup, &HeroBitmaps->Head, V2Set1(0.0f), 0, HeroBitmaps->Align, 1.0f, 1.0f);

                DrawHitpoints(LowEntity, &PieceGroup);
            } break;

            case EntityType_Wall:
            {
                PushBitmap(&PieceGroup, &AppState->Tree, V2Set1(0.0f), 0, V2(40.0f, 80.0f), 1.0f, 1.0f);
            } break;

            case EntityType_Sword:
            {
                PushBitmap(&PieceGroup, &AppState->Shadow, V2Set1(0.0f), 0, HeroBitmaps->Align, ShadowAlpha, 0.0f);
                PushBitmap(&PieceGroup, &AppState->Sword, V2Set1(0.0f), 0, V2(29.0f, 10.0f), 1.0f, 1.0f);
            } break;

            case EntityType_Familiar:
            {
                UpdateFamiliar(AppState, Entity, dt);
                Entity.High->tBob += dt;
                if(Entity.High->tBob > (2.0f*Pi32))
                {
                    Entity.High->tBob -= (2.0f*Pi32);
                }
                f32 BobSin = Sin(2.0f*Entity.High->tBob);
                PushBitmap(&PieceGroup, &AppState->Shadow, V2Set1(0.0f), 0, HeroBitmaps->Align, ShadowAlpha, 1.0f);
                PushBitmap(&PieceGroup, &HeroBitmaps->Head, V2Set1(0.0f), 0.25f*BobSin, HeroBitmaps->Align, 1.0f, 1.0f);
            } break;

            case EntityType_Monstar:
            {
                UpdateMonstar(AppState, Entity, dt);
                PushBitmap(&PieceGroup, &AppState->Shadow, V2Set1(0.0f), 0, HeroBitmaps->Align, ShadowAlpha, 1.0f);
                PushBitmap(&PieceGroup, &HeroBitmaps->Torso, V2Set1(0.0f), 0,  HeroBitmaps->Align, 1.0f, 1.0f);

                DrawHitpoints(LowEntity, &PieceGroup);
            } break;

            default:
            {
                InvalidCodePath;
            } break;
        }

        f32 ddZ = -9.8f;
        HighEntity->Z = 0.5f*ddZ*Square(dt) + HighEntity->dZ*dt + HighEntity->Z;
        HighEntity->dZ = ddZ*dt + HighEntity->dZ;
        if(HighEntity->Z < 0)
        {
            HighEntity->Z = 0;
        }

        f32 EntityGroundPointX = ScreenCenterX + MetersToPixels*HighEntity->P.X;
        f32 EntityGroundPointY = ScreenCenterY - MetersToPixels*HighEntity->P.Y;
        f32 EntityZ = -MetersToPixels*HighEntity->Z;
#if 0
        v2 PlayerLeftTop = V2(PlayerGroundPointX - 0.5f*MetersToPixels*LowEntity->Width,
                              // PlayerGroundPointY - 0.5f*MetersToPixels*LowEntity->Height);
        v2 EntityWidthHeight = V2(LowEntity->Width, LowEntity->Height);

            DrawRectangle(Buffer,
                          PlayerLeftTop,
                          V2Add(PlayerLeftTop, V2MultiplyScalar(EntityWidthHeight, MetersToPixels)),
                          PlayerR, PlayerG, PlayerB);
#endif 
        for(u32 PieceIndex = 0;
            PieceIndex < PieceGroup.PieceCount;
            ++PieceIndex)
        {
            entity_visible_piece *Piece = PieceGroup.Pieces + PieceIndex;
            v2 Center = V2(EntityGroundPointX + Piece->Offset.X,
                           EntityGroundPointY + Piece->Offset.Y + Piece->OffsetZ + Piece->EntityZC*EntityZ);
            if(Piece->Bitmap)
            {
                DrawBitmap(Buffer, Piece->Bitmap, Center.X, Center.Y, Piece->A);
            }
            else 
            {
                v2 HalfDim = V2MultiplyScalar(Piece->Dim, 0.5f*MetersToPixels);
                DrawRectangle(Buffer, V2Subtract(Center, HalfDim), V2Add(Center, HalfDim), Piece->R, Piece->G, Piece->B);
            }
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
