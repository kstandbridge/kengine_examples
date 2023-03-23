#define KENGINE_WIN32
#define KENGINE_DIRECTX
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

global f32 GlobalMargin = 7.0f;

#define RGBv4(R, G ,B) {R/255.0f,G/255.0f,B/255.0f, 1.0f }
global v4 GlobalBackColor = RGBv4(240, 240, 240);
global v4 GlobalFormColor = RGBv4(255, 255, 255);
global v4 GlobalFormTextColor = RGBv4(0, 0, 0);
global v4 GlobalFormDisabledTextColor = RGBv4(131, 131, 131);

global v4 GlobalButtonBackColor = RGBv4(225, 225, 225);
global v4 GlobalButtonBorderColor = RGBv4(173, 173, 173);

global v4 GlobalButtonDisabledBackColor = RGBv4(204, 204, 204);
global v4 GlobalButtonDisabledBorderColor = RGBv4(191, 191, 191);

global v4 GlobalButtonHotBackColor = RGBv4(229, 241, 251);
global v4 GlobalButtonHotBorderColor = RGBv4(0, 120, 215);

global v4 GlobalButtonClickedBackColor = RGBv4(204, 228, 247);
global v4 GlobalButtonClickedBorderColor = RGBv4(0, 84, 153);

global v4 GlobalTabButtonBorder = RGBv4(217, 217, 217);
global v4 GlobalTabButtonBackground = RGBv4(240, 240, 240);
global v4 GlobalTabButtonHot = RGBv4(216, 234, 249);
global v4 GlobalTabButtonClicked = RGBv4(204, 228, 247);
global v4 GlobalTabButtonSelected = RGBv4(255, 255, 255);

global v4 GlobalGroupBorder = RGBv4(220, 220, 220);

typedef struct ui_grid
{
    rectangle2 Bounds;
    u32 Columns;
    u32 Rows;
    
    b32 GridSizeCalculated;
    f32 *ColumnWidths;
    f32 *RowHeights;
    
    f32 DefaultRowHeight;
    
    struct ui_grid *Prev;
} ui_grid;

typedef union ui_id_value
{
    void *Void;
    u64 U64;
    u32 U32[2];
} ui_id_value;

typedef struct ui_id
{
    ui_id_value Value[2];
} ui_id;

typedef enum ui_interaction_type
{
    UI_Interaction_None,
    
    UI_Interaction_NOP,
    
    UI_Interaction_Draggable,
    UI_Interaction_ImmediateButton,
    
} ui_interaction_type;

typedef struct ui_interaction
{
    ui_id Id;
    ui_interaction_type Type;
    
    void *Target;
    union
    {
        void *Generic;
        v2 *P;
    };
} ui_interaction;

typedef struct ui_frame
{
    memory_arena *Arena;
    render_group *RenderGroup;
    app_input *Input;
    ui_grid *CurrentGrid;
} ui_frame;

typedef struct glyph_info
{
    u8 *Data;
    
    u32 CodePoint;
    
    s32 Width;
    s32 Height;
    s32 XOffset;
    s32 YOffset;
    
    s32 AdvanceWidth;
    s32 LeftSideBearing;
    
    v4 UV;
    
} glyph_info;


typedef enum properties_tab_type
{
    PropertiesTab_General,
    PropertiesTab_Sharing,
    PropertiesTab_Security,
    PropertiesTab_PreviousVersions,
    PropertiesTab_Customize,
} properties_tab_type;

typedef struct app_state
{
    memory_arena Arena;
    
    properties_tab_type SelectedPropertyTab;
    
    f32 FontScale;
    s32 FontAscent;
    s32 FontDescent;
    s32 FontLineGap;
    stbtt_fontinfo FontInfo;
    glyph_info GlyphInfos[256];
    
    v2 MouseP;
    v2 dMouseP;
    v2 LastMouseP;
    
    ui_interaction Interaction;
    
    ui_interaction HotInteraction;
    ui_interaction NextHotInteraction;
    
    ui_interaction ToExecute;
    ui_interaction NextToExecute;
    
    ui_interaction SelectedInteration;
    
    // NOTE(kstandbridge): Transient
    ui_frame *Frame;
    
} app_state;

