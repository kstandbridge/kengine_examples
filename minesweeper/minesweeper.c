#define KENGINE_IMPLEMENTATION
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
    
    AppState->FrameQueue = PlatformMakeWorkQueue(&AppState->Arena, 6);
    AppState->BackgroundQueue = PlatformMakeWorkQueue(&AppState->Arena, 2);
    
    AppState->GameDifficulty = GameDifficulty_Expert;
    
    InitAssets(&AppState->Assets, AppState->BackgroundQueue);
    InitUI(&AppState->UIState, AppState->Assets);
    
    InitGame(AppState);
}

extern void
AppUpdateFrame(app_memory *AppMemory, render_group *RenderGroup, app_input *Input, f32 DeltaTime)
{
    app_state *AppState = AppMemory->AppState;
    RenderGroup->ClearColor = RGBv4(192, 192, 192);
    if((!AppState->IsGameOver) && 
       (AppState->IsInitialized) &&
       (AppState->RemainingTiles > 0))
    {
        AppState->Timer += DeltaTime;
    }
    Assert(AppState);
    ui_state *UIState = AppState->UIState;
    
    
    
    BeginUI(UIState, Input, RenderGroup);
#if 0
    
    BeginTicketMutex(&AppState->Assets->AssetLock);
    
    v2 P = V2(0, 0);
    f32 Scale = 2.0f;
    P;
    Scale;
#if 0   
    DrawFace(AppState, RenderGroup, P, 0);
    P.X += (26.0f + 50) * Scale;
    
    {
        sprite_sheet *Sprite = &AppState->Sprite;
        if(Sprite->AssetState == AssetState_Loaded)
        {
            v2 Size = V2(Sprite->Width, Sprite->Height);
            PushRenderCommandSprite(RenderGroup, P, 3.0f, V2Multiply(Size, V2Set1(Scale)), V4(1, 1, 1, 1), V4(0, 0, 1, 1), Sprite->Handle);
            P.X += (Size.X + 10) * Scale;
        }
    }
#endif
    
#if 0   
    for(glyph_sprite_sheet *Sprite = AppState->Assets->GlyphSpriteSheets;
        Sprite;
        Sprite = Sprite->Next)
    {
        if(Sprite->AssetState == AssetState_Loaded)
        {
            
            PushRenderCommandGlyph(RenderGroup, P, 3.0f, V2Multiply(Sprite->Size, V2Set1(Scale)), V4(1, 1, 1, 1), V4(0, 0, 1, 1), Sprite->Handle);
            P.X += (Sprite->Size.X + 10) * Scale;
        }
    }
#endif
    
    EndTicketMutex(&AppState->Assets->AssetLock);
    
#if 0
    string LoremIpsum = String("\\u0400 \\u2714 Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer \\u2715 took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.");
#else
    string LoremIpsum = String("before \\u2714 after");
#endif
    
    rectangle2 Bounds = Rectangle2(UIState->MouseP, V2(RenderGroup->Width, RenderGroup->Height));
    
    DrawTextAt(UIState, Bounds, 4.0f, 2.0f, V4(0.3f, 0, 0.3f, 1), LoremIpsum);
#else
    
    v2 WorkingArea = V2(RenderGroup->Width, RenderGroup->Height);
    v2 OffSet = V2((RenderGroup->Width * 0.5f) - (WorkingArea.X * 0.5f),
                   (RenderGroup->Height * 0.5f) - (WorkingArea.Y * 0.5f));
    rectangle2 Bounds = Rectangle2(OffSet, V2Add(OffSet, WorkingArea));
    
    BeginGrid(UIState, Bounds, 1, 2);
    {
        GridSetRowHeight(UIState, 0, 28.0f);
        rectangle2 MenuBounds = GridGetCellBounds(UIState, 0, 0, 1.0f);
        BeginGrid(UIState, MenuBounds, 4, 1);
        {
            GridSetColumnWidth(UIState, 0, 66.0f);
            GridSetColumnWidth(UIState, 1, 54.0f);
            GridSetColumnWidth(UIState, 2, 72.0f);
            
            if(BeginMenu(UIState, GridGetCellBounds(UIState, 0, 0, 0.0f), GlobalScale, String("Game"), 5, 240))
            {
                if(MenuButton(UIState, 0, GlobalScale, String("New")))
                {
                    InitGame(AppState);
                }
                if(MenuButton(UIState, 1, GlobalScale, String("Beginner")))
                {
                    LogDebug("Set beginner level");
                }
                if(MenuButton(UIState, 2, GlobalScale, String("Intermediate")))
                {
                    LogDebug("Set Intermediate level");
                }
                if(MenuButton(UIState, 3, GlobalScale, String("Expert")))
                {
                    LogDebug("Set Expert level");
                }
                if(MenuButton(UIState, 4, GlobalScale, String("Custom...")))
                {
                    LogDebug("Set Custom level");
                }
                EndMenu(UIState);
            }
            
            if(BeginMenu(UIState, GridGetCellBounds(UIState, 1, 0, 0.0f), GlobalScale, String("Help"), 2, 160))
            {
                MenuButton(UIState, 0, GlobalScale, String("How to play?"));
                MenuButton(UIState, 1, GlobalScale, String("About"));
                EndMenu(UIState);
            }
            
#if KENGINE_INTERNAL
            if(BeginMenu(UIState, GridGetCellBounds(UIState, 2, 0, 0.0f), GlobalScale, String("Debug"), 4, 240))
            {
                MenuCheck(UIState, 0, GlobalScale, String("Show Mines"), &AppState->DEBUGShowMines);
                MenuCheck(UIState, 1, GlobalScale, String("Show Mine Counts"), &AppState->DEBUGShowMineCounts);
                MenuCheck(UIState, 2, GlobalScale, String("Slow Simulation"), &AppState->DEBUGSlowSimulation);
                if(MenuButton(UIState, 3, GlobalScale, String("Reset Timer")))
                {
                    AppState->Timer = 0.0f;
                }
                EndMenu(UIState);
            }
#endif
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
            BeginGrid(UIState, ScoreBounds, 5, 1);
            {
                GridSetColumnWidth(UIState, 1, (39.0f + 9.0f)*GlobalScale);
                GridSetColumnWidth(UIState, 2, (26.0f + 8.0f)*GlobalScale);
                GridSetColumnWidth(UIState, 3, (39.0f + 9.0f)*GlobalScale);
                
                
                rectangle2 MineCountBounds = GridGetCellBounds(UIState, 1, 0, 16.0f);
                DrawNumber(AppState, RenderGroup, MineCountBounds, (AppState->RemainingTiles == 0) ? 0 : AppState->MinesRemaining);
                
                rectangle2 FaceButtonBounds = GridGetCellBounds(UIState, 2, 0, 16.0f);
                
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
                    DrawFace(AppState, RenderGroup, FaceButtonBounds.Min, 0);
                }
                else if(AppState->RemainingTiles == 0)
                {
                    DrawFace(AppState, RenderGroup, FaceButtonBounds.Min, 4);
                }
                else if(AppState->IsGameOver)
                {
                    DrawFace(AppState, RenderGroup, FaceButtonBounds.Min, 3);
                }
                else if(UIState->Interaction.Target == AppState->Tiles)
                {
                    DrawFace(AppState, RenderGroup, FaceButtonBounds.Min, 2);
                }
                else
                {
                    DrawFace(AppState, RenderGroup, FaceButtonBounds.Min, 1);
                }
                
                rectangle2 TimerBounds = GridGetCellBounds(UIState, 3, 0, 16.0f);
                DrawNumber(AppState, RenderGroup, TimerBounds, (u32)AppState->Timer);
            }
            EndGrid(UIState);
            
            rectangle2 BoardBounds = GridGetCellBounds(UIState, 0, 1, 16.0f);
            PushRenderCommandAlternateRectOutline(RenderGroup, BoardBounds, 1.0f, 4.0f, 
                                                  RGBv4(128, 128, 128), RGBv4(255, 255, 255));
            BoardBounds = Rectangle2AddRadiusTo(BoardBounds, -4.0f);
            
            BeginGrid(UIState, BoardBounds, AppState->Columns + 2, AppState->Rows + 2);
            {
                for(u8 Row = 0;
                    Row < AppState->Rows;
                    ++Row)
                {
                    GridSetRowHeight(UIState, Row + 1, 16.0f*GlobalScale);
                }
                
                for(u8 Column = 0;
                    Column < AppState->Columns;
                    ++Column)
                {
                    GridSetColumnWidth(UIState, Column + 1, 16.0f*GlobalScale);
                }
                
                for(u8 Row = 0;
                    Row < AppState->Rows;
                    ++Row)
                {
                    for(u8 Column = 0;
                        Column < AppState->Columns;
                        ++Column)
                    {
                        rectangle2 TileBounds = GridGetCellBounds(UIState, Column + 1, Row + 1, 0.0f);
                        
                        u32 Index = (Row * AppState->Columns) + Column;
                        u8 Tile = AppState->Tiles[Index];
                        u8 Flags = UnpackU8High(Tile);
                        u8 Mines = UnpackU8Low(Tile);
                        
                        if((!AppState->IsGameOver) && 
                           (AppState->RemainingTiles > 0) &&
                           ((Flags & TileFlag_Visited) == 0))
                        {                        
                            ui_interaction Interaction =
                            {
                                .Id = GenerateUIId(AppState->Tiles + Index),
                                .Type = UI_Interaction_ImmediateButton,
                                .Target = AppState->Tiles
                            };
                            
                            if(((Flags & TileFlag_Flag) == 0) &&
                               ((Flags & TileFlag_Unknown) == 0))
                            {                                
                                if(InteractionsAreEqual(Interaction, UIState->ToExecute))
                                {
                                    simulate_game_work *NewWork = PushStruct(AppState->MemoryFlush.Arena, simulate_game_work);
                                    NewWork->AppState = AppState;
                                    NewWork->Column = Column;
                                    NewWork->Row = Row;
                                    PlatformAddWorkEntry(AppState->BackgroundQueue, SimulateGameThread, NewWork);
                                }
                            }
                            
                            ui_interaction_state InteractionState = AddUIInteraction(UIState, TileBounds, Interaction);
                            if(InteractionState == UIInteractionState_Hot)
                            {
                                if(WasPressed(Input->MouseButtons[MouseButton_Right]))
                                {
                                    if(Flags & TileFlag_Flag)
                                    {
                                        Flags &= ~TileFlag_Flag;
                                        Flags |= TileFlag_Unknown;
                                        ++AppState->MinesRemaining;
                                    }
                                    else if(Flags & TileFlag_Unknown)
                                    {
                                        Flags &= ~TileFlag_Unknown;
                                    }
                                    else
                                    {
                                        if(AppState->MinesRemaining > 0)
                                        {
                                            Flags |= TileFlag_Flag;
                                            --AppState->MinesRemaining;
                                        }
                                    }
                                    AppState->Tiles[Index] = PackU8(Flags, Mines);
                                }
                            }
                        }
                        
                        if((AppState->IsGameOver) || 
                           (AppState->RemainingTiles == 0))
                        {
                            if(Flags & TileFlag_Flag)
                            {
                                if((Flags & TileFlag_Mine) == 0)
                                {
                                    DrawButton(AppState, RenderGroup, TileBounds.Min, 1);
                                }
                                else
                                {
                                    DrawButton(AppState, RenderGroup, TileBounds.Min, 4);
                                }
                            } 
                            else if(Flags & TileFlag_Mine)
                            {
                                if(Flags & TileFlag_Visited)
                                {
                                    DrawButton(AppState, RenderGroup, TileBounds.Min, 2);
                                }
                                else
                                {
                                    DrawButton(AppState, RenderGroup, TileBounds.Min, 0);
                                }
                            }
                            else
                            {
                                DrawButtonNumber(AppState, RenderGroup, TileBounds.Min, Mines % 8);
                            }
                        }
#if KENGINE_INTERNAL
                        else if((Flags & TileFlag_Mine) &&
                                (AppState->DEBUGShowMines))
                        {
                            DrawButton(AppState, RenderGroup, TileBounds.Min, 0);
                        }
                        else if(AppState->DEBUGShowMineCounts)
                        {
                            DrawButtonNumber(AppState, RenderGroup, TileBounds.Min, Mines % 8);
                        }
#endif
                        else if(Flags & TileFlag_Visited)
                        {
                            DrawButtonNumber(AppState, RenderGroup, TileBounds.Min, Mines % 8);
                        }
                        else if(Flags & TileFlag_Flag)
                        {
                            DrawButton(AppState, RenderGroup, TileBounds.Min, 4);
                        }
                        else if(Flags & TileFlag_Unknown)
                        {
                            DrawButton(AppState, RenderGroup, TileBounds.Min, 3);
                        }
                        else
                        {
                            DrawButton(AppState, RenderGroup, TileBounds.Min, 5);
                        }
                    }
                }
            }
            EndGrid(UIState);
        }
        EndGrid(UIState);
        
    }
    EndGrid(UIState);
#endif
    
    EndUI(UIState);
    CheckArena(&AppState->Arena);
}

#include "minesweeper_rendering.c"
#include "minesweeper_simulate.c"
