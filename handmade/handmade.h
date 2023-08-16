typedef struct offscreen_buffer
{
    // NOTE(kstandbridge): Pixels are alwasy 32-bits wide, Memory Order BB GG RR XX
    void *Memory;
    int Width;
    int Height;
    int Pitch;
} offscreen_buffer;

internal void 
GameUpdateAndRender(offscreen_buffer *Buffer, int BlueOffset, int GreenOffset);
