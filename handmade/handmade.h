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

internal void 
GameUpdateAndRender(offscreen_buffer *Buffer, s32 BlueOffset, s32 GreenOffset,
                    sound_output_buffer *SoundBuffer, s32 ToneHz);
