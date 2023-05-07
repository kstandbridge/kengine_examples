#ifndef MINESWEEPER_H

typedef struct app_state
{
    memory_arena Arena;
    
    ui_state *UIState;
    random_state RandomState;
    sprite_sheet Sprite;
    
    f32 Timer;
    
    platform_work_queue *WorkQueue;
    
    memory_arena TransientArena;
    temporary_memory MemoryFlush;
    b32 IsInitialized;
    b32 IsGameOver;
    u8 Mines;
    u8 MinesRemaining;
    u8 Columns;
    u8 Rows;
    u8 *Tiles;
    u8 RemainingTiles;
    
#if KENGINE_INTERNAL
    b32 DEBUGShowMines;
#endif
    
} app_state;


#define MINESWEEPER_H
#endif //MINESWEEPER_H
