#define KENGINE_WIN32
#define KENGINE_DIRECTX
#include "kengine.h"

#include "minesweeper.h"

inline void
DrawSprite(render_group *RenderGroup, sprite_sheet *Sprite, f32 OffsetY, u32 Index, v2 Size, v2 P)
{
    v4 UV = V4((Index*Size.X), 
               OffsetY, 
               ((Index+1)*Size.X), 
               (Size.Y + OffsetY));
    UV.R = UV.R / Sprite->Width;
    UV.G = UV.G / Sprite->Height;
    UV.B = UV.B / Sprite->Width;
    UV.A = UV.A / Sprite->Height;
    
    Size = V2Multiply(Size, V2Set1(GlobalScale));
    
    PushRenderCommandSprite(RenderGroup, P, 3.0f, Size, V4(1, 1, 1, 1), UV, Sprite->Handle);
}

inline void
DrawNumber_(render_group *RenderGroup, sprite_sheet *Sprite, v2 P, u32 Index)
{
    f32 OffsetY = 32.0f;
    v2 Size = V2(13.0f, 23.0f);
    DrawSprite(RenderGroup, Sprite, OffsetY, Index, Size, P);
}

inline void
DrawNumber(render_group *RenderGroup, sprite_sheet *Sprite, rectangle2 Bounds, u32 Number)
{
    PushRenderCommandAlternateRectOutline(RenderGroup, Bounds, 1.0f, 1.0f,
                                          RGBv4(128, 128, 128), RGBv4(255, 255, 255));
    v2 P = Bounds.Min; P.X += 1; P.Y += 1;
    DrawNumber_(RenderGroup, Sprite, P, (Number/100) % 10);
    P.X += 13.0f*GlobalScale;
    DrawNumber_(RenderGroup, Sprite, P, (Number/10) % 10);
    P.X += 13.0f*GlobalScale;
    DrawNumber_(RenderGroup, Sprite, P, Number % 10);
}

inline void
DrawFace(render_group *RenderGroup, sprite_sheet *Sprite, v2 P, u32 Index)
{
    f32 OffsetY = 55.0f;
    v2 Size = V2(26.0f, 26.0f);
    DrawSprite(RenderGroup, Sprite, OffsetY, Index, Size, P);
}

inline void
DrawButton(render_group *RenderGroup, sprite_sheet *Sprite, v2 P, u32 Index)
{
    f32 OffsetY = 16.0f;
    v2 Size = V2(16.0f, 16.0f);
    DrawSprite(RenderGroup, Sprite, OffsetY, Index, Size, P);
}

inline void
DrawButtonNumber(render_group *RenderGroup, sprite_sheet *Sprite, v2 P, u32 Index)
{
    f32 OffsetY = 0.0f;
    v2 Size = V2(16.0f, 16.0f);
    DrawSprite(RenderGroup, Sprite, OffsetY, Index, Size, P);
}

inline b32
IsMine(app_state *AppState, u8 Column, u8 Row)
{
    b32 Result = false;
    
    u32 Index = (Row * AppState->Columns) + Column;
    u8 Tile = AppState->Tiles[Index];
    u8 Flags = UnpackU8High(Tile);
    
    if(Flags & TileFlag_Mine)
    {
        Result = true;
    }
    
    return Result;
}

typedef struct fan_out_work
{
    app_state *AppState;
    u8 Column;
    u8 Row;
} fan_out_work;

internal void
FanoutThread(void *Data)
{
    fan_out_work *Work = (fan_out_work *)Data;
    app_state *AppState = Work->AppState;
    u8 Column = Work->Column;
    u8 Row = Work->Row;
    
    u32 Index = (Row * AppState->Columns) + Column;
    u8 Tile = AppState->Tiles[Index];
    u8 Flags = UnpackU8High(Tile);
    u8 Mines = UnpackU8Low(Tile);
    
    if((Flags & TileFlag_Visited) == 0)
    {
        if(Flags & TileFlag_Mine)
        {
            AppState->IsGameOver = true;
        }
        Flags |= TileFlag_Visited;
        AppState->Tiles[Index] = PackU8(Flags, Mines);
        
        for(s8 Y = -1; Y < 2; ++Y)
        {
            for(s8 X = -1; X < 2; ++X)
            {
                if((Column + X >= 0) &&
                   (Column + X < AppState->Columns) &&
                   (Row + Y >= 0) &&
                   (Row + Y < AppState->Rows))
                {
                    if(Mines == 0 && !IsMine(AppState, Column + X, Row + Y))
                    {
                        fan_out_work *NewWork = PushStruct(AppState->MemoryFlush.Arena, fan_out_work);
                        NewWork->AppState = AppState;
                        NewWork->Column = Column + X;
                        NewWork->Row = Row + Y;
                        Win32AddWorkEntry(AppState->WorkQueue, FanoutThread, NewWork);
                    }
                }
            }
        }
    }
}

