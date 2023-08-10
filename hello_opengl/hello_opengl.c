#define KENGINE_OPENGL
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

internal void
AppUpdateAndRender(app_render_commands *RenderCommands)
{
    RenderCommands->RenderWidth = 1024;
    RenderCommands->RenderHeight = 576;

    render_group RenderGroup_ =
    {
        .Commands = RenderCommands
    };
    render_group *RenderGroup = &RenderGroup_;

    PushRect(RenderGroup, V2(10, 10), V2(100, 100), V4Set1(1), 1.0f);

    local_persist memory_arena Arena = {0};
    local_persist loaded_bitmap *HeadBitmap = 0;
    if(HeadBitmap == 0)
    {
        string File = PlatformReadEntireFile(&Arena, String("test_hero_front_head.bmp"));
        HeadBitmap = PushStruct(&Arena, loaded_bitmap);
        *HeadBitmap = LoadBMP(File);
    }
    else
    {
        PushBitmap(RenderGroup, HeadBitmap, V2(150, 150), V2(HeadBitmap->Width, HeadBitmap->Height), V4Set1(1), 2.0f);
    }

    local_persist loaded_bitmap *BackgroundBitmap = 0;
    if(BackgroundBitmap == 0)
    {
        string File = PlatformReadEntireFile(&Arena, String("test_background.bmp"));
        BackgroundBitmap = PushStruct(&Arena, loaded_bitmap);
        *BackgroundBitmap = LoadBMP(File);
    }
    else
    {
        PushBitmap(RenderGroup, BackgroundBitmap, V2Set1(0), V2(BackgroundBitmap->Width, BackgroundBitmap->Height), V4Set1(1), 1.0f);
    }
}