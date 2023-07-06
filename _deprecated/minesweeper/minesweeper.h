#ifndef MINESWEEPER_H

typedef enum game_difficulty_type
{
    GameDifficulty_Unknown,
    
    GameDifficulty_Beginner,
    GameDifficulty_Intermediate,
    GameDifficulty_Expert,
} game_difficulty_type;

typedef struct app_state
{
    memory_arena Arena;
    
    ui_state *UIState;
    app_assets *Assets;
    
    random_state RandomState;
    sprite_sheet Sprite;
    ticket_mutex AssetLock;
    
    f32 Timer;
    
    platform_work_queue *FrameQueue;
    platform_work_queue *BackgroundQueue;
    
    memory_arena TransientArena;
    temporary_memory MemoryFlush;
    
    b32 IsInitialized;
    b32 IsGameOver;
    game_difficulty_type GameDifficulty;
    u8 Mines;
    u8 MinesRemaining;
    u8 Columns;
    u8 Rows;
    u8 *Tiles;
    u32 RemainingTiles;
    
#if KENGINE_INTERNAL
    b32 DEBUGShowMines;
    b32 DEBUGShowMineCounts;
    b32 DEBUGSlowSimulation;
#endif
    
} app_state;



#define MINESWEEPER_H
#endif //MINESWEEPER_H
