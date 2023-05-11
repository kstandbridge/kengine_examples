#ifndef MINESWEEPER_RENDERING_H

typedef enum asset_state_type
{
    AssetState_Unloaded,
    AssetState_Queued,
    AssetState_Loaded,
} asset_state_type;

typedef struct sprite_sheet
{
    asset_state_type StateType;
    
    s32 Width;
    s32 Height;
    s32 Comp;
    void *Handle;
} sprite_sheet;

inline void
DrawNumber(struct app_state *AppState, render_group *RenderGroup, rectangle2 Bounds, u32 Number);

inline void
DrawFace(struct app_state *AppState, render_group *RenderGroup, v2 P, u32 Index);

inline void
DrawButton(struct app_state *AppState, render_group *RenderGroup, v2 P, u32 Index);

inline void
DrawButtonNumber(struct app_state *AppState, render_group *RenderGroup, v2 P, u32 Index);

#define MINESWEEPER_RENDERING_H
#endif //MINESWEEPER_RENDERING_H
