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

// TODO(kstandbridge): Is this ever necessary?
typedef struct raw_position
{
    s32 TileMapX;
    s32 TileMapY;

    // NOTE(kstandbridge): Tile-map relative X and Y
    f32 X;
    f32 Y;
} raw_position;

typedef struct tile_map
{
    u32 *Tiles;
} tile_map;

typedef struct world
{
    s32 CountX;
    s32 CountY;

    f32 UpperLeftX;
    f32 UpperLeftY;
    f32 TileWidth;
    f32 TileHeight;

    // TODO(kstandbridge): Sparseness
    s32 TileMapCountX;
    s32 TileMapCountY;
    
    tile_map *TileMaps;
} world;

typedef struct app_state
{
    memory_arena Arena;

    // TODO(kstandbridge): Player state should be canonical position now?
    s32 PlayerTileMapX;
    s32 PlayerTileMapY;

    f32 PlayerX;
    f32 PlayerY;

} app_state;