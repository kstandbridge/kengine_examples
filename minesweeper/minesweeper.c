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
    
    AppState->WorkQueue = PlatformMakeWorkQueue(&AppState->Arena, 16);
    
    // TODO(kstandbridge): Move spritesheet loading to thread?
    {    
        sprite_sheet *Sprite = &AppState->Sprite;
        stbi_uc *Bytes = stbi_load("sprite.png", &Sprite->Width, &Sprite->Height, &Sprite->Comp, 4);
        Sprite->Handle = DirectXLoadTexture(Sprite->Width, Sprite->Height, (u32 *)Bytes);
        free(Bytes);
    }
    
    InitUI(&AppState->UIState);
    
    InitGame(AppState);
    
    PlatformSetWindowSize(V2(322, 464));
    
    // TODO(kstandbridge): Move loading a glyph sprite sheet to thread in engine?
    {
        ui_state *UIState = AppState->UIState;
        LogDebug("Loading a glyph sprite sheet");
        
        string FontData = PlatformReadEntireFile(&AppState->Arena, String("C:\\Windows\\Fonts\\segoeui.ttf"));
        stbtt_InitFont(&UIState->FontInfo, FontData.Data, 0);
        
        f32 MaxFontHeightInPixels = 12.0f;
        UIState->FontScale = stbtt_ScaleForPixelHeight(&UIState->FontInfo, MaxFontHeightInPixels);
        stbtt_GetFontVMetrics(&UIState->FontInfo, &UIState->FontAscent, &UIState->FontDescent, &UIState->FontLineGap);
        
#if 1
        u32 FirstChar = 0;
        u32 LastChar = 255;
#else
        u32 FirstChar = 0x0400;
        u32 LastChar = FirstChar + 255;
#endif
        s32 MaxWidth = 0;
        s32 MaxHeight = 0;
        s32 TotalWidth = 0;
        s32 TotalHeight = 0;
        u32 ColumnAt = 0;
        u32 RowCount = 1;
        
        glyph_info *GlyphInfo = UIState->GlyphInfos;
        
        for(u32 CodePoint = FirstChar;
            CodePoint < LastChar;
            ++CodePoint)
        {                
            s32 Padding = (s32)(MaxFontHeightInPixels / 3.0f);
            u8 OnEdgeValue = (u8)(0.8f*255.0f);
            f32 PixelDistanceScale = (f32)OnEdgeValue/(f32)Padding;
            GlyphInfo->Data = stbtt_GetCodepointSDF(&UIState->FontInfo, UIState->FontScale, CodePoint, Padding, OnEdgeValue, PixelDistanceScale, 
                                                    &GlyphInfo->Width, &GlyphInfo->Height, 
                                                    &GlyphInfo->XOffset, &GlyphInfo->YOffset);
            
            stbtt_GetCodepointHMetrics(&UIState->FontInfo, CodePoint, &GlyphInfo->AdvanceWidth, &GlyphInfo->LeftSideBearing);
            
            GlyphInfo->CodePoint = CodePoint;
            
            if(GlyphInfo->Data)
            {
                TotalWidth += GlyphInfo->Width;
                ++ColumnAt;
                
                if(GlyphInfo->Height > MaxHeight)
                {
                    MaxHeight = GlyphInfo->Height;
                }
            }
            
            if((ColumnAt % 16) == 0)
            {
                ++RowCount;
                ColumnAt = 0;
                if(TotalWidth > MaxWidth)
                {
                    MaxWidth = TotalWidth;
                }
                TotalWidth = 0;
            }
            
            ++GlyphInfo;
        }
        
        TotalWidth = MaxWidth;
        TotalHeight = MaxHeight*RowCount;
        
        umm TextureSize = TotalWidth*TotalHeight*sizeof(u32);
        // TODO(kstandbridge): Temp memory here
        u32 *TextureBytes = PushSize(&AppState->Arena, TextureSize);
        
        u32 AtX = 0;
        u32 AtY = 0;
        
        ColumnAt = 0;
        
        for(u32 Index = 0;
            Index < ArrayCount(UIState->GlyphInfos);
            ++Index)
        {
            GlyphInfo = UIState->GlyphInfos + Index;
            
            GlyphInfo->UV = V4((f32)AtX / (f32)TotalWidth, (f32)AtY / (f32)TotalHeight,
                               ((f32)AtX + (f32)GlyphInfo->Width) / (f32)TotalWidth, 
                               ((f32)AtY + (f32)GlyphInfo->Height) / (f32)TotalHeight);
            
            for(s32 Y = 0;
                Y < GlyphInfo->Height;
                ++Y)
            {
                for(s32 X = 0;
                    X < GlyphInfo->Width;
                    ++X)
                {
                    u32 Alpha = (u32)GlyphInfo->Data[(Y*GlyphInfo->Width) + X];
                    TextureBytes[(Y + AtY)*TotalWidth + (X + AtX)] = 0x00FFFFFF | (u32)((Alpha) << 24);
                }
            }
            
            AtX += GlyphInfo->Width;
            
            ++ColumnAt;
            
            if((ColumnAt % 16) == 0)
            {
                AtY += MaxHeight;
                AtX = 0;
            }
            
            stbtt_FreeSDF(GlyphInfo->Data, 0);
        }
        
        UIState->SpriteSheetSize = V2(TotalWidth, TotalHeight);
        UIState->GlyphSheetHandle = DirectXLoadTexture(TotalWidth, TotalHeight, TextureBytes);
    }
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
    {    
        v2 P = V2(0, 0);
        v2 Size = UIState->SpriteSheetSize;
        PushRenderCommandGlyph(RenderGroup, P, 3.0f, Size, V4(1, 1, 1, 1), V4(0, 0, 1, 1), UIState->GlyphSheetHandle);
    }
    
    {    
        v2 P = V2(10 + UIState->SpriteSheetSize.X, 0);
        v2 Size = UIState->SpriteSheetSize;
        PushRenderCommandSprite(RenderGroup, P, 3.0f, Size, V4(1, 1, 1, 1), V4(0, 0, 1, 1), UIState->GlyphSheetHandle);
    }
    
    string LoremIpsum = String("Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.");
    rectangle2 Bounds = Rectangle2(Input->MouseP, V2(RenderGroup->Width, RenderGroup->Height));
    DrawTextAt(UIState, Bounds, 4.0f, GlobalScale, V4(0.3f, 0, 0.3f, 1), LoremIpsum);
