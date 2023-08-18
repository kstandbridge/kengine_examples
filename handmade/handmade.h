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
    b32 IsConnected;
    b32 IsAnalog;
    f32 StickAverageX;
    f32 StickAverageY;

    union
    {
        button_state Buttons[12];

        struct
        {
            button_state MoveUp;
            button_state MoveDown;
            button_state MoveLeft;
            button_state MoveRight;

            button_state ActionUp;
            button_state ActionDown;
            button_state ActionLeft;
            button_state ActionRight;

            button_state LeftShoulder;
            button_state RightShoulder;

            button_state Back;
            button_state Start;
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