internal void
GenerateBoardThread(void *Data)
{
    app_state *AppState = (app_state *)Data;
    Assert(AppState);
    
    AppState->IsLoading = true;
    
    if(AppState->MemoryFlush.Arena != 0)
    {
        EndTemporaryMemory(AppState->MemoryFlush);
        CheckArena(&AppState->TransientArena);
    }
    AppState->MemoryFlush = BeginTemporaryMemory(&AppState->TransientArena);
    
    AppState->Columns = 8;
    AppState->Rows = 8;
    AppState->Mines = 10;
    AppState->MinesRemaining = 0;
    AppState->IsGameOver = true;
    
    AppState->Tiles = PushArray(AppState->MemoryFlush.Arena, AppState->Rows * AppState->Columns, u8);
    
    while(AppState->Mines > AppState->MinesRemaining)
    {
        u32 Random = RandomU32(&AppState->RandomState) % 64;
        u8 Tile = AppState->Tiles[Random];
        if((UnpackU8High(Tile) & TileFlag_Mine) == 0)
        {
            ++AppState->MinesRemaining;
            AppState->Tiles[Random] = PackU8(TileFlag_Mine, 0);
            
            Sleep(50);
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
                        if(IsMine(AppState, Column + X, Row + Y))
                        {
                            ++Mines;
                        }
                    }
                }
            }
            
            AppState->Tiles[Index] = PackU8(Flags, Mines);
            
            Sleep(50);
        }
    }
    
    AppState->Timer = 0.0f;
    AppState->IsLoading = false;
    AppState->IsGameOver = false;
}

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
    
    PlatformSetWindowSize(V2(320, 464));
    Win32AddWorkEntry(AppState->WorkQueue, GenerateBoardThread, AppState);
}

extern void
AppUpdateFrame(app_memory *AppMemory, render_group *RenderGroup, app_input *Input, f32 DeltaTime)
{
    app_state *AppState = AppMemory->AppState;
    AppState->Timer += DeltaTime;
    RenderGroup->ClearColor = RGBv4(192, 192, 192);
    Assert(AppState);
    
    ui_state *UIState = AppState->UIState;
    BeginUI(UIState, Input);
    
    rectangle2 Bounds = Rectangle2(V2Set1(0), V2(RenderGroup->Width, RenderGroup->Height));
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
                    if(!AppState->IsLoading)
                    {
                        Win32AddWorkEntry(AppState->WorkQueue, GenerateBoardThread, AppState);
                    }
                }
                ui_interaction_state InteractionState = AddUIInteraction(UIState, FaceButtonBounds, Interaction);
                if(InteractionState == UIInteractionState_HotClicked)
                {
                    DrawFace(RenderGroup, &AppState->Sprite, FaceButtonBounds.Min, 0);
                }
                else if(AppState->MinesRemaining == 0)
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
                        
                        ui_interaction Interaction =
                        {
                            .Id = GenerateUIId(AppState->Tiles + Index),
                            .Type = UI_Interaction_ImmediateButton,
                            .Target = AppState->Tiles
                        };
                        
                        if(InteractionsAreEqual(Interaction, UIState->ToExecute))
                        {
                            fan_out_work *NewWork = PushStruct(AppState->MemoryFlush.Arena, fan_out_work);
                            NewWork->AppState = AppState;
                            NewWork->Column = Column;
                            NewWork->Row = Row;
                            Win32AddWorkEntry(AppState->WorkQueue, FanoutThread, NewWork);
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
