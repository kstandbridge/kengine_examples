typedef struct canonical_position
{
    s32 TileMapX;
    s32 TileMapY;

    s32 TileX;
    s32 TileY;

    // NOTE(kstandbridge): This is tile-relative X and Y
    // TODO(kstandbridge): These are still in pixels...
    f32 TileRelX;
    f32 TileRelY;
} canonical_position;

typedef struct tile_map
{
    u32 *Tiles;
} tile_map;

typedef struct world
{
    f32 TileSideInMeters;
    s32 TileSideInPixels;
    f32 MetersToPixels;
    
    s32 CountX;
    s32 CountY;

    f32 UpperLeftX;
    f32 UpperLeftY;

    // TODO(kstandbridge): Sparseness
    s32 TileMapCountX;
    s32 TileMapCountY;
    
    tile_map *TileMaps;
} world;

typedef struct app_state
{
    memory_arena Arena;

    canonical_position PlayerP;

} app_state;