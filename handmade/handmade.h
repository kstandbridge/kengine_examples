#pragma once

#include "handmade_world.h"

typedef struct hero_bitmaps
{
    v2 Align;

    loaded_bitmap Head;
    loaded_bitmap Cape;
    loaded_bitmap Torso;
} hero_bitmaps;

typedef struct high_entity
{
    v2 P; // NOTE(kstandbridge): Relative to the camera!
    v2 dP;
    u32 ChunkZ;
    u32 FacingDirection;

    f32 tBob;

    f32 Z;
    f32 dZ;
    u32 LowEntityIndex;
} high_entity;

typedef enum entity_type
{
    EntityType_Null,

    EntityType_Hero,
    EntityType_Wall,
    EntityType_Familiar,
    EntityType_Monstar,
    EntityType_Sword,
} entity_type;

#define HIT_POINT_SUB_COUNT 4
typedef struct hit_point 
{
    // TODO(kstandbridge): Bake this down into one variable
    u8 Flags;
    u8 FilledAmount;
} hit_point;

typedef struct low_entity
{
    entity_type Type;

    world_position P;
    f32 Width, Height;

    b32 Collides;
    s32 dAbsTileZ;

    u32 HighEntityIndex;

    // TODO(kstandbridge): Should hitpoints themselves be entities?
    u32 HitPointMax;
    hit_point HitPoint[16];

    u32 SwordLowIndex;
    f32 DistanceRemaining;
} low_entity;

typedef struct entity
{
    u32 LowIndex;
    low_entity *Low;
    high_entity *High;
} entity;

typedef struct entity_visible_piece
{
    loaded_bitmap *Bitmap;
    v2 Offset;
    f32 OffsetZ;
    f32 EntityZC;

    f32 R, G, B, A;

    v2 Dim;
} entity_visible_piece;

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

    loaded_bitmap Tree;
    loaded_bitmap Sword;
    f32 MetersToPixels;
} app_state;

// TODO(kstandbridge): This is dumb, this should just be part of 
// the renderer pushbuffer - add correction of coordinates
// in there and be done with it.
typedef struct entity_visible_piece_group
{
    app_state *AppState;
    u32 PieceCount;
    entity_visible_piece Pieces[32];
} entity_visible_piece_group;

