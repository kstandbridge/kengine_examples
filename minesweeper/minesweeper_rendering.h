#ifndef MINESWEEPER_RENDERING_H

#define RGBv4(R, G ,B) V4((f32)R/255.0f,(f32)G/255.0f,(f32)B/255.0f, 1.0f)

typedef struct sprite_sheet
{
    s32 Width;
    s32 Height;
    s32 Comp;
    void *Handle;
    
} sprite_sheet;

inline void
DrawNumber(render_group *RenderGroup, sprite_sheet *Sprite, rectangle2 Bounds, u32 Number);

inline void
DrawFace(render_group *RenderGroup, sprite_sheet *Sprite, v2 P, u32 Index);

inline void
DrawButton(render_group *RenderGroup, sprite_sheet *Sprite, v2 P, u32 Index);

inline void
DrawButtonNumber(render_group *RenderGroup, sprite_sheet *Sprite, v2 P, u32 Index);

#define MINESWEEPER_RENDERING_H
#endif //MINESWEEPER_RENDERING_H
