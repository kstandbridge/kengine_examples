#pragma once

#include "handmade_tile.h"

typedef struct world
{
    tile_map *TileMap;
} world;

typedef struct hero_bitmaps
{
    s32 AlignX;
    s32 AlignY;
    
    loaded_bitmap Head;
    loaded_bitmap Cape;
    loaded_bitmap Torso;
} hero_bitmaps;

typedef struct app_state
{
    memory_arena Arena;
    random_state RandomState;

    world *World;

    tile_map_position CameraP;
    tile_map_position PlayerP;

    loaded_bitmap Backdrop;
    u32 HeroFacingDirection;

    hero_bitmaps HeroBitmaps[4];
} app_state;

