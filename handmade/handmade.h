typedef struct app_state
{
    memory_arena Arena;

    s32 ToneHz;
    s32 GreenOffset;
    s32 BlueOffset;

    f32 tSine;

    s32 PlayerX;
    s32 PlayerY;
    f32 tJump;
    
} app_state;