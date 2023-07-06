#define KENGINE_WIN32
#define KENGINE_DIRECTX
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

typedef struct app_state
{
    memory_arena Arena;
    
    platform_work_queue *BackgroundQueue;
    
    ui_state *UIState;
    app_assets *Assets;
} app_state;

extern void
InitApp(app_memory *AppMemory)
{
    Assert(AppMemory->AppState == 0);
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    
    AppState->BackgroundQueue = PlatformMakeWorkQueue(&AppState->Arena, 2);
    InitAssets(&AppState->Assets, AppState->BackgroundQueue);
    InitUI(&AppState->UIState, AppState->Assets);
}

extern void
AppUpdateFrame(app_memory *AppMemory, render_group *RenderGroup, app_input *Input, f32 DeltaTime)
{
    app_state *AppState = AppMemory->AppState;
    RenderGroup->ClearColor = RGBv4(192, 192, 192);
    
    DeltaTime;
    
    ui_state *UIState = AppState->UIState;
    
    BeginUI(UIState, Input, RenderGroup);
    
    BeginTicketMutex(&AppState->Assets->AssetLock);
    
    v2 P = V2(0, 0);
    f32 Scale = 2.0f;
    
    for(glyph_sprite_sheet *Sprite = AppState->Assets->GlyphSpriteSheets;
        Sprite;
        Sprite = Sprite->Next)
    {
        if(Sprite->AssetState == AssetState_Loaded)
        {
            PushRenderCommandGlyph(RenderGroup, P, 3.0f, V2Multiply(Sprite->Size, V2Set1(Scale)), V4(1, 1, 1, 1), V4(0, 0, 1, 1), Sprite->Handle);
            P.X += (Sprite->Size.X + 10) * Scale;
            
            PushRenderCommandSprite(RenderGroup, P, 3.0f, V2Multiply(Sprite->Size, V2Set1(Scale)), V4(1, 1, 1, 1), V4(0, 0, 1, 1), Sprite->Handle);
            P.X += (Sprite->Size.X + 10) * Scale;
        }
    }
    
    EndTicketMutex(&AppState->Assets->AssetLock);
    
    string LoremIpsum = String("Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer \\u2715 took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.");
    
    rectangle2 Bounds = Rectangle2(UIState->MouseP, V2(RenderGroup->Width, RenderGroup->Height));
    
    DrawTextAt(UIState, Bounds, 4.0f, 2.0f, V4(0.3f, 0, 0.3f, 1), LoremIpsum);
    
    EndUI(UIState);
    CheckArena(&AppState->Arena);
}