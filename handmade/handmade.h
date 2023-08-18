typedef struct offscreen_buffer
{
    // NOTE(kstandbridge): Pixels are alwasy 32-bits wide, Memory Order BB GG RR XX
    void *Memory;
    s32 Width;
    s32 Height;
    s32 Pitch;
} offscreen_buffer;

typedef struct sound_output_buffer
{
    s32 SamplesPerSecond;
    s32 SampleCount;
    s16 *Samples;
} sound_output_buffer;

typedef struct button_state
{
    s32 HalfTransitionCount;
    b32 EndedDown;
} button_state;

typedef struct controller_input
{
    b32 IsAnalog;
    
    f32 StartX;
    f32 StartY;

    f32 MinX;
    f32 MinY;

    f32 MaxX;
    f32 MaxY;
    
    f32 EndX;
    f32 EndY;

    union
    {
        button_state Buttons[6];
        struct
        {
            button_state Up;
            button_state Down;
            button_state Left;
            button_state Right;
            button_state LeftShoulder;
            button_state RightShoulder;
        };
    };
} controller_input;

typedef struct app_input
{
    controller_input Controllers[4];
} app_input;

internal void 
AppUpdateAndRender(app_input *Input, offscreen_buffer *Buffer,
                   sound_output_buffer *SoundBuffer);
