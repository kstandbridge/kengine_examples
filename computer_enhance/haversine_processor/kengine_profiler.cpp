
internal u64
EstimateCPUTimerFrequency()
{
    u64 Result = 0;

    u64 MillisecondsToWait = 100;
    u64 OSFrequency = PlatformGetOSTimerFrequency();

    u64 CPUStart = PlatformReadCPUTimer();
    u64 OSStart = PlatformReadOSTimer();
    u64 OSEnd = 0;
    u64 OSElapsed = 0;
    u64 OSWaitTime = OSFrequency * MillisecondsToWait / 1000;

    while(OSElapsed < OSWaitTime)
    {
        OSEnd = PlatformReadOSTimer();
        OSElapsed = OSEnd - OSStart;
    }

    u64 CPUEnd = PlatformReadCPUTimer();
    u64 CPUElapsed = CPUEnd - CPUStart;

    if(OSElapsed)
    {
        Result = OSFrequency * CPUElapsed / OSElapsed;
    }

    return Result;
}

typedef struct profile_anchor
{
    u64 TSCElapsed;
    u64 HitCount;
    char const *Label;
} profile_anchor;

typedef struct profiler
{
    profile_anchor Anchors[4096];

    u64 StartTSC;
    u64 EndTSC;
} profiler;
global profiler GlobalProfiler;

typedef struct profile_block
{
    profile_block(char const *Label_, u32 AnchorIndex_)
    {
        AnchorIndex = AnchorIndex_;
        Label = Label_;
        StartTSC = PlatformReadCPUTimer();
    }

    ~profile_block()
    {
        u64 Elapsed = PlatformReadCPUTimer() - StartTSC;

        profile_anchor *Anchor = GlobalProfiler.Anchors + AnchorIndex;
        Anchor->TSCElapsed += Elapsed;
        ++Anchor->HitCount;

        Anchor->Label = Label;
    }

    char const *Label;
    u64 StartTSC;
    u32 AnchorIndex;
} profile_block;

// TODO(kstandbridge): Relocate these?
#define NAME_CONCAT_(A, B) A##B
#define NAME_CONCAT(A, B) NAME_CONCAT_(A, B)

#define TIMED_BLOCK(Name) profile_block NAME_CONCAT(Block, __LINE__)(Name, __COUNTER__ + 1)
#define TIMED_FUNCTION() TIMED_BLOCK(__FUNCTION__)

internal void
BeginProfile()
{
    GlobalProfiler.StartTSC = PlatformReadCPUTimer();
}

internal void
EndAndPrintProfile()
{
    GlobalProfiler.EndTSC = PlatformReadCPUTimer();
    u64 CPUFrequency = EstimateCPUTimerFrequency();

    u64 TotalCPUElapsed = GlobalProfiler.EndTSC - GlobalProfiler.StartTSC;

    if(CPUFrequency)
    {
        PlatformConsoleOut("\nTotal time: %.4fms (CPU freq %lu)\n", (f64)TotalCPUElapsed / (f64)CPUFrequency, CPUFrequency);
    }

    for(u32 AnchorIndex = 0;
        AnchorIndex < ArrayCount(GlobalProfiler.Anchors);
        ++AnchorIndex)
    {
        profile_anchor *Anchor = GlobalProfiler.Anchors + AnchorIndex;
        if(Anchor->TSCElapsed)
        {
            u64 Elapsed = Anchor->TSCElapsed;
            f64 Percent = 100.0f * ((f64)Elapsed / (f64)TotalCPUElapsed);
            PlatformConsoleOut("%s[%lu]: %lu (%.2f%%)\n", Anchor->Label, Anchor->HitCount, Elapsed, Percent);
        }
    }

    PlatformConsoleOut("\n");
}
