#pragma once

typedef struct world_difference
{
    v2 dXY;
    f32 dZ;
} world_difference;

typedef struct world_position
{
    // TODO(kstandbridge): Puzzler! How can we get rid of abstile* here,
    // and still allow references to entities to be able to figure
    // out _where they are_  (or rather, which world_chunk they are 
    // in?)

    s32 ChunkX;
    s32 ChunkY;
    s32 ChunkZ;

    // NOTE(kstandbridge): These are the offsets from the Chunk center
    v2 Offset_;
} world_position;

// TODO(kstandbridge): Could make this just tile_chunk and then allow mutliple tile chunks per X/Y/Z
typedef struct world_entity_block
{
    u32 EntityCount;
    u32 LowEntityIndex[16];
    struct world_entity_block *Next;
} world_entity_block;

typedef struct world_chunk
{
    s32 ChunkX;
    s32 ChunkY;
    s32 ChunkZ;

    // TODO(kstandbridge): Profile this and determine if a pointer would be better here!
    world_entity_block FirstBlock;

    struct world_chunk *NextInHash;
} world_chunk;

typedef struct world 
{
    f32 TileSideInMeters;
    f32 ChunkSideInMeters;

    world_entity_block *FirstFree;

    // TODO(kstandbridge): WorldChunkHash should probably switch to pointers IF 
    // tile entity blocks continue to bew stored en masse directly in the tile chunk!
    // NOTE(kstandbridge): A the moment, this must be a power of two!
    world_chunk ChunkHash[4096];
} world;