typedef enum text_op
{
    TextOp_Draw,
    TextOp_Size
} text_op;
internal rectangle2
TextOp_(app_state *AppState, render_group *RenderGroup, rectangle2 Bounds, f32 Depth, f32 Scale, v4 Color, string Text, text_op Op)
{
    rectangle2 Result = Rectangle2(Bounds.Min, Bounds.Min);
    v2 Offset = Bounds.Min;
    
    f32 AtX = Offset.X;
    f32 AtY = Offset.Y + AppState->FontScale*AppState->FontAscent*Scale;
    
    for(umm Index = 0;
        Index < Text.Size;
        ++Index)
    {
        u32 CodePoint = Text.Data[Index];
        
        if(CodePoint == '\n')
        {
            AtY += AppState->FontScale*AppState->FontAscent*Scale;
            AtX = Offset.X;
        }
        else
        {
            b32 WordWrap = false;
            if(IsWhitespace((char)CodePoint))
            {
                f32 WordWidth = 0;
                for(umm WordIndex = 1;
                    (Index + WordIndex) < Text.Size;
                    ++WordIndex)
                {
                    u32 WordCodePoint = Text.Data[Index + WordIndex];
                    if(IsWhitespace((char)WordCodePoint))
                    {
                        break;
                    }
                    glyph_info *Info = AppState->GlyphInfos + WordCodePoint;
                    Assert(Info->CodePoint == WordCodePoint);
                    
                    WordWidth += AppState->FontScale*Info->AdvanceWidth*Scale;
                    
                    if(Index < Text.Size)
                    {
                        s32 Kerning = stbtt_GetCodepointKernAdvance(&AppState->FontInfo, CodePoint, Text.Data[Index + 1]);
                        if(Kerning > 0)
                        {
                            WordWidth += AppState->FontScale*Kerning*Scale;
                        }
                    }
                    
                }
                WordWrap = (AtX + WordWidth) > Bounds.Max.X;
            }
            
            if(WordWrap)
            {
                AtY += AppState->FontScale*AppState->FontAscent*Scale;
                AtX = Offset.X;
            }
            else
            {            
                glyph_info *Info = AppState->GlyphInfos + CodePoint;
                Assert(Info->CodePoint == CodePoint);
                
                v2 Size = V2(Scale*Info->Width, Scale*Info->Height);
                
                v2 GlyphOffset = V2(AtX, AtY + Info->YOffset*Scale);
                if(Op == TextOp_Draw)
                {
                    PushRenderCommandGlyph(RenderGroup, GlyphOffset, Depth, Size, Color, Info->UV);
                }
                else
                {
                    Assert(Op == TextOp_Size);
                    Result = Rectangle2Union(Result, Rectangle2(GlyphOffset, V2Add(GlyphOffset, Size)));
                }
                
                AtX += AppState->FontScale*Info->AdvanceWidth*Scale;
                
                if(Index < Text.Size)
                {
                    s32 Kerning = stbtt_GetCodepointKernAdvance(&AppState->FontInfo, CodePoint, Text.Data[Index + 1]);
                    AtX += AppState->FontScale*Kerning*Scale;
                }
            }
            
        }
    }
    
    return Result;
}

internal void
DrawTextAt(app_state *AppState, render_group *RenderGroup, rectangle2 Bounds, f32 Depth, f32 Scale, v4 Color, string Text)
{
    TextOp_(AppState, RenderGroup, Bounds, Depth, Scale, Color, Text, TextOp_Draw);
}

internal rectangle2
GetTextBounds(app_state *AppState, render_group *RenderGroup, rectangle2 Bounds, f32 Depth, f32 Scale, v4 Color, string Text)
{
    rectangle2 Result = TextOp_(AppState, RenderGroup, Bounds, Depth, Scale, Color, Text, TextOp_Size);
    return Result;
}

#define GenerateUIId__(Ptr, File, Line) GenerateUIId___(Ptr, File "|" #Line)
#define GenerateUIId_(Ptr, File, Line) GenerateUIId__(Ptr, File, Line)
#define GenerateUIId(Ptr) GenerateUIId_((Ptr), __FILE__, __LINE__)
inline ui_id
GenerateUIId___(void *Ptr, char *Text)
{
    ui_id Result =
    {
        .Value = 
        {
            Ptr,
            Text
        },
    };
    
    return Result;
}


inline void
ClearInteraction(ui_interaction *Interaction)
{
    Interaction->Type = UI_Interaction_None;
    Interaction->Generic = 0;
}

inline b32
InteractionsAreEqual(ui_interaction A, ui_interaction B)
{
    b32 Result = ((A.Id.Value[0].U64 == B.Id.Value[0].U64) &&
                  (A.Id.Value[1].U64 == B.Id.Value[1].U64) &&
                  (A.Type == B.Type) &&
                  (A.Target == B.Target) &&
                  (A.Generic == B.Generic));
    
    return Result;
}

inline b32
InteractionIsHot(app_state *AppState, ui_interaction B)
{
    b32 Result = InteractionsAreEqual(AppState->HotInteraction, B);
    
    if(B.Type == UI_Interaction_None)
    {
        Result = false;
    }
    
    return Result;
}


internal void
BeginGrid(app_state *AppState, rectangle2 Bounds, u32 Columns, u32 Rows)
{
    ui_frame *Frame = AppState->Frame;
    memory_arena *Arena = Frame->Arena;
    
    ui_grid *Grid = PushStruct(Arena, ui_grid);
    Grid->Prev = Frame->CurrentGrid;
    Frame->CurrentGrid = Grid;
    
    Grid->Bounds = Bounds;
    Grid->Columns = Columns;
    Grid->Rows = Rows;
    
    Grid->GridSizeCalculated = false;
    Grid->ColumnWidths = PushArray(Arena, Columns, f32);
    for(u32 ColumnIndex = 0;
        ColumnIndex < Columns;
        ++ColumnIndex)
    {
        Grid->ColumnWidths[ColumnIndex] = 0.0f;
    }
    
    Grid->DefaultRowHeight = AppState->FontScale*AppState->FontAscent*2.0f;
    Grid->RowHeights = PushArray(Arena, Rows, f32);
    for(u32 RowIndex = 0;
        RowIndex < Rows;
        ++RowIndex)
    {
        Grid->RowHeights[RowIndex] = 0.0f;
    }
}

