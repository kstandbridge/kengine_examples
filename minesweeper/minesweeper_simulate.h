#ifndef MINESWEEPER_SIMULATE_H

typedef enum tile_flag
{
    TileFlag_Visited = (1 << 0),
    TileFlag_Mine =    (1 << 1),
    TileFlag_Flag =    (1 << 2),
    TileFlag_Unknown =  (1 << 3),
} tile_flag;

internal void
InitGame(app_state *AppState);

typedef struct simulate_game_work
{
    app_state *AppState;
    u8 Column;
    u8 Row;
} simulate_game_work;

internal void
SimulateGameThread(simulate_game_work *Work);

#define MINESWEEPER_SIMULATE_H
#endif //MINESWEEPER_SIMULATE_H
