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

    // NOTE(kstandbridge): These are fixed point tile locations. The high
    // bits are the tile chunk index, and thw low bits are the tile
    // index in the chunk.
    s32 AbsTileX;
    s32 AbsTileY;
    s32 AbsTileZ;

    // NOTE(kstandbridge): These are the offsets from the tile center
    v2 Offset_;
} world_position;

// TODO(kstandbridge): Could make this just tile_chunk and then allow mutliple tile chunks per X/Y/AbsTileZ
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

    world_entity_block FirstBlock;

    struct world_chunk *NextInHash;
} world_chunk;

typedef struct world 
{
    f32 TileSideInMeters;

    // TODO(kstandbridge): WorldChunkHash should probably switch to pointers IF 
    // tile entity blocks continue to bew stored en masse directly in the tile chunk!
    // NOTE(kstandbridge): A the moment, this must be a power of two!
    s32 ChunkShift;
    s32 ChunkMask;
    s32 ChunkDim;
    world_chunk ChunkHash[4096];
} world;
