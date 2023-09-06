// TODO(kstandbridge): Think about what the real safe margin is!
#define TILE_CHUNK_SAFE_MARGIN (S32Max/64)
#define TILE_CHUNK_UNINITIALIZED S32Max

internal world_chunk *
GetWorldChunk(world *World, s32 ChunkX, s32 ChunkY, s32 ChunkZ,
             memory_arena *Arena)
{
    Assert(ChunkX > -TILE_CHUNK_SAFE_MARGIN);
    Assert(ChunkY > -TILE_CHUNK_SAFE_MARGIN);
    Assert(ChunkZ > -TILE_CHUNK_SAFE_MARGIN);
    Assert(ChunkX < TILE_CHUNK_SAFE_MARGIN);
    Assert(ChunkY < TILE_CHUNK_SAFE_MARGIN);
    Assert(ChunkZ < TILE_CHUNK_SAFE_MARGIN);
    
    // TODO(kstandbridge): BETTER HASH FUNCTION!!!!
    u32 HashValue = 19*ChunkX + 7*ChunkY + 3*ChunkZ;
    u32 HashSlot = HashValue & (ArrayCount(World->ChunkHash) - 1);
    Assert(HashSlot < ArrayCount(World->ChunkHash));
    
    world_chunk *Chunk = World->ChunkHash + HashSlot;
    do
    {
        if((ChunkX == Chunk->ChunkX) &&
           (ChunkY == Chunk->ChunkY) &&
           (ChunkZ == Chunk->ChunkZ))
        {            
            break;
        }

        if(Arena && (Chunk->ChunkX != TILE_CHUNK_UNINITIALIZED) && (!Chunk->NextInHash))
        {
            Chunk->NextInHash = PushStruct(Arena, world_chunk);
            Chunk = Chunk->NextInHash;
            Chunk->ChunkX = TILE_CHUNK_UNINITIALIZED;
        }
        
        if(Arena && (Chunk->ChunkX == TILE_CHUNK_UNINITIALIZED))
        {
            // u32 TileCount = World->ChunkDim*World->ChunkDim;

            Chunk->ChunkX = ChunkX;
            Chunk->ChunkY = ChunkY;
            Chunk->ChunkZ = ChunkZ;

            Chunk->NextInHash = 0;

            break;
        }

        Chunk = Chunk->NextInHash;
    } while(Chunk);
    
    return(Chunk);
}

#if 0
internal world_chunk_position
GetChunkPositionFor(world *World, u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ)
{
    tile_chunk_position Result;

    Result.ChunkX = AbsTileX >> World->ChunkShift;
    Result.ChunkY = AbsTileY >> World->ChunkShift;
    Result.ChunkZ = AbsTileZ;
    Result.RelTileX = AbsTileX & World->ChunkMask;
    Result.RelTileY = AbsTileY & World->ChunkMask;

    return(Result);
}
#endif

internal void
InitializeWorld(world *World, f32 TileSideInMeters)
{
    World->ChunkShift = 4;
    World->ChunkMask = (1 << World->ChunkShift) - 1;
    World->ChunkDim = (1 << World->ChunkShift);        
    World->TileSideInMeters = TileSideInMeters;

    for(u32 ChunkIndex = 0;
        ChunkIndex < ArrayCount(World->ChunkHash);
        ++ChunkIndex)
    {
        World->ChunkHash[ChunkIndex].ChunkX = TILE_CHUNK_UNINITIALIZED;
    }
}

//
// TODO(kstandbridge): Do these really belong in more of a "positioning" or "geometry" file?
//

internal void
RecanonicalizeCoord(world *World, s32 *Tile, f32 *TileRel)
{
    // TODO(kstandbridge): Need to do something that doesn't use the divide/multiply method
    // for recanonicalizing because this can end up rounding back on to the tile
    // you just came from.

    // NOTE(kstandbridge): World is assumed to be toroidal topology, if you
    // step off one end you come back on the other!
    s32 Offset = RoundF32ToS32(*TileRel / World->TileSideInMeters);
    *Tile += Offset;
    *TileRel -= Offset*World->TileSideInMeters;

    // TODO(kstandbridge): Fix floating point math so this can be exact?
    Assert(*TileRel > -0.5f*World->TileSideInMeters);
    Assert(*TileRel < 0.5f*World->TileSideInMeters);
}

internal world_position
MapIntoTileSpace(world *World, world_position BasePos, v2 Offset)
{
    world_position Result = BasePos;

    Result.Offset_ = V2Add(Result.Offset_, Offset);
    RecanonicalizeCoord(World, &Result.AbsTileX, &Result.Offset_.X);
    RecanonicalizeCoord(World, &Result.AbsTileY, &Result.Offset_.Y);
    
    return(Result);
}

internal b32
AreOnSameTile(world_position *A, world_position *B)
{
    b32 Result = ((A->AbsTileX == B->AbsTileX) &&
                     (A->AbsTileY == B->AbsTileY) &&
                     (A->AbsTileZ == B->AbsTileZ));

    return(Result);
}

internal world_difference
Subtract(world *World, world_position *A, world_position *B)
{
    world_difference Result;

    v2 dTileXY = V2((f32)A->AbsTileX - (f32)B->AbsTileX,
                    (f32)A->AbsTileY - (f32)B->AbsTileY);
    f32 dTileZ = (f32)A->AbsTileZ - (f32)B->AbsTileZ;
    
    Result.dXY = V2Add(V2MultiplyScalar(dTileXY, World->TileSideInMeters),
                       V2Subtract(A->Offset_, B->Offset_));

    // TODO(kstandbridge): Think about what we want to do about Z
    Result.dZ = World->TileSideInMeters*dTileZ;

    return(Result);
}

internal world_position
CenteredTilePoint(u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ)
{
    world_position Result =
    {
        .AbsTileX = AbsTileX,
        .AbsTileY = AbsTileY,
        .AbsTileZ = AbsTileZ,
    };

    return(Result);
}