internal void
EndGrid(app_state *AppState)
{
    ui_frame *Frame = AppState->Frame;
    Assert(Frame->CurrentGrid);
    Frame->CurrentGrid = Frame->CurrentGrid->Prev;
}

internal void
GridSetRowHeight(app_state *AppState, u32 Row, f32 Height)
{
    ui_frame *Frame = AppState->Frame;
    Assert(Frame->CurrentGrid);
    ui_grid *Grid = Frame->CurrentGrid;
    Assert(Row <= Grid->Rows);
    
    // NOTE(kstandbridge): Sizes must be set before a control is drawn
    Assert(!Grid->GridSizeCalculated);
    
    Grid->RowHeights[Row] = Height;
}

internal void
GridSetColumnWidth(app_state *AppState, u32 Column, f32 Width)
{
    ui_frame *Frame = AppState->Frame;
    Assert(Frame->CurrentGrid);
    ui_grid *Grid = Frame->CurrentGrid;
    Assert(Column <= Grid->Columns);
    
    // NOTE(kstandbridge): Sizes must be set before a control is drawn
    Assert(!Grid->GridSizeCalculated);
    
    Grid->ColumnWidths[Column] = Width;
}

internal rectangle2
GridGetCellBounds(app_state *AppState, u32 Column, u32 Row)
{
    ui_frame *Frame = AppState->Frame;
    Assert(Frame->CurrentGrid);
    ui_grid *Grid = Frame->CurrentGrid;
    
    if(!Grid->GridSizeCalculated)
    {
        
        v2 Dim = Rectangle2GetDim(Grid->Bounds);
        u32 ColumnsFilled = 0;
        f32 WidthUsed = 0.0f;
        for(u32 ColumnIndex = 0;
            ColumnIndex < Grid->Columns;
            ++ColumnIndex)
        {
            if(Grid->ColumnWidths[ColumnIndex] == 0.0f)
            {
                ++ColumnsFilled;
            }
            else
            {
                WidthUsed += Grid->ColumnWidths[ColumnIndex];
            }
        }
        if(ColumnsFilled > 0)
        {
            f32 WidthRemaining = Dim.X - WidthUsed;
            f32 ColumnWidth = WidthRemaining / ColumnsFilled;
            
            for(u32 ColumnIndex = 0;
                ColumnIndex < Grid->Columns;
                ++ColumnIndex)
            {
                if(Grid->ColumnWidths[ColumnIndex] == 0.0f)
                {
                    Grid->ColumnWidths[ColumnIndex] = ColumnWidth;
                }
            }
        }
        
        u32 RowsFilled = 0;
        f32 HeightUsed = 0.0f;
        for(u32 RowIndex = 0;
            RowIndex < Grid->Rows;
            ++RowIndex)
        {
            if(Grid->RowHeights[RowIndex] == 0.0f)
            {
                ++RowsFilled;
            }
            else
            {
                HeightUsed += Grid->RowHeights[RowIndex];
            }
        }
        if(RowsFilled > 0)
        {
            f32 HeightRemaining = Dim.Y - HeightUsed;
            f32 RowHeight = HeightRemaining / RowsFilled;
            
            for(u32 RowIndex = 0;
                RowIndex < Grid->Rows;
                ++RowIndex)
            {
                if(Grid->RowHeights[RowIndex] == 0.0f)
                {
                    Grid->RowHeights[RowIndex] = RowHeight;
                }
            }
        }
        
        Grid->GridSizeCalculated = true;
    }
    
    v2 Min = Grid->Bounds.Min;
    for(u32 ColumnIndex = 0;
        ColumnIndex < Column;
        ++ColumnIndex)
    {
        Min.X += Grid->ColumnWidths[ColumnIndex];
    }
    
    for(u32 RowIndex = 0;
        RowIndex < Row;
        ++RowIndex)
    {
        Min.Y += Grid->RowHeights[RowIndex];
    }
    
    v2 Max = V2Add(Min, V2(Grid->ColumnWidths[Column], Grid->RowHeights[Row]));
    
    rectangle2 Result = Rectangle2(Min, Max);
    return Result;
}

typedef enum ui_interaction_state
{
    UIInteractionState_None,
    UIInteractionState_HotClicked,
    UIInteractionState_Hot,
    UIInteractionState_Selected
} ui_interaction_state;

