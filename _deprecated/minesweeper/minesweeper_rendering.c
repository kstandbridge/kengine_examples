internal void
LoadSpriteSheetThread(memory_arena *TransientArena, sprite_sheet *Sprite)
{
    Sprite->AssetState = AssetState_Processing;
    
    string File = PlatformReadEntireFile(TransientArena, String("sprite.png"));
    stbi_uc *Bytes = stbi_load_from_memory(File.Data, (s32)File.Size, &Sprite->Width, &Sprite->Height, &Sprite->Comp, 4);
    Sprite->Handle = DirectXLoadTexture(Sprite->Width, Sprite->Height, (u32 *)Bytes);
    
    Sprite->AssetState = AssetState_Loaded;
}

inline void
DrawSprite(app_state *AppState, render_group *RenderGroup, f32 OffsetY, u32 Index, v2 Size, v2 P)
{
    sprite_sheet *Sprite = &AppState->Sprite;
    
    v4 UV = V4((Index*Size.X), 
               OffsetY, 
               ((Index+1)*Size.X), 
               (Size.Y + OffsetY));
    UV.R = UV.R / Sprite->Width;
    UV.G = UV.G / Sprite->Height;
    UV.B = UV.B / Sprite->Width;
    UV.A = UV.A / Sprite->Height;
    
    Size = V2Multiply(Size, V2Set1(GlobalScale));
    
    BeginTicketMutex(&AppState->AssetLock);
    
    if(Sprite->AssetState == AssetState_Loaded)
    {
        PushRenderCommandSprite(RenderGroup, P, 1.0f, Size, V4(1, 1, 1, 1), UV, Sprite->Handle);
    }
    else if(Sprite->AssetState == AssetState_Unloaded)
    {
        Sprite->AssetState = AssetState_Queued;
        PlatformAddWorkEntry(AppState->BackgroundQueue, LoadSpriteSheetThread, Sprite);
    }
    
    EndTicketMutex(&AppState->AssetLock);
}

inline void
DrawNumber_(app_state *AppState, render_group *RenderGroup, v2 P, u32 Index)
{
    f32 OffsetY = 32.0f;
    v2 Size = V2(13.0f, 23.0f);
    DrawSprite(AppState, RenderGroup, OffsetY, Index, Size, P);
}

inline void
DrawNumber(app_state *AppState, render_group *RenderGroup, rectangle2 Bounds, u32 Number)
{
    PushRenderCommandAlternateRectOutline(RenderGroup, Bounds, 1.0f, 1.0f,
                                          RGBv4(128, 128, 128), RGBv4(255, 255, 255));
    v2 P = Bounds.Min; P.X += 1; P.Y += 1;
    DrawNumber_(AppState, RenderGroup, P, (Number/100) % 10);
    P.X += 13.0f*GlobalScale;
    DrawNumber_(AppState, RenderGroup, P, (Number/10) % 10);
    P.X += 13.0f*GlobalScale;
    DrawNumber_(AppState, RenderGroup, P, Number % 10);
}

inline void
DrawFace(app_state *AppState, render_group *RenderGroup, v2 P, u32 Index)
{
    f32 OffsetY = 55.0f;
    v2 Size = V2(26.0f, 26.0f);
    DrawSprite(AppState, RenderGroup, OffsetY, Index, Size, P);
}

inline void
DrawButton(app_state *AppState, render_group *RenderGroup, v2 P, u32 Index)
{
    f32 OffsetY = 16.0f;
    v2 Size = V2(16.0f, 16.0f);
    DrawSprite(AppState, RenderGroup, OffsetY, Index, Size, P);
}

inline void
DrawButtonNumber(app_state *AppState, render_group *RenderGroup, v2 P, u32 Index)
{
    f32 OffsetY = 0.0f;
    v2 Size = V2(16.0f, 16.0f);
    DrawSprite(AppState, RenderGroup, OffsetY, Index, Size, P);
}
