typedef struct offscreen_buffer
{
    // NOTE(kstandbridge): Pixels are alwasy 32-bits wide, Memory Order BB GG RR XX
    void *Memory;
    s32 Width;
    s32 Height;
    s32 Pitch;
    s32 BytesPerPixel;
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

            button_state Terminator;
        };
    };
} controller_input;

typedef struct app_input
{
    controller_input Controllers[5];
} app_input;

inline controller_input *
GetController(app_input *Input, u32 ControllerIndex)
{
    Assert(ControllerIndex < ArrayCount(Input->Controllers));
    
    controller_input *Result = &Input->Controllers[ControllerIndex];


    return Result;
}

typedef struct app_state
{
    memory_arena Arena;

    s32 ToneHz;
    s32 GreenOffset;
    s32 BlueOffset;
} app_state;

internal void 
AppUpdateAndRender(app_memory *AppMemory, app_input *Input, offscreen_buffer *Buffer);

internal void
AppGetSoundSamples(app_memory *AppMemory, sound_output_buffer *SoundBuffer);