#else
    v2 WorkingArea = V2(316, 436);
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
            
            if(BeginMenu(UIState, GridGetCellBounds(UIState, 0, 0, 0.0f), GlobalScale, String("Game"), 5, 120))
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
            BeginGrid(UIState, ScoreBounds, 3, 1);
            {
                GridSetColumnWidth(UIState, 1, (26.0f + 8.0f)*GlobalScale);
                
                rectangle2 MineCountBounds = GridGetCellBounds(UIState, 0, 0, 16.0f);
                DrawNumber(RenderGroup, &AppState->Sprite, MineCountBounds, (AppState->RemainingTiles == 0) ? 0 : AppState->MinesRemaining);
                
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
                                    PlatformAddWorkEntry(AppState->WorkQueue, SimulateGameThread, NewWork);
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
                                    DrawButton(RenderGroup, &AppState->Sprite, TileBounds.Min, 1);
                                }
                                else
                                {
                                    DrawButton(RenderGroup, &AppState->Sprite, TileBounds.Min, 4);
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
#if KENGINE_INTERNAL
                        else if((Flags & TileFlag_Mine) &&
                                (AppState->DEBUGShowMines))
                        {
                            DrawButton(RenderGroup, &AppState->Sprite, TileBounds.Min, 0);
                        }
                        else if(AppState->DEBUGShowMineCounts)
                        {
                            DrawButtonNumber(RenderGroup, &AppState->Sprite, TileBounds.Min, Mines % 8);
                        }
#endif
                        else if(Flags & TileFlag_Visited)
                        {
                            DrawButtonNumber(RenderGroup, &AppState->Sprite, TileBounds.Min, Mines % 8);
                        }
                        else if(Flags & TileFlag_Flag)
                        {
                            DrawButton(RenderGroup, &AppState->Sprite, TileBounds.Min, 4);
                        }
                        else if(Flags & TileFlag_Unknown)
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
#endif
    EndUI(UIState);
    CheckArena(&AppState->Arena);
    
#if KENGINE_INTERNAL
    if(!AppState->DEBUGSlowSimulation)
#endif
    {    
        Win32CompleteAllWork(AppState->WorkQueue);
    }
    
}

#include "minesweeper_rendering.c"
#include "minesweeper_simulate.c"
