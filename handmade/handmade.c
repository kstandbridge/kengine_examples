internal void
GameOutputSound(sound_output_buffer *SoundBuffer, s32 ToneHz)
{
    local_persist f32 tSine;
    s16 ToneVolume = 3000;
    s32 WavePeriod = SoundBuffer->SamplesPerSecond/ToneHz;

    s16 *SampleOut = SoundBuffer->Samples;
    for(int SampleIndex = 0;
        SampleIndex < SoundBuffer->SampleCount;
        ++SampleIndex)
    {
        f32 SineValue = sinf(tSine);
        s16 SampleValue = (s16)(SineValue * ToneVolume);
        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;

        tSine += 2.0f*Pi32*1.0f/(f32)WavePeriod;
    }
}

internal void
RenderWeirdGradient(offscreen_buffer *Buffer, s32 BlueOffset, s32 GreenOffset)
{
    u8 *Row = (u8 *)Buffer->Memory;    
    for(s32 Y = 0;
        Y < Buffer->Height;
        ++Y)
    {
        u32 *Pixel = (u32 *)Row;
        for(s32 X = 0;
            X < Buffer->Width;
            ++X)
        {
            u8 Blue = (X + BlueOffset);
            u8 Green = (Y + GreenOffset);

            *Pixel++ = ((Green << 8) | Blue);
        }
        
        Row += Buffer->Pitch;
    }
}

internal void 
AppUpdateAndRender(app_input *Input, offscreen_buffer *Buffer,
                   sound_output_buffer *SoundBuffer)
{
    local_persist s32 BlueOffset = 0;
    local_persist s32 GreenOffset = 0;
    local_persist s32 ToneHz = 256;

    controller_input *Input0 = &Input->Controllers[0];    
    if(Input0->IsAnalog)
    {
        // NOTE(kstandbridge): Use analog movement tuning
        BlueOffset += (s32)4.0f*(Input0->EndX);
        ToneHz = 256 + (s32)(128.0f*(Input0->EndY));
    }
    else
    {
        // NOTE(kstandbridge): Use digital movement tuning
    }

    // Input.AButtonEndedDown;
    // Input.AButtonHalfTransitionCount;
    if(Input0->Down.EndedDown)
    {
        GreenOffset += 1;
    }
    
    // TODO(kstandbridge): Allow sample offsets here for more robust platform options
    GameOutputSound(SoundBuffer, ToneHz);
    RenderWeirdGradient(Buffer, BlueOffset, GreenOffset);
}
