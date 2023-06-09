
inline void
GenerateBoard(app_state *AppState, u8 FirstMoveColumn, u8 FirstMoveRow)
{
    AppState->MinesRemaining = 0;
    AppState->Tiles = PushArray(AppState->MemoryFlush.Arena, AppState->Rows * AppState->Columns, u8);
    
    while(AppState->Mines > AppState->MinesRemaining)
    {
        u32 Random = RandomU32(&AppState->RandomState) % (AppState->Rows * AppState->Columns);
        u32 Index = (FirstMoveRow * AppState->Columns) + FirstMoveColumn;
        // NOTE(kstandbridge): Dont place mine on players first move
        if(Index != Random)
        {
            u8 Tile = AppState->Tiles[Random];
            if((UnpackU8High(Tile) & TileFlag_Mine) == 0)
            {
                --AppState->RemainingTiles;
                ++AppState->MinesRemaining;
                AppState->Tiles[Random] = PackU8(TileFlag_Mine, 0);
                
                
#if KENGINE_INTERNAL
                if(AppState->DEBUGSlowSimulation)
                {
                    Sleep(50);
                }
#endif
                
            }
        }
    }
    
    for(u8 Row = 0;
        Row < AppState->Rows;
        ++Row)
    {
        for(u8 Column = 0;
            Column < AppState->Columns;
            ++Column)
        {
            u32 Index = (Row * AppState->Columns) + Column;
            u8 Tile = AppState->Tiles[Index];
            u8 Flags = UnpackU8High(Tile);
            if((Flags & TileFlag_Mine) == 0)
            {
                u8 Mines = 0;
                for(s8 Y = -1; Y < 2; ++Y)
                {
                    for(s8 X = -1; X < 2; ++X)
                    {
                        if((Column + X >= 0) &&
                           (Column + X < AppState->Columns) &&
                           (Row + Y >= 0) &&
                           (Row + Y < AppState->Rows))
                        {
                            u32 CheckIndex = ((Row + Y) * AppState->Columns) + Column + X;
                            u8 CheckTile = AppState->Tiles[CheckIndex];
                            u8 CheckFlags = UnpackU8High(CheckTile);
                            
                            if(CheckFlags & TileFlag_Mine)
                            {
                                ++Mines;
                            }
                        }
                    }
                }
                AppState->Tiles[Index] = PackU8(Flags, Mines);
#if KENGINE_INTERNAL
                if(AppState->DEBUGSlowSimulation)
                {
                    Sleep(50);
                }
#endif
            }
        }
    }
    
    AppState->IsInitialized = true;
    AppState->IsGameOver = false;
}

internal void
SimulateGameThread(memory_arena *TransientArena, simulate_game_work *Work)
{
    app_state *AppState = Work->AppState;
    u8 Column = Work->Column;
    u8 Row = Work->Row;
    
    if(!AppState->IsInitialized || AppState->IsGameOver)
    {
        GenerateBoard(AppState, Column, Row);
    }
    
    u32 Index = (Row * AppState->Columns) + Column;
    u8 Tile = AppState->Tiles[Index];
    u8 Flags = UnpackU8High(Tile);
    u8 Mines = UnpackU8Low(Tile);
    
    if((Flags & TileFlag_Visited) == 0)
    {
        Flags |= TileFlag_Visited;
        AppState->Tiles[Index] = PackU8(Flags, Mines);
        
#if KENGINE_INTERNAL
        if(AppState->DEBUGSlowSimulation)
        {
            Sleep(50);
        }
#endif
        
        if(Flags & TileFlag_Mine)
        {
            AppState->IsGameOver = true;
        }
        else
        {        
            --AppState->RemainingTiles;
            if(Mines == 0)
            {
                for(s8 Y = -1; Y < 2; ++Y)
                {
                    for(s8 X = -1; X < 2; ++X)
                    {
                        if((Column + X >= 0) &&
                           (Column + X < AppState->Columns) &&
                           (Row + Y >= 0) &&
                           (Row + Y < AppState->Rows))
                        {
                            u32 CheckIndex = ((Row + Y) * AppState->Columns) + Column + X;
                            u8 CheckTile = AppState->Tiles[CheckIndex];
                            u8 CheckFlags = UnpackU8High(CheckTile);
                            
                            if(((CheckFlags & TileFlag_Mine) == 0) &&
                               ((CheckFlags & TileFlag_Visited) == 0))
                            {
                                simulate_game_work NewWork = 
                                {
                                    .AppState = AppState,
                                    .Column = Column + X,
                                    .Row = Row + Y,
                                };
                                SimulateGameThread(TransientArena, &NewWork);
                            }
                        }
                    }
                }
            }
        }
    }
}

internal void
InitGame(app_state *AppState)
{
    switch(AppState->GameDifficulty)
    {
        case GameDifficulty_Beginner:
        {
            AppState->Columns = 8;
            AppState->Rows = 8;
            AppState->Mines = 10;
            
        } break;
        
        case GameDifficulty_Intermediate:
        {
            AppState->Columns = 16;
            AppState->Rows = 16;
            AppState->Mines = 40;
        } break;
        
        case GameDifficulty_Expert:
        {
            AppState->Columns = 30;
            AppState->Rows = 16;
            AppState->Mines = 99;
        } break;
    }
    
    f32 Width = (AppState->Columns * 16.0f * GlobalScale) + 56.0f;
    f32 Height = (AppState->Rows * 16.0f * GlobalScale) + 202.0f;
    PlatformSetWindowSize(V2(Width, Height));
    
    
    AppState->MinesRemaining = AppState->Mines;
    AppState->RemainingTiles = AppState->Rows * AppState->Columns;
    AppState->IsInitialized = false;
    AppState->IsGameOver = false;
    AppState->Timer = 0.0f;
    
    if(AppState->MemoryFlush.Arena)
    {
        EndTemporaryMemory(AppState->MemoryFlush);
    }
    CheckArena(&AppState->TransientArena);
    AppState->MemoryFlush = BeginTemporaryMemory(&AppState->TransientArena);
    AppState->Tiles = PushArray(AppState->MemoryFlush.Arena, AppState->RemainingTiles, u8);
}
