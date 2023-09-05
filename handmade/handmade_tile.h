#if !defined(HANDMADE_TILE_H)

typedef struct tile_map_difference
{
    v2 dXY;
    
    f32 dZ;
} tile_map_difference;

typedef struct tile_map_position
{
    // NOTE(kstandbridge): These are fixed point tile locations. the high
    // bits are the tile chunk index, and the low bits are the tile
    // index in the chunk.
    u32 AbsTileX;
    u32 AbsTileY;
    u32 AbsTileZ;

    // NOTE(kstandbridge): These are the offsets from the tile center
    
    v2 Offset;
} tile_map_position;

typedef struct tile_chunk_position
{
    u32 TileChunkX;
    u32 TileChunkY;
    u32 TileChunkZ;

    u32 RelTileX;
    u32 RelTileY;
} tile_chunk_position;

typedef struct tile_chunk
{
    // TODO(kstandbridge): Real structure for a tile!
    u32 *Tiles;
} tile_chunk;

typedef struct tile_map
{
    u32 ChunkShift;
    u32 ChunkMask;
    u32 ChunkDim;

    f32 TileSideInMeters;
    s32 TileSideInPixels;
    f32 MetersToPixels;
    
    // TODO(kstandbridge): REAL sparseness so anywhere in the world can be
    // represented without the giant pointer array.
    u32 TileChunkCountX;
    u32 TileChunkCountY;
    u32 TileChunkCountZ;
    
    tile_chunk *TileChunks;
} tile_map;

#define HANDMADE_TILE_H
#endif
