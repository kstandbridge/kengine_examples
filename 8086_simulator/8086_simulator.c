#define KENGINE_WIN32
#define KENGINE_TEST
#define KENGINE_IMPLEMENTATION
#include "kengine.h"


inline string
GetRegisterName(u32 Index, b32 IsWord)
{
    string Bytes[] = { String("al"), String("cl"), String("dl"), String("bl"), String("ah"), String("ch"), String("dh"), String("bh") };
    string Words[] = { String("ax"), String("cx"), String("dx"), String("bx"), String("sp"), String("bp"), String("si"), String("di") };
    
    string Result = IsWord ? Words[Index] : Bytes[Index];
    
    return Result;
}

void
RunAllTests(memory_arena *Arena)
{
    string FileName = String("listing_38");
    PlatformConsoleOut("\n; %S\n", FileName);
    
    PlatformConsoleOut("\nbits 16\n\n");
    
    string Source = Win32ReadEntireFile(Arena, FileName);
    AssertTrue(Source.Data != 0);
    
    for(u32 Index = 0;
        Index < Source.Size;
        Index += 2)
    {
        u8 Op0 = Source.Data[Index];
        u8 Op1 = Source.Data[Index + 1];
        
#define MOV 0b100010
        
        if((Op0 >> 2) == MOV)
        {
            b32 IsWord = (Op0 & 1);
            u8 Dest = (Op1 >> 0) & ((1<<3)-1);
            u8 Src =  (Op1 >> 3) & ((1<<3)-1);
            
            PlatformConsoleOut("mov %S, %S\n", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
        }
        
        
        
    }
}
