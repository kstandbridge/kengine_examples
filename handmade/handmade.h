#include "handmade_tile.h"

typedef struct world
{
    tile_map *TileMap;
} world;

typedef struct app_state
{
    memory_arena Arena;
    random_state RandomState;

    world *World;

    tile_map_position PlayerP;
    loaded_bitmap LoadedBitmap;

} app_state;
