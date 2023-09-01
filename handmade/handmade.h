#include "handmade_tile.h"

typedef struct world
{
    tile_map *TileMap;
} world;

typedef struct app_state
{
    memory_arena Arena;
    world *World;

    tile_map_position PlayerP;

} app_state;
