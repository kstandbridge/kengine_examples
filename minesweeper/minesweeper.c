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

extern void
AppUpdateFrame(app_memory *AppMemory, render_group *RenderGroup, app_input *Input, f32 DeltaTime)
{
    app_state *AppState = AppMemory->AppState;
    RenderGroup->ClearColor = GlobalBackColor;
    Assert(AppState);
    Input;
    DeltaTime;
    
#if 0
    // NOTE(kstandbridge): Rect test
#define BOX_WIDTH 10
#define BOX_HEIGHT 10
#define BOX_PADDING 3
    u32 Columns = RenderGroup->Width / (BOX_WIDTH + BOX_PADDING);
    u32 Rows = RenderGroup->Height / (BOX_HEIGHT + BOX_PADDING);
    u32 AtX = BOX_PADDING;
    u32 AtY = BOX_PADDING;
    
    for(u32 Row = 0;
        Row < Rows;
        ++Row)
    {
        for(u32 Column = 0;
            Column < Columns;
            ++Column)
        {
            v2 P = V2((f32)AtX, (f32)AtY);
            v2 Size = V2(BOX_WIDTH, BOX_HEIGHT);
            v4 Color = V4(0.3f, 0.5f, 0.2f, 1.0f);
            PushRenderCommandRect(RenderGroup, Rectangle2(P, V2Add(P, Size)), 1.0f, Color);
            AtX += BOX_WIDTH + BOX_PADDING;
        }
        AtX = BOX_PADDING;
        AtY += BOX_HEIGHT + BOX_PADDING;
    }
#endif
    
#if 1
    // NOTE(kstandbridge): Texture test
    
    {    
        v2 P = V2(200, 100);
        sprite_sheet *Sprite = &AppState->Sprite;
        v2 Size = V2(Sprite->Width, Sprite->Height);
        PushRenderCommandSprite(RenderGroup, P, 3.0f, Size, V4(1, 1, 1, 1), V4(0, 0, 1, 1), Sprite->Handle);
    }
    
#endif
    
}