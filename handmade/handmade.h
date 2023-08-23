typedef struct tile_map
{
    s32 CountX;
    s32 CountY;

    f32 UpperLeftX;
    f32 UpperLeftY;
    f32 TileWidth;
    f32 TileHeight;

    u32 *Tiles;

} tile_map;

typedef struct world
{
    // TODO(kstandbridge): Sparseness

    s32 TileMapCountX;
    s32 TileMapCountY;
    
    tile_map *TileMaps;
} world;

typedef struct app_state
{
    memory_arena Arena;

    f32 PlayerX;
    f32 PlayerY;

} app_state;