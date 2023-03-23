#define KENGINE_WIN32
#define KENGINE_DIRECTX
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

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

typedef struct app_state
{
    memory_arena Arena;
    
    f32 FontScale;
    s32 FontAscent;
    s32 FontDescent;
    s32 FontLineGap;
    stbtt_fontinfo FontInfo;
    glyph_info GlyphInfos[256];
} app_state;

typedef enum text_op
{
    TextOp_Draw,
    TextOp_Size,
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
        
        f32 MaxFontHeightInPixels = 64.0f;
        AppState->FontScale = stbtt_ScaleForPixelHeight(&AppState->FontInfo, MaxFontHeightInPixels);
        stbtt_GetFontVMetrics(&AppState->FontInfo, &AppState->FontAscent, &AppState->FontDescent, &AppState->FontLineGap);
        
#if 1
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
AppUpdateFrame(app_memory *AppMemory, render_group *RenderGroup, f32 DeltaTime)
{
    app_state *AppState = AppMemory->AppState;
    
    {    
        u8 Buffer[4096];
        f32 FramesPerSecond = 1.0f / DeltaTime;
        string Thing = FormatStringToBuffer(Buffer, sizeof(Buffer), "%.3fms %.2ffps", DeltaTime, FramesPerSecond);
        rectangle2 Bounds = Rectangle2(V2(0, 0), V2(RenderGroup->Width, RenderGroup->Height));
        
        DrawTextAt(AppState, RenderGroup, Bounds, 10.0f, 0.5f, V4(1, 1, 1, 1), Thing);
    }
    
#if 1
    // NOTE(kstandbridge): Rect test
#define BOX_WIDTH 15
#define BOX_HEIGHT 15
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
        
        DrawTextAt(AppState, RenderGroup, Bounds, 3.0f, 0.5f, V4(1, 1, 1, 1), LoremIpsum);
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
}
