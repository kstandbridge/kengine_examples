#define KENGINE_WIN32
#define KENGINE_DIRECTX
#include "kengine.h"

#include "minesweeper_rendering.h"

#include "minesweeper.h"
#include "minesweeper_simulate.h"

global f32 GlobalScale = 2.0f;

extern void
InitApp(app_memory *AppMemory)
{
    Assert(AppMemory->AppState == 0);
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    Assert(AppState);
    AppState->RandomState.Value = (u32)PlatformGetSystemTimestamp();
    
    sprite_sheet *Sprite = &AppState->Sprite;
    stbi_uc *Bytes = stbi_load("sprite.png", &Sprite->Width, &Sprite->Height, &Sprite->Comp, 4);
    Sprite->Handle = DirectXLoadTexture(Sprite->Width, Sprite->Height, (u32 *)Bytes);
    free(Bytes);
    
    AppState->WorkQueue = PlatformMakeWorkQueue(&AppState->Arena, 4);
    
    InitUI(&AppState->UIState);
    
    InitGame(AppState);
    
    PlatformSetWindowSize(V2(322, 464));
}

extern void
AppUpdateFrame(app_memory *AppMemory, render_group *RenderGroup, app_input *Input, f32 DeltaTime)
{
    app_state *AppState = AppMemory->AppState;
    if(!AppState->IsGameOver && AppState->RemainingTiles > 0)
    {
        AppState->Timer += DeltaTime;
    }
    RenderGroup->ClearColor = RGBv4(192, 192, 192);
    Assert(AppState);
    
    ui_state *UIState = AppState->UIState;
    BeginUI(UIState, Input);
    
    v2 WorkingArea = V2(316, 436);
    v2 OffSet = V2((RenderGroup->Width * 0.5f) - (WorkingArea.X * 0.5f),
                   (RenderGroup->Height * 0.5f) - (WorkingArea.Y * 0.5f));
    rectangle2 Bounds = Rectangle2(OffSet, V2Add(OffSet, WorkingArea));
    BeginGrid(UIState, Bounds, 1, 2);
    {
        GridSetRowHeight(UIState, 0, 40.0f);
        rectangle2 MenuBounds = GridGetCellBounds(UIState, 0, 0, 0);
        BeginGrid(UIState, MenuBounds, 1, 3);
        {
            
        }
        EndGrid(UIState);
        
        rectangle2 GameBounds = GridGetCellBounds(UIState, 0, 1, 4.0f);
        PushRenderCommandAlternateRectOutline(RenderGroup, GameBounds, 1.0f, 6.0f, 
                                              RGBv4(255, 255, 255), RGBv4(128, 128, 128));
        BeginGrid(UIState, GameBounds, 1, 2);
        {
            GridSetRowHeight(UIState, 0, 104.0f);
            rectangle2 ScoreBounds = GridGetCellBounds(UIState, 0, 0, 16.0f);
            PushRenderCommandAlternateRectOutline(RenderGroup, ScoreBounds, 1.0f, 4.0f, 
                                                  RGBv4(128, 128, 128), RGBv4(255, 255, 255));
            BeginGrid(UIState, ScoreBounds, 3, 1);
            {
                GridSetColumnWidth(UIState, 1, (26.0f + 8.0f)*GlobalScale);
                
                rectangle2 MineCountBounds = GridGetCellBounds(UIState, 0, 0, 16.0f);
                PushRenderCommandAlternateRectOutline(RenderGroup, MineCountBounds, 1.0f, 1.0f,
                                                      RGBv4(128, 128, 128), RGBv4(255, 255, 255));
                DrawNumber(RenderGroup, &AppState->Sprite, MineCountBounds, AppState->MinesRemaining);
                
                rectangle2 FaceButtonBounds = GridGetCellBounds(UIState, 1, 0, 16.0f);
                
                ui_interaction Interaction =
                {
                    .Id = GenerateUIId(0),
                    .Type = UI_Interaction_ImmediateButton,
                    .Target = 0
                };
                if(InteractionsAreEqual(Interaction, UIState->ToExecute))
                {
                    InitGame(AppState);
                }
                ui_interaction_state InteractionState = AddUIInteraction(UIState, FaceButtonBounds, Interaction);
                if(InteractionState == UIInteractionState_HotClicked)
                {
                    DrawFace(RenderGroup, &AppState->Sprite, FaceButtonBounds.Min, 0);
                }
                else if(AppState->RemainingTiles == 0)
                {
                    DrawFace(RenderGroup, &AppState->Sprite, FaceButtonBounds.Min, 4);
                }
                else if(AppState->IsGameOver)
                {
                    DrawFace(RenderGroup, &AppState->Sprite, FaceButtonBounds.Min, 3);
                }
                else if(UIState->Interaction.Target == AppState->Tiles)
                {
                    DrawFace(RenderGroup, &AppState->Sprite, FaceButtonBounds.Min, 2);
                }
                else
                {
                    DrawFace(RenderGroup, &AppState->Sprite, FaceButtonBounds.Min, 1);
                }
                
                rectangle2 TimerBounds = GridGetCellBounds(UIState, 2, 0, 16.0f);
                PushRenderCommandAlternateRectOutline(RenderGroup, TimerBounds, 1.0f, 1.0f,
                                                      RGBv4(128, 128, 128), RGBv4(255, 255, 255));
                DrawNumber(RenderGroup, &AppState->Sprite, TimerBounds, (u32)AppState->Timer);
            }
            EndGrid(UIState);
            
            rectangle2 BoardBounds = GridGetCellBounds(UIState, 0, 1, 16.0f);
            PushRenderCommandAlternateRectOutline(RenderGroup, BoardBounds, 1.0f, 4.0f, 
                                                  RGBv4(128, 128, 128), RGBv4(255, 255, 255));
            BoardBounds = Rectangle2AddRadiusTo(BoardBounds, -4.0f);
            
            BeginGrid(UIState, BoardBounds, AppState->Columns, AppState->Rows);
            {
                for(u8 Row = 0;
                    Row < AppState->Rows;
                    ++Row)
                {
                    for(u8 Column = 0;
                        Column < AppState->Columns;
                        ++Column)
                    {
                        rectangle2 TileBounds = GridGetCellBounds(UIState, Column, Row, 0.0f);
                        
                        u32 Index = (Row * AppState->Columns) + Column;
                        u8 Tile = AppState->Tiles[Index];
                        u8 Flags = UnpackU8High(Tile);
                        u8 Mines = UnpackU8Low(Tile);
                        
                        if((!AppState->IsGameOver) && 
                           (AppState->RemainingTiles > 0))
                        {                        
                            ui_interaction Interaction =
                            {
                                .Id = GenerateUIId(AppState->Tiles + Index),
                                .Type = UI_Interaction_ImmediateButton,
                                .Target = AppState->Tiles
                            };
                            
                            if(InteractionsAreEqual(Interaction, UIState->ToExecute))
                            {
                                simulate_game_work *NewWork = PushStruct(AppState->MemoryFlush.Arena, simulate_game_work);
                                NewWork->AppState = AppState;
                                NewWork->Column = Column;
                                NewWork->Row = Row;
                                PlatformAddWorkEntry(AppState->WorkQueue, SimulateGameThread, NewWork);
                            }
                            
                            ui_interaction_state InteractionState = AddUIInteraction(UIState, TileBounds, Interaction);
                            if(InteractionState == UIInteractionState_Hot)
                            {
                                if(WasPressed(Input->MouseButtons[MouseButton_Right]))
                                {
                                    if(Flags & TileFlag_Flag)
                                    {
                                        Flags &= ~TileFlag_Flag;
                                        Flags |= TileFlag_Unkown;
                                        ++AppState->MinesRemaining;
                                    }
                                    else if(Flags & TileFlag_Unkown)
                                    {
                                        Flags &= ~TileFlag_Unkown;
                                    }
                                    else
                                    {
                                        Flags |= TileFlag_Flag;
                                        --AppState->MinesRemaining;
                                    }
                                    AppState->Tiles[Index] = PackU8(Flags, Mines);
                                }
                            }
                        }
                        
                        if(AppState->IsGameOver)
                        {
                            if(Flags & TileFlag_Flag)
                            {
                                if((Flags & TileFlag_Mine) == 0)
                                {
                                    DrawButton(RenderGroup, &AppState->Sprite, TileBounds.Min, 1);
                                }
                                else
                                {
                                    DrawButton(RenderGroup, &AppState->Sprite, TileBounds.Min, 0);
                                }
                            } 
                            else if(Flags & TileFlag_Mine)
                            {
                                if(Flags & TileFlag_Visited)
                                {
                                    DrawButton(RenderGroup, &AppState->Sprite, TileBounds.Min, 2);
                                }
                                else
                                {
                                    DrawButton(RenderGroup, &AppState->Sprite, TileBounds.Min, 0);
                                }
                            }
                            else
                            {
                                DrawButtonNumber(RenderGroup, &AppState->Sprite, TileBounds.Min, Mines % 8);
                            }
                        }
                        else if(Flags & TileFlag_Flag)
                        {
                            DrawButton(RenderGroup, &AppState->Sprite, TileBounds.Min, 4);
                        }
                        else if(Flags & TileFlag_Visited)
                        {
                            DrawButtonNumber(RenderGroup, &AppState->Sprite, TileBounds.Min, Mines % 8);
                        }
                        else if(Flags & TileFlag_Unkown)
                        {
                            DrawButton(RenderGroup, &AppState->Sprite, TileBounds.Min, 3);
                        }
                        else
                        {
                            DrawButton(RenderGroup, &AppState->Sprite, TileBounds.Min, 5);
                        }
                    }
                }
            }
            EndGrid(UIState);
        }
        EndGrid(UIState);
        
    }
    EndGrid(UIState);
    
    EndUI(UIState, Input);
    CheckArena(&AppState->Arena);
}


#include "minesweeper_rendering.c"
#include "minesweeper_simulate.c"
