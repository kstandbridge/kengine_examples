#pragma once

#include "handmade_world.h"

typedef struct hero_bitmaps
{
    s32 AlignX;
    s32 AlignY;

    loaded_bitmap Head;
    loaded_bitmap Cape;
    loaded_bitmap Torso;
} hero_bitmaps;

typedef struct high_entity
{
    v2 P; // NOTE(kstandbridge): Relative to the camera!
    
    v2 dP;
    u32 AbsTileZ;
    u32 FacingDirection;

    f32 Z;
    f32 dZ;
    u32 LowEntityIndex;
} high_entity;

typedef enum entity_type
{
    EntityType_Null,

    EntityType_Hero,
    EntityType_Wall,
} entity_type;

typedef struct low_entity
{
    entity_type Type;

    world_position P;
    f32 Width, Height;

    // NOTE(kstandbridge): This is for "stairs"
    b32 Collides;
    s32 dAbsTileZ;

    u32 HighEntityIndex;
} low_entity;

typedef struct entity
{
    u32 LowIndex;
    low_entity *Low;
    high_entity *High;
} entity;

typedef struct app_state
{
    memory_arena WorldArena;
    world *World;

    // TODO(kstandbridge): Should we allow split-screen?
    u32 CameraFollowingEntityIndex;
    world_position CameraP;

    u32 PlayerIndexForController[ArrayCount(((app_input *)0)->Controllers)];

    u32 LowEntityCount;
    low_entity LowEntities[100000];

    u32 HighEntityCount;
    high_entity HighEntities_[256];

    loaded_bitmap Backdrop;
    loaded_bitmap Shadow;
    hero_bitmaps HeroBitmaps[4];
} app_state;