internal ui_interaction_state
AddUIInteraction(app_state *AppState, rectangle2 Bounds, ui_interaction Interaction)
{
    ui_interaction_state Result = UIInteractionState_None;
    
    Assert(Interaction.Type);
    
    if(Rectangle2IsIn(Bounds, AppState->MouseP))
    {
        AppState->NextHotInteraction = Interaction;
    }
    
    if(InteractionsAreEqual(Interaction, AppState->Interaction) &&
       InteractionsAreEqual(Interaction, AppState->NextHotInteraction))
    {
        Result = UIInteractionState_HotClicked;
    }
    else if(InteractionsAreEqual(Interaction, AppState->HotInteraction))
    {
        Result = UIInteractionState_Hot;
    }
    else if(InteractionsAreEqual(Interaction, AppState->SelectedInteration))
    {
        Result = UIInteractionState_Selected;
    }
    else
    {
        Result = UIInteractionState_None;
    }
    
    return Result;
}

#define Button(AppState, Column, Row, Enabled, Text) Button_(AppState, Column, Row, Enabled, Text, GenerateUIId(0))
internal b32
Button_(app_state *AppState, u32 Column, u32 Row, b32 Enabled, string Text, ui_id Id)
{
    b32 Result = false;
    
    ui_frame *Frame = AppState->Frame;
    render_group *RenderGroup = Frame->RenderGroup;
    Assert(Frame->CurrentGrid);
    
    rectangle2 Bounds = GridGetCellBounds(AppState, Column, Row);
    Bounds.Max = V2Subtract(Bounds.Max, V2Set1(GlobalMargin));
    
    v2 Dim = Rectangle2GetDim(Bounds);
    
    v4 TextColor = GlobalFormTextColor;
    
    if(Enabled)
    {    
        ui_interaction Interaction =
        {
            .Id = Id,
            .Type = UI_Interaction_ImmediateButton,
            .Target = 0
        };
        
        Result = InteractionsAreEqual(Interaction, AppState->ToExecute);
        
        
        ui_interaction_state InteractionState = AddUIInteraction(AppState, Bounds, Interaction);
        switch(InteractionState)
        {
            case UIInteractionState_HotClicked:
            {
                PushRenderCommandRect(RenderGroup, Bounds, 1.0f, GlobalButtonClickedBackColor);
                PushRenderCommandRectOutline(RenderGroup, Bounds, 1.0f, GlobalButtonClickedBorderColor, 1);
            } break;
            
            case UIInteractionState_Hot:
            {
                PushRenderCommandRect(RenderGroup, Bounds, 1.0f, GlobalButtonHotBackColor);
                PushRenderCommandRectOutline(RenderGroup, Bounds, 1.0f, GlobalButtonHotBorderColor, 1);
            } break;
            
            case UIInteractionState_Selected:
            {
                PushRenderCommandRect(RenderGroup, Bounds, 1.0f, GlobalButtonBackColor);
                PushRenderCommandRectOutline(RenderGroup, Bounds, 1.0f, GlobalButtonHotBorderColor, 2);
            } break;
            
            default:
            {
                PushRenderCommandRect(RenderGroup, Bounds, 1.0f, GlobalButtonBackColor);
                PushRenderCommandRectOutline(RenderGroup, Bounds, 1.0f, GlobalButtonBorderColor, 1);
            } break;
        }
    }
    else
    {
        TextColor = GlobalFormDisabledTextColor;
        PushRenderCommandRect(RenderGroup, Bounds, 1.0f, GlobalButtonDisabledBackColor);
        PushRenderCommandRectOutline(RenderGroup, Bounds, 1.0f, GlobalButtonDisabledBorderColor, 1);
    }
    
    rectangle2 TextBounds = GetTextBounds(AppState, RenderGroup, Bounds, 2.0f, 1.0f, TextColor, Text);
    
    v2 TextDim = Rectangle2GetDim(TextBounds);
    
    v2 TextOffset = V2Add(Bounds.Min,
                          V2Subtract(V2Multiply(Dim, V2Set1(0.5f)),
                                     V2Multiply(TextDim, V2Set1(0.5f))));
    
    DrawTextAt(AppState, RenderGroup, Rectangle2(TextOffset, V2Add(TextOffset, TextDim))
               , 2.0f, 1.0f, TextColor, Text);
    
    return Result;
}

#define Label(AppState, Column, Row, Text) Label_(AppState, Column, Row, GenerateUIId(0), Text);
internal void
Label_(app_state *AppState, u32 Column, u32 Row, ui_id Id, string Text)
{
    Id;
    
    ui_frame *Frame = AppState->Frame;
    render_group *RenderGroup = Frame->RenderGroup;
    Assert(Frame->CurrentGrid);
    
    rectangle2 Bounds = GridGetCellBounds(AppState, Column, Row);
    
    DrawTextAt(AppState, RenderGroup, Bounds, 2.0f, 1.0f, GlobalFormTextColor, Text);
    
}

