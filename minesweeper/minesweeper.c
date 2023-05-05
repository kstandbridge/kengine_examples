#define KENGINE_WIN32
#define KENGINE_DIRECTX
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

typedef struct sprite_sheet
{
    s32 Width;
    s32 Height;
    s32 Comp;
    void *Handle;
    
} sprite_sheet;

typedef struct tile
{
    u32 Type;
} tile;

typedef struct app_state
{
    sprite_sheet Sprite;
    memory_arena Arena;
    
    ui_state UIState;
    
    tile *Tiles;
    
    f32 Timer;
} app_state;

extern void
InitApp(app_memory *AppMemory)
{
    Assert(AppMemory->AppState == 0);
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    Assert(AppState);
    AppState->Tiles = PushArray(&AppState->Arena, 8*8, tile);
    
    sprite_sheet *Sprite = &AppState->Sprite;
    stbi_uc *Bytes = stbi_load("sprite.png", &Sprite->Width, &Sprite->Height, &Sprite->Comp, 4);
    Sprite->Handle = DirectXLoadTexture(Sprite->Width, Sprite->Height, (u32 *)Bytes);
    free(Bytes);
    
    PlatformSetWindowSize(V2(320, 464));
}

#define RGBv4(R, G ,B) V4((f32)R/255.0f,(f32)G/255.0f,(f32)B/255.0f, 1.0f)
global f32 GlobalScale = 2.0f;

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
DrawTimerNumber(render_group *RenderGroup, sprite_sheet *Sprite, v2 P, u32 Index)
{
    f32 OffsetY = 32.0f;
    v2 Size = V2(13.0f, 23.0f);
    DrawSprite(RenderGroup, Sprite, OffsetY, Index, Size, P);
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

extern void
AppUpdateFrame(app_memory *AppMemory, render_group *RenderGroup, app_input *Input, f32 DeltaTime)
{
    app_state *AppState = AppMemory->AppState;
    AppState->Timer += DeltaTime;
    RenderGroup->ClearColor = RGBv4(192, 192, 192);
    Assert(AppState);
    Input;
    
#if 1
    temporary_memory MemoryFlush = BeginTemporaryMemory(&AppState->Arena);
    ui_state *UIState = &AppState->UIState;
    ui_frame Frame_ = BeginUI(UIState, Input, MemoryFlush.Arena); ui_frame *Frame = &Frame_;
    
    rectangle2 Bounds = Rectangle2(V2Set1(0), V2(RenderGroup->Width, RenderGroup->Height));
    BeginGrid(Frame, Bounds, 1, 2);
    {
        GridSetRowHeight(Frame, 0, 40.0f);
        rectangle2 MenuBounds = GridGetCellBounds(Frame, 0, 0, 0);
        BeginGrid(Frame, MenuBounds, 1, 3);
        {
            
        }
        EndGrid(Frame);
        
        rectangle2 GameBounds = GridGetCellBounds(Frame, 0, 1, 4.0f);
        PushRenderCommandAlternateRectOutline(RenderGroup, GameBounds, 1.0f, 6.0f, 
                                              RGBv4(255, 255, 255), RGBv4(128, 128, 128));
        BeginGrid(Frame, GameBounds, 1, 2);
        {
            GridSetRowHeight(Frame, 0, 104.0f);
            rectangle2 ScoreBounds = GridGetCellBounds(Frame, 0, 0, 16.0f);
            PushRenderCommandAlternateRectOutline(RenderGroup, ScoreBounds, 1.0f, 4.0f, 
                                                  RGBv4(128, 128, 128), RGBv4(255, 255, 255));
            BeginGrid(Frame, ScoreBounds, 3, 1);
            {
                GridSetColumnWidth(Frame, 1, (26.0f+8.0f)*GlobalScale);
                
                rectangle2 MineCountBounds = GridGetCellBounds(Frame, 0, 0, 16.0f);
                PushRenderCommandAlternateRectOutline(RenderGroup, MineCountBounds, 1.0f, 1.0f,
                                                      RGBv4(128, 128, 128), RGBv4(255, 255, 255));
                v2 P = MineCountBounds.Min; P.X += 1; P.Y += 1;
                DrawTimerNumber(RenderGroup, &AppState->Sprite, P, (u32)(4*AppState->Timer) % 8);
                P.X += 13.0f*GlobalScale;
                DrawTimerNumber(RenderGroup, &AppState->Sprite, P, (u32)(2*AppState->Timer) % 8);
                P.X += 13.0f*GlobalScale;
                DrawTimerNumber(RenderGroup, &AppState->Sprite, P, (u32)(1*AppState->Timer) % 8);
                
                rectangle2 FaceButtonBounds = GridGetCellBounds(Frame, 1, 0, 16.0f);
                DrawFace(RenderGroup, &AppState->Sprite, FaceButtonBounds.Min, 1);
                
                rectangle2 TimerBounds = GridGetCellBounds(Frame, 2, 0, 16.0f);
                PushRenderCommandAlternateRectOutline(RenderGroup, TimerBounds, 1.0f, 1.0f,
                                                      RGBv4(128, 128, 128), RGBv4(255, 255, 255));
                P = TimerBounds.Min; P.X += 1; P.Y += 1;
                DrawTimerNumber(RenderGroup, &AppState->Sprite, P, (u32)(9*AppState->Timer) % 8);
                P.X += 13.0f*GlobalScale;
                DrawTimerNumber(RenderGroup, &AppState->Sprite, P, (u32)(3*AppState->Timer) % 8);
                P.X += 13.0f*GlobalScale;
                DrawTimerNumber(RenderGroup, &AppState->Sprite, P, (u32)(5*AppState->Timer) % 8);
            }
            EndGrid(Frame);
            
            rectangle2 BoardBounds = GridGetCellBounds(Frame, 0, 1, 16.0f);
            PushRenderCommandAlternateRectOutline(RenderGroup, BoardBounds, 1.0f, 4.0f, 
                                                  RGBv4(128, 128, 128), RGBv4(255, 255, 255));
            BoardBounds = Rectangle2AddRadiusTo(BoardBounds, -4.0f);
            
            BeginGrid(Frame, BoardBounds, 8, 8);
            {
                for(u32 Row = 0;
                    Row < 8;
                    ++Row)
                {
                    for(u32 Column = 0;
                        Column < 8;
                        ++Column)
                    {
                        rectangle2 TileBounds = GridGetCellBounds(Frame, Column, Row, 0.0f);
                        tile *Tile = AppState->Tiles + (Row * 8) + Column;
                        
                        ui_interaction Interaction =
                        {
                            .Id = GenerateUIId(Tile),
                            .Type = UI_Interaction_ImmediateButton,
                            .Target = 0
                        };
                        
                        if(InteractionsAreEqual(Interaction, UIState->ToExecute))
                        {
                            ++Tile->Type;
                            LogDebug("%u, %u = %u", Column, Row, Tile->Type);
                        }
                        
                        
                        ui_interaction_state InteractionState = AddUIInteraction(UIState, TileBounds, Interaction);
                        switch(InteractionState)
                        {
                            
#if 0                            
                            case UIInteractionState_HotClicked:
                            {
                                DrawButtonNumber(RenderGroup, &AppState->Sprite, TileBounds.Min, 7);
                            } break;
                            
                            case UIInteractionState_Hot:
                            {
                                DrawButtonNumber(RenderGroup, &AppState->Sprite, TileBounds.Min, 0);
                            } break;
                            
                            case UIInteractionState_Selected:
                            {
                                DrawButtonNumber(RenderGroup, &AppState->Sprite, TileBounds.Min, 3);
                            } break;
#endif
                            
                            default:
                            {
                                DrawButtonNumber(RenderGroup, &AppState->Sprite, TileBounds.Min, Tile->Type % 8);
                            } break;
                        }
                        
                        
                    }
                }
            }
            EndGrid(Frame);
        }
        EndGrid(Frame);
        
    }
    EndGrid(Frame);
    
    EndUI(UIState, Input);
    EndTemporaryMemory(MemoryFlush);
    
    
#else
    // NOTE(kstandbridge): Texture test
    {    
        v2 P = V2(32, 600);
        sprite_sheet *Sprite = &AppState->Sprite;
        v2 Size = V2(Sprite->Width*GlobalScale, Sprite->Height*GlobalScale);
        PushRenderCommandSprite(RenderGroup, P, 3.0f, Size, V4(1, 1, 1, 1), V4(0, 0, 1, 1), Sprite->Handle);
    }
    
    // NOTE(kstandbridge): Sprite test
    {    
        DrawTimerNumber(RenderGroup, &AppState->Sprite, V2(640, 100), (u32)AppState->Timer % 10);
        DrawFace(RenderGroup, &AppState->Sprite, V2(640, 300), (u32)AppState->Timer % 5);
        DrawButton(RenderGroup, &AppState->Sprite, V2(640, 500), (u32)AppState->Timer % 6);
        DrawButtonNumber(RenderGroup, &AppState->Sprite, V2(640, 700), (u32)AppState->Timer % 9);
    }
    
    // NOTE(kstandbridge): Board test
    {
        f32 CellSize = 16.0f;
        u32 BoardColumns = 8;
        u32 BoardRows = 8;
        v2 P = V2(32, 32);
        
        for(u32 Row = 0;
            Row < BoardRows;
            ++Row)
        {
            for(u32 Column = 0;
                Column < BoardColumns;
                ++Column)
            {
                DrawButton(RenderGroup, &AppState->Sprite, P, 5);
                P.X += CellSize*GlobalScale;
            }
            P.X = 32;
            P.Y += CellSize*GlobalScale;
        }
    }
#endif
    
}
