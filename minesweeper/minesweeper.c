#define KENGINE_WIN32
#define KENGINE_DIRECTX
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

typedef struct app_state
{
    memory_arena *Arena;
} app_state;

extern void
InitApp(app_memory *AppMemory)
{
    Assert(AppMemory->AppState == 0);
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    Assert(AppState);
}

extern void
AppUpdateFrame(app_memory *AppMemory, render_group *RenderGroup, app_input *Input, f32 DeltaTime)
{
    app_state *AppState = AppMemory->AppState;
    Assert(AppState);
    Input;
    DeltaTime;
    
#if 1
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
}