internal void
Slider(app_state *AppState, u32 Column, u32 Row, ui_id Id, f32 *Target, string Text)
{
    ui_frame *Frame = AppState->Frame;
    render_group *RenderGroup = Frame->RenderGroup;
    Assert(Frame->CurrentGrid);
    
    rectangle2 Bounds = GridGetCellBounds(AppState, Column, Row);
    
    ui_interaction Interaction =
    {
        .Id = Id,
        .Type = UI_Interaction_Draggable,
        .Target = Target
    };
    
    v4 ButtonColor = V4(1.0f, 0.5f, 0.0f, 1.0f);
    if(Rectangle2IsIn(Bounds, AppState->MouseP))
    {
        AppState->NextHotInteraction = Interaction;
        ButtonColor = V4(0.5f, 1.0f, 0.0f, 1.0f);
    }
    
    if(InteractionsAreEqual(Interaction, AppState->Interaction))
    {
        *Target += 0.01f*AppState->dMouseP.X;
    }
    
    PushRenderCommandRect(RenderGroup, Bounds, 1.0f, ButtonColor);
    
    DrawTextAt(AppState, RenderGroup, Bounds, 2.0f, 1.0f, V4(0.0f, 0.0f, 0.0f, 1.0f), Text);
}

internal rectangle2
GroupControl(app_state *AppState, u32 Column, u32 Row, string Header)
{
    rectangle2 Result = GridGetCellBounds(AppState, Column, Row);
    
    ui_frame *Frame = AppState->Frame;
    render_group *RenderGroup = Frame->RenderGroup;
    
    v2 TextP = V2(Result.Min.X + GlobalMargin, Result.Min.Y);
    rectangle2 TextBounds = GetTextBounds(AppState, RenderGroup, Rectangle2(TextP, Result.Max),
                                          2.0f, 1.0f, GlobalFormTextColor, Header);
    TextBounds.Min.X -= GlobalMargin*0.25f;
    TextBounds.Max.X += GlobalMargin*0.25f;
    PushRenderCommandRect(RenderGroup, TextBounds, 2.0f, GlobalFormColor);
    DrawTextAt(AppState, RenderGroup, Rectangle2(TextP, Result.Max), 
               3.0f, 1.0f, GlobalFormTextColor,Header);
    Result.Min.Y += GlobalMargin;
    Result.Max = V2Subtract(Result.Max, V2Set1(GlobalMargin));
    PushRenderCommandRectOutline(RenderGroup, Result, 1.0f,GlobalGroupBorder, 1.0f);
    Result.Min.Y += GlobalMargin;
    Result = Rectangle2AddRadiusTo(Result, -GlobalMargin);
    
    return Result;
}

internal rectangle2
TabControl(app_state *AppState, u32 Column, u32 Row, u32 *SelectedIndex, string *Labels, u32 LabelCount)
{
    rectangle2 Result = GridGetCellBounds(AppState, Column, Row);
    
    ui_frame *Frame = AppState->Frame;
    render_group *RenderGroup = Frame->RenderGroup;
    
    BeginGrid(AppState, Result, 1, 2);
    {
        GridSetRowHeight(AppState, 0, 20.0f);
        
        // NOTE(kstandbridge): Tab controls
        {
            rectangle2 Bounds = GridGetCellBounds(AppState, 0, 0);
            v2 At = Bounds.Min;
            
            for(u32 LabelIndex = 0;
                LabelIndex < LabelCount;
                ++LabelIndex)
            {
                string Text = Labels[LabelIndex];
                rectangle2 TextBounds = GetTextBounds(AppState, RenderGroup, Rectangle2(At, Bounds.Max), 
                                                      1.0f, 1.0f, V4(0.0f, 0.0f, 0.0f, 1.0f), Text);
                TextBounds.Max.X += GlobalMargin;
                if(LabelIndex == *SelectedIndex)
                {
                    rectangle2 ButtonBounds = Rectangle2(TextBounds.Min, V2(TextBounds.Max.X, Bounds.Max.Y));
                    PushRenderCommandRect(RenderGroup, ButtonBounds, 1.0f, 
                                          GlobalTabButtonSelected);
                    PushRenderCommandRect(RenderGroup, Rectangle2(V2(ButtonBounds.Min.X, ButtonBounds.Max.Y - 1.0f), ButtonBounds.Max), 3.0f, GlobalFormColor);
                }
                else
                {
                    rectangle2 ButtonBounds = Rectangle2(TextBounds.Min, V2(TextBounds.Max.X, Bounds.Max.Y));
                    ButtonBounds.Min.Y += 4.0f;
                    
                    ui_interaction Interaction =
                    {
                        .Id = GenerateUIId(Labels + LabelIndex),
                        .Type = UI_Interaction_ImmediateButton,
                        .Target = 0
                    };
                    
                    ui_interaction_state InteractionState = AddUIInteraction(AppState, ButtonBounds, Interaction);
                    switch(InteractionState)
                    {
                        case UIInteractionState_HotClicked:
                        {
                            PushRenderCommandRect(RenderGroup, ButtonBounds, 1.0f, 
                                                  GlobalTabButtonClicked);
                        } break;
                        case UIInteractionState_Hot:
                        {
                            PushRenderCommandRect(RenderGroup, ButtonBounds, 1.0f, 
                                                  GlobalTabButtonHot);
                        } break;
                        
                        default:
                        {
                            PushRenderCommandRect(RenderGroup, ButtonBounds, 1.0f, 
                                                  GlobalTabButtonBackground);
                        } break;
                    }
                    
                    PushRenderCommandRectOutline(RenderGroup, ButtonBounds, 2.0f, GlobalTabButtonBorder, 1.0f);
                    
                    if(InteractionsAreEqual(Interaction, AppState->ToExecute))
                    {
                        *SelectedIndex = LabelIndex;
                    }
                }
                At.X += GlobalMargin/2.0f;
                DrawTextAt(AppState, RenderGroup, Rectangle2(V2(At.X, At.Y + 2.0f), Bounds.Max), 
                           1.0f, 1.0f, V4(0.0f, 0.0f, 0.0f, 1.0f), Text);
                
                At.X = TextBounds.Max.X;
            }
        }
        
        // NOTE(kstandbridge): Tab content
        {
            Result = GridGetCellBounds(AppState, 0, 1);
            Result.Min.Y -= 1.0f;
            Result.Max = V2Subtract(Result.Max, V2Set1(GlobalMargin));
            PushRenderCommandRect(RenderGroup, Result, 1.0f, GlobalFormColor);
            PushRenderCommandRectOutline(RenderGroup, Result, 2.0f, GlobalTabButtonBorder, 1.0f);
            Result.Min.Y += 1.0f;
            Result.Min = V2Add(Result.Min, V2Set1(GlobalMargin));
        }
        
    }
    EndGrid(AppState);
    
    
    return Result;
}

