#ifndef MINESWEEPER_H

typedef struct sprite_sheet
{
    s32 Width;
    s32 Height;
    s32 Comp;
    void *Handle;
    
} sprite_sheet;

typedef enum tile_flag
{
    TileFlag_Visited = (1 << 0),
    TileFlag_Mine =    (1 << 1),
    TileFlag_Flag =    (1 << 2),
    TileFlag_Unkown =  (1 << 3),
} tile_flag;

typedef struct app_state
{
    memory_arena Arena;
    
    ui_state UIState;
    random_state RandomState;
    sprite_sheet Sprite;
    
    f32 Timer;
    
    b32 IsLoading;
    b32 IsGameOver;
    platform_work_queue *WorkQueue;
    
    memory_arena TransientArena;
    temporary_memory MemoryFlush;
    u8 Mines;
    u8 MinesRemaining;
    u8 Columns;
    u8 Rows;
    u8 *Tiles;
} app_state;

#define RGBv4(R, G ,B) V4((f32)R/255.0f,(f32)G/255.0f,(f32)B/255.0f, 1.0f)
global f32 GlobalScale = 2.0f;


#define MINESWEEPER_H
#endif //MINESWEEPER_H
