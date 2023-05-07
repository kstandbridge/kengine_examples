#ifndef MINESWEEPER_SIMULATE_H

typedef enum tile_flag
{
    TileFlag_Visited = (1 << 0),
    TileFlag_Mine =    (1 << 1),
    TileFlag_Flag =    (1 << 2),
    TileFlag_Unkown =  (1 << 3),
} tile_flag;

internal void
InitGame(struct app_state *AppState);


typedef struct simulate_game_work
{
    struct app_state *AppState;
    u8 Column;
    u8 Row;
} simulate_game_work;

internal void
SimulateGameThread(void *Data);

#define MINESWEEPER_SIMULATE_H
#endif //MINESWEEPER_SIMULATE_H