void
InitApp(app_memory *AppMemory)
{
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    memory_arena *Arena = &AppState->Arena;
    
    // NOTE(kstandbridge): Loading a glyph sprite sheet
    {
        LogDebug("Loading a glyph sprite sheet");
        
        string FontData = PlatformReadEntireFile(Arena, String("C:\\Windows\\Fonts\\segoeui.ttf"));
        stbtt_InitFont(&AppState->FontInfo, FontData.Data, 0);
        
        f32 MaxFontHeightInPixels = 30.0f;
        AppState->FontScale = stbtt_ScaleForPixelHeight(&AppState->FontInfo, MaxFontHeightInPixels);
        stbtt_GetFontVMetrics(&AppState->FontInfo, &AppState->FontAscent, &AppState->FontDescent, &AppState->FontLineGap);
        
#if 0
        s32 Padding = (s32)(MaxFontHeightInPixels / 3.0f);
        u8 OnEdgeValue = (u8)(0.8f*255.0f);
        f32 PixelDistanceScale = (f32)OnEdgeValue/(f32)Padding;
#else
        s32 Padding = 4;
        u8 OnEdgeValue = 128;
        f32 PixelDistanceScale = 100.0f;
#endif
        
#if 1
        u32 FirstChar = 0;
        u32 LastChar = 256;
#else
        u32 FirstChar = 0x0400;
        u32 LastChar = FirstChar + 255;
#endif
        
        s32 MaxWidth = 0;
        s32 MaxHeight = 0;
        s32 TotalWidth = 0;
        s32 TotalHeight = 0;
        u32 ColumnAt = 0;
        u32 RowCount = 1;
        
        glyph_info *GlyphInfo = AppState->GlyphInfos;
        
        for(u32 CodePoint = FirstChar;
            CodePoint < LastChar;
            ++CodePoint)
        {                
            GlyphInfo->Data = stbtt_GetCodepointSDF(&AppState->FontInfo, AppState->FontScale, CodePoint, Padding, OnEdgeValue, PixelDistanceScale, 
                                                    &GlyphInfo->Width, &GlyphInfo->Height, 
                                                    &GlyphInfo->XOffset, &GlyphInfo->YOffset);
            stbtt_GetCodepointHMetrics(&AppState->FontInfo, CodePoint, &GlyphInfo->AdvanceWidth, &GlyphInfo->LeftSideBearing);
            
            GlyphInfo->CodePoint = CodePoint;
            
            if(GlyphInfo->Data)
            {
                TotalWidth += GlyphInfo->Width;
                ++ColumnAt;
                
                if(GlyphInfo->Height > MaxHeight)
                {
                    MaxHeight = GlyphInfo->Height;
                }
            }
            
            if((ColumnAt % 16) == 0)
            {
                ++RowCount;
                ColumnAt = 0;
                if(TotalWidth > MaxWidth)
                {
                    MaxWidth = TotalWidth;
                }
                TotalWidth = 0;
            }
            
            ++GlyphInfo;
        }
        
        TotalWidth = MaxWidth;
        TotalHeight = MaxHeight*RowCount;
        
        umm TextureSize = TotalWidth*TotalHeight*sizeof(u32);
        // TODO(kstandbridge): Temp memory here
        u32 *TextureBytes = PushSize(&AppState->Arena, TextureSize);
        
        u32 AtX = 0;
        u32 AtY = 0;
        
        ColumnAt = 0;
        
        for(u32 Index = 0;
            Index < ArrayCount(AppState->GlyphInfos);
            ++Index)
        {
            GlyphInfo = AppState->GlyphInfos + Index;
            
            GlyphInfo->UV = V4((f32)AtX / (f32)TotalWidth, (f32)AtY / (f32)TotalHeight,
                               ((f32)AtX + (f32)GlyphInfo->Width) / (f32)TotalWidth, 
                               ((f32)AtY + (f32)GlyphInfo->Height) / (f32)TotalHeight);
            
            for(s32 Y = 0;
                Y < GlyphInfo->Height;
                ++Y)
            {
                for(s32 X = 0;
                    X < GlyphInfo->Width;
                    ++X)
                {
                    u32 Alpha = (u32)GlyphInfo->Data[(Y*GlyphInfo->Width) + X];
                    TextureBytes[(Y + AtY)*TotalWidth + (X + AtX)] = 0x00FFFFFF | (u32)((Alpha) << 24);
                }
            }
            
            AtX += GlyphInfo->Width;
            
            ++ColumnAt;
            
            if((ColumnAt % 16) == 0)
            {
                AtY += MaxHeight;
                AtX = 0;
            }
            
            stbtt_FreeSDF(GlyphInfo->Data, 0);
        }
        
        DirectXLoadTexture(TotalWidth, TotalHeight, TextureBytes);
        
    }
    
}

