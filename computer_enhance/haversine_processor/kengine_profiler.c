

internal u64
PlatformEstimateCPUTimerFrequency()
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


typedef struct timed_metric
{
    char *Label;
    u64 Begin;
    u64 End;
} timed_metric;
global u32 TimedMetricCount;
global timed_metric TimedMetrics[64];
global u64 GlobalBegin;

#define BEGIN_TIMED_FUNCTION() TimedMetrics[TimedMetricCount] = (timed_metric){ __FUNCTION__, PlatformReadCPUTimer(), 0}
#define END_TIMED_FUNCTION() TimedMetrics[TimedMetricCount++].End = PlatformReadCPUTimer()

internal void
BeginProfile()
{
    GlobalBegin = PlatformReadCPUTimer();
}

internal void
EndAndPrintProfile()
{
    u64 TotalElapsed = PlatformReadCPUTimer() - GlobalBegin;

    u64 CPUFrequency = PlatformEstimateCPUTimerFrequency();
    if(CPUFrequency)
    {
        PlatformConsoleOut("\nTotal time: %.4fms (CPU freq %lu)\n", (f64)TotalElapsed/(f64)CPUFrequency, CPUFrequency);
    }

    for(u32 MetricIndex = 0;
        MetricIndex < TimedMetricCount;
        ++MetricIndex)
    {
        timed_metric Metric = TimedMetrics[MetricIndex];
        u64 Elapsed = Metric.End - Metric.Begin;
        f64 Percent = 100.0f * ((f64)Elapsed / (f64)TotalElapsed);
        PlatformConsoleOut("\t%s\t%lu\t(%.2f%%)\n", Metric.Label, Elapsed, Percent);
    }

    PlatformConsoleOut("\n");
}
