// TODO(kstandbridge): Think about what the real safe margin is!
#define TILE_CHUNK_SAFE_MARGIN (S32Max/64)
#define TILE_CHUNK_UNINITIALIZED S32Max

#define TILES_PER_CHUNK 16

internal b32
IsCanonical_(world *World, f32 TileRel)
{
    // TODO(kstandbridge): Fix floating point math so this can be exact?
    b32 Result = ((TileRel >= -0.5f*World->ChunkSideInMeters) &&
                  (TileRel <= 0.5f*World->ChunkSideInMeters));

    return Result;
}

internal b32
IsCanonical(world *World, v2 Offset)
{
    b32 Result = (IsCanonical_(World, Offset.X) &&
                  IsCanonical_(World, Offset.Y));

    return Result;
}

internal b32
AreInSameChunk(world *World, world_position *A, world_position *B)
{
    Assert(IsCanonical(World, A->Offset_));
    Assert(IsCanonical(World, B->Offset_));

    b32 Result = ((A->ChunkX == B->ChunkX) &&
                  (A->ChunkY == B->ChunkY) &&
                  (A->ChunkZ == B->ChunkZ));

    return Result;
}

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

internal void
InitializeWorld(world *World, f32 TileSideInMeters)
{
    World->TileSideInMeters = TileSideInMeters;
    World->ChunkSideInMeters = (f32)TILES_PER_CHUNK*TileSideInMeters;
    World->FirstFree = 0;

    for(u32 ChunkIndex = 0;
        ChunkIndex < ArrayCount(World->ChunkHash);
        ++ChunkIndex)
    {
        World->ChunkHash[ChunkIndex].ChunkX = TILE_CHUNK_UNINITIALIZED;
        World->ChunkHash[ChunkIndex].FirstBlock.EntityCount = 0;
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

    // NOTE(kstandbridge): Wrapping IS NOT ALLOWED, so all coordinates are assumed to be 
    // within the safe margin!
    // TODO(kstandbridge): Assert that we are nowhere near the edges of the world.

    s32 Offset = RoundF32ToS32(*TileRel / World->ChunkSideInMeters);
    *Tile += Offset;
    *TileRel -= Offset*World->ChunkSideInMeters;

    // TODO(kstandbridge): Fix floating point math so this can be exact?
    Assert(IsCanonical_(World, *TileRel));
}

internal world_position
MapIntoChunkSpace(world *World, world_position BasePos, v2 Offset)
{
    world_position Result = BasePos;

    Result.Offset_ = V2Add(Result.Offset_, Offset);
    RecanonicalizeCoord(World, &Result.ChunkX, &Result.Offset_.X);
    RecanonicalizeCoord(World, &Result.ChunkY, &Result.Offset_.Y);
    
    return(Result);
}

internal world_position
ChunkPositionFromTilePosition(world *World, s32 AbsTileX, s32 AbsTileY, s32 AbstileZ)
{
    world_position Result =
    {
        .ChunkX = AbsTileX / TILES_PER_CHUNK,
        .ChunkY = AbsTileY / TILES_PER_CHUNK,
        .ChunkZ = AbstileZ / TILES_PER_CHUNK,
    };

    // TODO(kstandbridge): Think this through on the real stream and actually work out the math.
    if(AbsTileX < 0)
    {
        --Result.ChunkX;
    }
    if(AbsTileY < 0)
    {
        --Result.ChunkY;
    }
    if(AbstileZ < 0)
    {
        --Result.ChunkZ;
    }

    // TODO(kstandbridge): DECIDE ON TILE ALIGNMENT IN CHUNKS!
    Result.Offset_.X = (f32)((AbsTileX - TILES_PER_CHUNK/2) - (Result.ChunkX*TILES_PER_CHUNK)) * World->TileSideInMeters;
    Result.Offset_.Y = (f32)((AbsTileY - TILES_PER_CHUNK/2) - (Result.ChunkY*TILES_PER_CHUNK)) * World->TileSideInMeters;
    // TODO(kstandbridge): Move to 3D Z!!!
    
    Assert(IsCanonical(World, Result.Offset_));
    
    return Result;
}

internal world_difference
Subtract(world *World, world_position *A, world_position *B)
{
    world_difference Result;

    v2 dTileXY = V2((f32)A->ChunkX - (f32)B->ChunkX,
                    (f32)A->ChunkY - (f32)B->ChunkY);
    f32 dTileZ = (f32)A->ChunkZ - (f32)B->ChunkZ;
    
    Result.dXY = V2Add(V2MultiplyScalar(dTileXY, World->ChunkSideInMeters),
                       V2Subtract(A->Offset_, B->Offset_));

    // TODO(kstandbridge): Think about what we want to do about Z
    Result.dZ = World->ChunkSideInMeters*dTileZ;

    return(Result);
}

internal world_position
CenteredChunkPoint(u32 ChunkX, u32 ChunkY, u32 ChunkZ)
{
    world_position Result =
    {
        .ChunkX = ChunkX,
        .ChunkY = ChunkY,
        .ChunkZ = ChunkZ,
    };

    return(Result);
}

internal void
ChangeEntityLocation(memory_arena *Arena, world *World, u32 LowEntityIndex,
                     world_position *OldP, world_position *NewP)
{
    if(OldP && AreInSameChunk(World, OldP, NewP))
    {
        // NOTE(kstandbridge): Leave entity where it is
    }
    else 
    {
        if(OldP)
        {
            // NOTE(kstandbridge): Pull the entity out of its old entity block 
            world_chunk *Chunk = GetWorldChunk(World, OldP->ChunkX, OldP->ChunkY, OldP->ChunkZ, 0);
            Assert(Chunk);
            if(Chunk)
            {
                b32 NotFound = true;
                world_entity_block *FirstBlock = &Chunk->FirstBlock;
                for(world_entity_block *Block = FirstBlock;
                    Block && NotFound;
                    Block = Block->Next)
                {
                    for(u32 Index = 0;
                        (Index < Block->EntityCount) && NotFound;
                        ++Index)
                    {
                        if(Block->LowEntityIndex[Index] == LowEntityIndex)
                        {
                            Assert(FirstBlock->EntityCount > 0);
                            Block->LowEntityIndex[Index] =
                                FirstBlock->LowEntityIndex[--FirstBlock->EntityCount];
                            if(FirstBlock->EntityCount == 0)
                            {
                                if(FirstBlock->Next)
                                {
                                    world_entity_block *NextBlock = FirstBlock->Next;
                                    *FirstBlock = *NextBlock;

                                    NextBlock->Next = World->FirstFree;
                                    World->FirstFree = NextBlock;
                                }
                            }

                            NotFound = false;
                        }
                    }
                }
            }
        }

        // NOTE(kstandbridge): Insert the entity into its new entity block 
        world_chunk *Chunk = GetWorldChunk(World, NewP->ChunkX, NewP->ChunkY, NewP->ChunkZ, Arena);
        Assert(Chunk);

        world_entity_block *Block = &Chunk->FirstBlock;
        if(Block->EntityCount == ArrayCount(Block->LowEntityIndex))
        {
            // NOTE(kstandbridge): We're out of room, get a new block!
            world_entity_block *OldBlock = World->FirstFree;
            if(OldBlock)
            {
                World->FirstFree = OldBlock->Next;
            }
            else 
            {
                OldBlock = PushStruct(Arena, world_entity_block);
            }

            *OldBlock = *Block;
            Block->Next = OldBlock;
            Block->EntityCount = 0;
        }

        Assert(Block->EntityCount < ArrayCount(Block->LowEntityIndex));
        Block->LowEntityIndex[Block->EntityCount++] = LowEntityIndex;
    }
}