void
AppUpdateFrame(app_memory *AppMemory, render_group *RenderGroup, app_input *Input, f32 DeltaTime)
{
    app_state *AppState = AppMemory->AppState;
    
#if 1
    {    
        u8 Buffer[4096];
        f32 FramesPerSecond = 1.0f / DeltaTime;
        string Thing = FormatStringToBuffer(Buffer, sizeof(Buffer), "%.3fms %.2ffps", DeltaTime, FramesPerSecond);
        rectangle2 Bounds = Rectangle2(V2(0, 0), V2(RenderGroup->Width, RenderGroup->Height));
        
        DrawTextAt(AppState, RenderGroup, Bounds, 10.0f, 1.0f, V4(1, 1, 1, 1), Thing);
    }
#endif
    
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
    // NOTE(kstandbridge): Text test
    {
        string LoremIpsum = String("Lorem Ipsum is simply dummy text of the printing and typesetting\nindustry. Lorem Ipsum has been the industry's standard dummy\ntext ever since the 1500s, when an unknown printer took a galley\nof type and scrambled it to make a type specimen book. It has\nsurvived not only five centuries, but also the leap into electronic\ntypesetting, remaining essentially unchanged. It was popularised in\nthe 1960s with the release of Letraset sheets containing Lorem\nIpsum passages, and more recently with desktop publishing\nsoftware like Aldus PageMaker including versions of Lorem Ipsum.");
        
        rectangle2 Bounds = Rectangle2(V2(100, 100), V2(RenderGroup->Width, RenderGroup->Height));
        
        DrawTextAt(AppState, RenderGroup, Bounds, 3.0f, 1.0f, V4(1, 1, 1, 1), LoremIpsum);
        //Bounds.Min = V2Add(Bounds.Min, V2(2, 2));
        //DrawTextAt(AppState, RenderGroup, Bounds, 2.0f, 1.0f, V4(0, 0, 0, 1), LoremIpsum);
    }
#endif
    
#if 1
    // NOTE(kstandbridge): Texture test
    
    {    
        v2 P = V2(500, 400);
        v2 Size = V2(81, 115);
        rectangle2 Bounds = Rectangle2(P, V2Add(P, Size));
        render_command *Command = PushRenderCommandRect(RenderGroup, Bounds, 3.0f, V4(1, 1, 1, 1));
        Command->Type = RenderCommand_Sprite;
        Command->UV = V4(0, 0, 1, 1);
    }
    
#endif
    
    temporary_memory MemoryFlush = BeginTemporaryMemory(&AppState->Arena);
    
    ui_frame _UiFrame = 
    {
        .Arena = MemoryFlush.Arena,
        .RenderGroup = RenderGroup,
        .Input = Input,
        .CurrentGrid = 0,
    };
    AppState->Frame = &_UiFrame;
    
    AppState->LastMouseP = AppState->MouseP;
    AppState->MouseP = Input->MouseP;
    AppState->dMouseP = V2Subtract(AppState->MouseP, AppState->LastMouseP);
    
    rectangle2 ScreenBounds = Rectangle2(V2Set1(0.0f), V2(RenderGroup->Width, RenderGroup->Height));
    ScreenBounds.Min = V2Add(ScreenBounds.Min, V2Set1(GlobalMargin));
    
#if 0    
    BeginGrid(AppState, ScreenBounds, 1, 2);
    {
        GridSetRowHeight(AppState, 1, 30.0f);
        
        string TabLabels[] =
        {
            String("General"),
            String("Sharing"),
            String("Security"),
            String("Previous Versions"),
            String("Customize"),
        };
        rectangle2 TabBounds = TabControl(AppState, 0, 0, (u32 *)&AppState->SelectedPropertyTab,
                                          TabLabels, ArrayCount(TabLabels));
        switch(AppState->SelectedPropertyTab)
        {
            case PropertiesTab_General:
            {
                BeginGrid(AppState, TabBounds, 1, 1);
                {
                    Button(AppState, 0, 0, true, String("This should be the general tab"));
                }
                EndGrid(AppState);
            } break;
            case PropertiesTab_Sharing:
            {
                BeginGrid(AppState, TabBounds, 1, 3);
                {
                    GridSetRowHeight(AppState, 0, 146.0f);
                    
                    BeginGrid(AppState, GroupControl(AppState, 0, 0, 
                                                     String("Network File and Folder Sharing")),
                              1, 2);
                    {
                        GridSetRowHeight(AppState, 0, 40.0f);
                        
                        BeginGrid(AppState, GridGetCellBounds(AppState, 0, 0), 2, 1);
                        {
                            GridSetColumnWidth(AppState, 0, 40.0f);
                            
                            Label(AppState, 1, 0, String("apps\nNot Shared"));
                        }
                        EndGrid(AppState);
                        
                        BeginGrid(AppState, GridGetCellBounds(AppState, 0, 1), 2, 2);
                        {
                            GridSetColumnWidth(AppState, 0, 80.0f);
                            GridSetRowHeight(AppState, 1, 30.0f);
                            
                            Label(AppState, 0, 0, String("Network Path:\nNot Shared"));
                            Button(AppState, 0, 1, true, String("Share..."));
                        }
                        EndGrid(AppState);
                        
                    }
                    EndGrid(AppState);
                    
                    BeginGrid(AppState, GroupControl(AppState, 0, 1, String("Advanced Sharing")), 1, 2);
                    {       
                        GridSetRowHeight(AppState, 1, 30.0f);
                        Label(AppState, 0, 0, String("Set custom permissions, create multiple shares, and set other advanced sharing options."));
                        
                        BeginGrid(AppState, GridGetCellBounds(AppState, 0, 1), 2, 1);
                        {
                            Button(AppState, 0, 0, true, String("Advanced Sharing..."));
                        }
                        EndGrid(AppState);
                    }
                    EndGrid(AppState);
                    
                    
                }
                EndGrid(AppState);
            } break;
            case PropertiesTab_Security:
            {
                BeginGrid(AppState, TabBounds, 1, 1);
                {
                    Button(AppState, 0, 0, true, String("Here we have the security tab"));
                }
                EndGrid(AppState);
            } break;
            case PropertiesTab_PreviousVersions:
            {
                BeginGrid(AppState, TabBounds, 1, 1);
                {
                    Button(AppState, 0, 0, true, String("Apparently files have version control"));
                }
                EndGrid(AppState);
            } break;
            case PropertiesTab_Customize:
            {
                BeginGrid(AppState, TabBounds, 1, 1);
                {
                    Button(AppState, 0, 0, true, String("Lets customize this"));
                }
                EndGrid(AppState);
            } break;
            
            InvalidDefaultCase;
        }
        
        BeginGrid(AppState, GridGetCellBounds(AppState, 0, 1), 4, 1);
        {
            //Label(AppState, 0, 0, String("Space"));
            
            if(Button(AppState, 1, 0, true, String("OK")))
            {
                LogInfo("OK");
            }
            if(Button(AppState, 2, 0, true, String("Cancel")))
            {
                LogInfo("Cancel");
            }
            if(Button(AppState, 3, 0, false, String("Apply")))
            {
                LogInfo("Apply");
            }
        }
        EndGrid(AppState);
        
    }
    EndGrid(AppState);
#endif
    
    AppState->ToExecute = AppState->NextToExecute;
    ClearInteraction(&AppState->NextToExecute);
    
    // NOTE(kstandbridge): Interact
    {
        u32 TransistionCount = Input->MouseButtons[MouseButton_Left].HalfTransitionCount;
        b32 MouseButton = Input->MouseButtons[MouseButton_Left].EndedDown;
        if(TransistionCount % 2)
        {
            MouseButton = !MouseButton;
        }
        
        for(u32 TransitionIndex = 0;
            TransitionIndex <= TransistionCount;
            ++TransitionIndex)
        {
            b32 MouseMove = false;
            b32 MouseDown = false;
            b32 MouseUp = false;
            if(TransitionIndex == 0)
            {
                MouseMove = true;
            }
            else
            {
                MouseDown = MouseButton;
                MouseUp = !MouseButton;
            }
            
            b32 EndInteraction = false;
            
            switch(AppState->Interaction.Type)
            {
                case UI_Interaction_ImmediateButton:
                {
                    if(MouseUp)
                    {
                        AppState->NextToExecute = AppState->Interaction;
                        EndInteraction = true;
                    }
                } break;
                
                case UI_Interaction_None:
                {
                    AppState->HotInteraction = AppState->NextHotInteraction;
                    if(MouseDown)
                    {
                        AppState->Interaction = AppState->HotInteraction;
                    }
                } break;
                
                default:
                {
                    if(MouseUp)
                    {
                        EndInteraction = true;
                    }
                } break;
            }
            
            if(MouseDown)
            {
                ClearInteraction(&AppState->SelectedInteration);
            }
            
            if(MouseUp)
            {
                AppState->SelectedInteration = AppState->Interaction;
            }
            
            if(EndInteraction)
            {
                ClearInteraction(&AppState->Interaction);
            }
            
            MouseButton = !MouseButton;
        }
    }
    
    ClearInteraction(&AppState->NextHotInteraction);
    
    
    
    EndTemporaryMemory(MemoryFlush);
}