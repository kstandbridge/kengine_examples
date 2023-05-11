#ifndef MINESWEEPER_RENDERING_H

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
