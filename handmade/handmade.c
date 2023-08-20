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
AppUpdateAndRender(app_memory *AppMemory, app_input *Input, offscreen_buffer *Buffer,
                   sound_output_buffer *SoundBuffer)
{
    app_state *AppState = AppMemory->AppState;
    if(AppState == 0)
    {
        AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);

        AppState->ToneHz = 256;
    }

    controller_input *Input1 = &Input->Controllers[1];    
    if(Input1->IsAnalog)
    {
        // NOTE(kstandbridge): Use analog movement tuning
        AppState->BlueOffset += (s32)4.0f*(Input1->StickAverageX);
        AppState->ToneHz = 256 + (s32)(128.0f*(Input1->StickAverageY));
    }
    else
    {
        // NOTE(kstandbridge): Use digital movement tuning
    }

    // Input.AButtonEndedDown;
    // Input.AButtonHalfTransitionCount;
    if(Input1->ActionDown.EndedDown)
    {
        AppState->GreenOffset += 1;
    }
    
    // TODO(kstandbridge): Allow sample offsets here for more robust platform options
    GameOutputSound(SoundBuffer, AppState->ToneHz);
    RenderWeirdGradient(Buffer, AppState->BlueOffset, AppState->GreenOffset);
}
