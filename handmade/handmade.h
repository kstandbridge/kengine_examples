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

typedef struct entity
{
    b32 Exists;
    tile_map_position P;
    v2 dP;
    u32 FacingDirection;
    f32 Width, Height;
} entity;

typedef struct app_state
{
    memory_arena Arena;

    world *World;

    // TODO(kstandbridge): Should we allow split-screen?
    u32 CameraFollowingEntityIndex;
    tile_map_position CameraP;

    u32 PlayerIndexForController[ArrayCount(((app_input *)0)->Controllers)];
    u32 EntityCount;
    entity Entities[256];

    loaded_bitmap Backdrop;
    hero_bitmaps HeroBitmaps[4];
} app_state;

