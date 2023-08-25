typedef struct tile_chunk_position
{
    u32 TileChunkX;
    u32 TileChunkY;

    u32 RelTileX;
    u32 RelTileY;
} tile_chunk_position;

typedef struct world_position
{
    s32 AbsTileX;
    s32 AbsTileY;

    // NOTE(kstandbridge): Should these be from the center of a tile?
    // TODO(kstandbridge): Rename to offset X and Y
    f32 TileRelX;
    f32 TileRelY;
} world_position;

typedef struct tile_chunk
{
    u32 *Tiles;
} tile_chunk;

typedef struct world
{
    u32 ChunkShift;
    u32 ChunkMask;
    u32 ChunkDim;

    f32 TileSideInMeters;
    s32 TileSideInPixels;
    f32 MetersToPixels;
    
    // TODO(kstandbridge): Sparseness
    s32 TileChunkCountX;
    s32 TileChunkCountY;
    
    tile_chunk *TileChunks;
} world;

typedef struct app_state
{
    memory_arena Arena;

    world_position PlayerP;

} app_state;