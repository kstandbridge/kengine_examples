internal void
AppOutputSound(sound_output_buffer *SoundBuffer, s32 ToneHz)
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
        if(tSine > 2.0f*Pi32)
        {
            tSine -= 2.0f*Pi32;
        }
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
            u8 Blue = (u8)(X + BlueOffset);
            u8 Green = (u8)(Y + GreenOffset);

            *Pixel++ = ((Green << 8) | Blue);
        }
        
        Row += Buffer->Pitch;
    }
}

internal void 
AppUpdateAndRender(app_memory *AppMemory, app_input *Input, offscreen_buffer *Buffer)
{
    Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) ==
        (ArrayCount(Input->Controllers[0].Buttons)));

    app_state *AppState = AppMemory->AppState;
    if(AppState == 0)
    {
        AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);

        AppState->ToneHz = 512;

        string FilePath = String(__FILE__);
        string File = PlatformReadEntireFile(&AppState->Arena, FilePath);
        if(File.Data)
        {
            PlatformWriteTextToFile(File, String("test.out"));
        }
    }

    for(int ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ++ControllerIndex)
    {
        controller_input *Controller = GetController(Input, ControllerIndex);
        if(Controller->IsAnalog)
        {
            // NOTE(kstandbridge): Use analog movement tuning
            AppState->BlueOffset += (s32)4.0f*(Controller->StickAverageX);
            AppState->ToneHz = 512 + (s32)(128.0f*(Controller->StickAverageY));
        }
        else
        {
            // NOTE(kstandbridge): Use digital movement tuning
            if(Controller->MoveLeft.EndedDown)
            {
                AppState->BlueOffset -= 1;
            }
            
            if(Controller->MoveRight.EndedDown)
            {
                AppState->BlueOffset += 1;
            }
        }

        // Input.AButtonEndedDown;
        // Input.AButtonHalfTransitionCount;
        if(Controller->ActionDown.EndedDown)
        {
            AppState->GreenOffset += 1;
        }
    }

    RenderWeirdGradient(Buffer, AppState->BlueOffset, AppState->GreenOffset);
}

internal void
AppGetSoundSamples(app_memory *AppMemory, sound_output_buffer *SoundBuffer)
{
    app_state *AppState = AppMemory->AppState;
    if(AppState)
    {
        AppOutputSound(SoundBuffer, AppState->ToneHz);
    }
    else
    {
        PlatformConsoleOut("WARN: SoundSamples requested before app state initialized!\n");
    }
}