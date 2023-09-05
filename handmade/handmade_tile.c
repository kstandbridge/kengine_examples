internal tile_chunk *
GetTileChuck(tile_map *TileMap, u32 TileChuckX, u32 TileChuckY, u32 TileChunkZ)
{
    tile_chunk *Result = 0;

    if((TileChuckX >= 0) && (TileChuckX < TileMap->TileChunkCountX) &&
       (TileChuckY >= 0) && (TileChuckY < TileMap->TileChunkCountY) &&
       (TileChunkZ >= 0) && (TileChunkZ < TileMap->TileChunkCountZ))
    {
        Result = &TileMap->TileChunks[
            TileChunkZ*TileMap->TileChunkCountY*TileMap->TileChunkCountX +
            TileChuckY*TileMap->TileChunkCountX + 
            TileChuckX];
    }

    return Result;
}

internal u32
GetTileValueUnchecked(tile_map *TileMap, tile_chunk *TileChunk, u32 TileX, u32 TileY)
{
    Assert(TileChunk);
    Assert(TileX < TileMap->ChunkDim);
    Assert(TileY < TileMap->ChunkDim);

    u32 Result = TileChunk->Tiles[TileY*TileMap->ChunkDim + TileX];
    return Result; 
}

internal void
SetTileValueUnchecked(tile_map *TileMap, tile_chunk *TileChunk, u32 TileX, u32 TileY,
                      u32 TileValue)
{
    Assert(TileChunk);
    Assert(TileX < TileMap->ChunkDim);
    Assert(TileY < TileMap->ChunkDim);

    TileChunk->Tiles[TileY*TileMap->ChunkDim + TileX] = TileValue;
}

internal u32
GetTileValue__(tile_map *TileMap, tile_chunk *TileChunk, u32 TestTileX, u32 TestTileY)
{
    u32 Result = 0;

    if(TileChunk && TileChunk->Tiles)
    {
        Result = GetTileValueUnchecked(TileMap, TileChunk, TestTileX, TestTileY);
    }

    return Result;
}

internal void
SetTileValue_(tile_map *TileMap, tile_chunk *TileChunk, 
              u32 TestTileX, u32 TestTileY, u32 TileValue)
{
    if(TileChunk && TileChunk->Tiles)
    {
        SetTileValueUnchecked(TileMap, TileChunk, TestTileX, TestTileY, TileValue);
    }
}

internal tile_chunk_position
GetChunkPositionFor(tile_map *TileMap, u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ)
{
    tile_chunk_position Result =
    {
        .TileChunkX = AbsTileX >> TileMap->ChunkShift,
        .TileChunkY = AbsTileY >> TileMap->ChunkShift,
        .TileChunkZ = AbsTileZ,
        .RelTileX = AbsTileX & TileMap->ChunkMask,
        .RelTileY = AbsTileY & TileMap->ChunkMask,
    };

    return Result;
}

internal u32
GetTileValue_(tile_map *TileMap, u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ)
{
    tile_chunk_position ChunkPos = GetChunkPositionFor(TileMap, AbsTileX, AbsTileY, AbsTileZ);
    tile_chunk *TileChunk = GetTileChuck(TileMap, ChunkPos.TileChunkX, ChunkPos.TileChunkY, ChunkPos.TileChunkZ);
    u32 Result = GetTileValue__(TileMap, TileChunk, ChunkPos.RelTileX, ChunkPos.RelTileY);

    return Result;
}

internal u32
GetTileValue(tile_map *TileMap, tile_map_position Pos)
{
    u32 Result = GetTileValue_(TileMap, Pos.AbsTileX, Pos.AbsTileY, Pos.AbsTileZ);
    
    return Result;
}

internal b32
IsTileMapPointEmpty(tile_map *TileMap, tile_map_position Pos)
{

    u32 TileChunkValue = GetTileValue(TileMap, Pos);
    b32 Result = ((TileChunkValue == 1) ||
                  (TileChunkValue == 3) ||
                  (TileChunkValue == 4));
    
    return Result;
}

internal void
SetTileValue(memory_arena *Arena, tile_map *TileMap, 
             u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ,
             u32 TileValue)
{
    tile_chunk_position ChunkPos = GetChunkPositionFor(TileMap, AbsTileX, AbsTileY, AbsTileZ);
    tile_chunk *TileChunk = GetTileChuck(TileMap, ChunkPos.TileChunkX, ChunkPos.TileChunkY, ChunkPos.TileChunkZ);

    Assert(TileChunk);
    if(!TileChunk->Tiles)
    {
        u32 TileCount = TileMap->ChunkDim*TileMap->ChunkDim;
        TileChunk->Tiles = PushArray(Arena, TileCount, u32);
        for(u32 TileIndex = 0;
            TileIndex < TileCount;
            ++TileIndex)
        {
            TileChunk->Tiles[TileIndex] = 1;
        }
    }

    SetTileValue_(TileMap, TileChunk, ChunkPos.RelTileX, ChunkPos.RelTileY, TileValue);
}

// 
// TODO(kstandbridge): Do these really belong in more of a "positioning" or "geometry" file?
//

internal void
RecanonicalizeCoord(tile_map *TileMap, u32 *Tile, f32 *TileRel)
{
    // TODO(kstandbridge): Need to do something that doesn't use the divide/multiple method
    // for recanonicallizing because this can end up rounding back on to the tile
    // you just came from.

    // NOTE(kstandbridge): World is assumed to be toroidal topology, if you
    // step off one end you come back on the other!
    s32 Offset = RoundF32ToS32(*TileRel / TileMap->TileSideInMeters);
    *Tile += Offset;
    *TileRel -= Offset*TileMap->TileSideInMeters;

    // TODO(kstandbridge): Fix floating point math so this can be <
    Assert(*TileRel >= -0.5f*TileMap->TileSideInMeters);
    Assert(*TileRel <= 0.5f*TileMap->TileSideInMeters);
}

internal tile_map_position
RecanonicalizePosition(tile_map *TileMap, tile_map_position Pos)
{
    tile_map_position Result = Pos;

    RecanonicalizeCoord(TileMap, &Result.AbsTileX, &Result.Offset.X);
    RecanonicalizeCoord(TileMap, &Result.AbsTileY, &Result.Offset.Y);

    return Result;
}

internal b32
AreOnSameTile(tile_map_position *A, tile_map_position *B)
{
    b32 Result = ((A->AbsTileX == B->AbsTileX) &&
                  (A->AbsTileY == B->AbsTileY) &&
                  (A->AbsTileZ == B->AbsTileZ));

    return Result;
}

internal tile_map_difference
Subtract(tile_map *TileMap, tile_map_position *A, tile_map_position *B)
{
    tile_map_difference Result;

    v2 dTileXY = V2((f32)A->AbsTileX - (f32)B->AbsTileX,
                    (f32)A->AbsTileY - (f32)B->AbsTileY);
    f32 dTileZ = (f32)A->AbsTileZ - (f32)B->AbsTileZ;

    Result.dXY = V2Add(V2MultiplyScalar(dTileXY, TileMap->TileSideInMeters),
                       V2Subtract(A->Offset, B->Offset));
    // Result.dXY = TileMap->TileSideInMeters*dTileXY + (A->Offset - B->Offset);

    // TODO(kstandbridge): Think about what we want to do about Z 
    Result.dZ = TileMap->TileSideInMeters*dTileZ;

    return Result;
}
