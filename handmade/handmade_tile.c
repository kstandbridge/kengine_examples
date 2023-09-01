
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

    RecanonicalizeCoord(TileMap, &Result.AbsTileX, &Result.TileRelX);
    RecanonicalizeCoord(TileMap, &Result.AbsTileY, &Result.TileRelY);

    return Result;
}

internal tile_chunk *
GetTileChuck(tile_map *TileMap, u32 TileChuckX, u32 TileChuckY)
{
    tile_chunk *Result = 0;

    if((TileChuckX >= 0) && (TileChuckX < TileMap->TileChunkCountX) &&
       (TileChuckY >= 0) && (TileChuckY < TileMap->TileChunkCountY))
    {
        Result = &TileMap->TileChunks[TileChuckY*TileMap->TileChunkCountX + TileChuckX];
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
GetTileValue_(tile_map *TileMap, tile_chunk *TileChunk, u32 TestTileX, u32 TestTileY)
{
    u32 Result = 0;

    if(TileChunk)
    {
        Result = GetTileValueUnchecked(TileMap, TileChunk, TestTileX, TestTileY);
    }

    return Result;
}

internal void
SetTileValue_(tile_map *TileMap, tile_chunk *TileChunk, u32 TestTileX, u32 TestTileY,
             u32 TileValue)
{
    if(TileChunk)
    {
        SetTileValueUnchecked(TileMap, TileChunk, TestTileX, TestTileY, TileValue);
    }
}

inline tile_chunk_position
GetChunkPositionFor(tile_map *TileMap, u32 AbsTileX, u32 AbsTileY)
{
    tile_chunk_position Result =
    {
        .TileChunkX = AbsTileX >> TileMap->ChunkShift,
        .TileChunkY = AbsTileY >> TileMap->ChunkShift,
        .RelTileX = AbsTileX & TileMap->ChunkMask,
        .RelTileY = AbsTileY & TileMap->ChunkMask,
    };

    return Result;
}

internal u32
GetTileValue(tile_map *TileMap, u32 AbsTileX, u32 AbsTileY)
{
    tile_chunk_position ChunkPos = GetChunkPositionFor(TileMap, AbsTileX, AbsTileY);
    tile_chunk *TileChunk = GetTileChuck(TileMap, ChunkPos.TileChunkX, ChunkPos.TileChunkY);
    u32 Result = GetTileValue_(TileMap, TileChunk, ChunkPos.RelTileX, ChunkPos.RelTileY);

    return Result;
}
internal b32
IsTileMapPointEmpty(tile_map *TileMap, tile_map_position CanPos)
{

    u32 TileChunkValue = GetTileValue(TileMap, CanPos.AbsTileX, CanPos.AbsTileY);
    b32 Result = (TileChunkValue == 0);
    
    return Result;
}

internal void
SetTileValue(memory_arena *Arena, tile_map *TileMap, u32 AbsTileX, u32 AbsTileY, u32 TileValue)
{
    tile_chunk_position ChunkPos = GetChunkPositionFor(TileMap, AbsTileX, AbsTileY);
    tile_chunk *TileChunk = GetTileChuck(TileMap, ChunkPos.TileChunkX, ChunkPos.TileChunkY);

    // TODO(kstandbridge): On-demand tile chunk creation
    Assert(TileChunk);

    SetTileValue_(TileMap, TileChunk, ChunkPos.RelTileX, ChunkPos.RelTileY, TileValue);
}
