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

typedef struct app_state
{
    sprite_sheet Sprite;
    memory_arena *Arena;
    
    f32 Timer;
} app_state;

extern void
InitApp(app_memory *AppMemory)
{
    Assert(AppMemory->AppState == 0);
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    Assert(AppState);
    
    temporary_memory MemoryFlush = BeginTemporaryMemory(AppState->Arena);
    {    
        string File = Win32ReadEntireFile(MemoryFlush.Arena, String("minesweeper-sprites.png"));
        Assert(File.Data);
        // TODO(kstandbridge): Memory leak on sbi texture
        sprite_sheet *Sprite = &AppState->Sprite;
        stbi_uc *Bytes = stbi_load_from_memory(File.Data, (s32)File.Size, &Sprite->Width, &Sprite->Height, &Sprite->Comp, 4);
        Sprite->Handle = DirectXLoadTexture(Sprite->Width, Sprite->Height, (u32 *)Bytes);
    }
    EndTemporaryMemory(MemoryFlush);
}

#define RGBv4(R, G ,B) {R/255.0f,G/255.0f,B/255.0f, 1.0f }
global v4 GlobalBackColor = RGBv4(192, 199, 200);
global f32 GlobalScale = 4.0f;

inline void
DrawSprite(render_group *RenderGroup, sprite_sheet *Sprite, f32 OffsetY, u32 Index, v2 Size, v2 P)
{
    v4 UV = V4((Index*Size.X + Index), 
               OffsetY, 
               ((Index+1)*Size.X + Index), 
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
    f32 OffsetY = 0.0f;
    v2 Size = V2(13.0f, 23.0f);
    DrawSprite(RenderGroup, Sprite, OffsetY, Index, Size, P);
}

inline void
DrawFace(render_group *RenderGroup, sprite_sheet *Sprite, v2 P, u32 Index)
{
    f32 OffsetY = 24.0f;
    v2 Size = V2(26.0f, 26.0f);
    DrawSprite(RenderGroup, Sprite, OffsetY, Index, Size, P);
}

inline void
DrawButton(render_group *RenderGroup, sprite_sheet *Sprite, v2 P, u32 Index)
{
    f32 OffsetY = 51.0f;
    v2 Size = V2(16.0f, 16.0f);
    DrawSprite(RenderGroup, Sprite, OffsetY, Index, Size, P);
}

inline void
DrawButtonNumber(render_group *RenderGroup, sprite_sheet *Sprite, v2 P, u32 Index)
{
    f32 OffsetY = 68.0f;
    v2 Size = V2(16.0f, 16.0f);
    DrawSprite(RenderGroup, Sprite, OffsetY, Index, Size, P);
}

extern void
AppUpdateFrame(app_memory *AppMemory, render_group *RenderGroup, app_input *Input, f32 DeltaTime)
{
    app_state *AppState = AppMemory->AppState;
    AppState->Timer += DeltaTime;
    RenderGroup->ClearColor = GlobalBackColor;
    Assert(AppState);
    Input;
    
    
#if 1    
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
        DrawButton(RenderGroup, &AppState->Sprite, V2(640, 500), (u32)AppState->Timer % 8);
        DrawButtonNumber(RenderGroup, &AppState->Sprite, V2(640, 700), (u32)AppState->Timer % 8);
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
                DrawButton(RenderGroup, &AppState->Sprite, P, 0);
                P.X += CellSize*GlobalScale;
            }
            P.X = 32;
            P.Y += CellSize*GlobalScale;
        }
    }
#endif
    
}