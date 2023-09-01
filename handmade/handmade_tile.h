typedef struct tile_map_position
{
    // NOTE(kstandbridge): These are fixed point tile locations. the high
    // bits are the tile chunk index, and the low bits are the tile
    // index in the chunk.
    u32 AbsTileX;
    u32 AbsTileY;

    // NOTE(kstandbridge): Should these be from the center of a tile?
    // TODO(kstandbridge): Rename to offset X and Y
    f32 TileRelX;
    f32 TileRelY;
} tile_map_position;

typedef struct tile_chunk_position
{
    u32 TileChunkX;
    u32 TileChunkY;

    u32 RelTileX;
    u32 RelTileY;
} tile_chunk_position;

typedef struct tile_chunk
{
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
    
    // TODO(kstandbridge): Sparseness
    u32 TileChunkCountX;
    u32 TileChunkCountY;
    
    tile_chunk *TileChunks;
} tile_map;
