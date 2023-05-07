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
