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

inline string
Parse(memory_arena *Arena, u8 Op0, u8 Op1)
{
    string Result;
    
#define MOV 0b100010
    
    if((Op0 >> 2) == MOV)
    {
        b32 IsWord = (Op0 & 1);
        u8 Dest = (Op1 >> 0) & ((1<<3)-1);
        u8 Src =  (Op1 >> 3) & ((1<<3)-1);
        
        Result = FormatString(Arena, "mov %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
    }
    else
    {
        Result = String("; Unknown Op code");
    }
    
    return Result;
}

inline void
RunListing38Tests(memory_arena *Arena)
{    
    {
        u8 Op0 = 0b10001001;
        u8 Op1 = 0b11011001;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov cx, bx"), Actual);
    }
    {
        u8 Op0 = 0b10001000;
        u8 Op1 = 0b11100101;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov ch, ah"), Actual);
    }
    {
        u8 Op0 = 0b10001001;
        u8 Op1 = 0b11011010;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov dx, bx"), Actual);
    }
    {
        u8 Op0 = 0b10001001;
        u8 Op1 = 0b11011110;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov si, bx"), Actual);
    }
    {
        u8 Op0 = 0b10001001;
        u8 Op1 = 0b11111011;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov bx, di"), Actual);
    }
    {
        u8 Op0 = 0b10001000;
        u8 Op1 = 0b11001000;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov al, cl"), Actual);
    }
    {
        u8 Op0 = 0b10001000;
        u8 Op1 = 0b11101101;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov ch, ch"), Actual);
    }
    {
        u8 Op0 = 0b10001001;
        u8 Op1 = 0b11000011;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov bx, ax"), Actual);
    }
    {
        u8 Op0 = 0b10001001;
        u8 Op1 = 0b11110011;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov bx, si"), Actual);
    }
    {
        u8 Op0 = 0b10001001;
        u8 Op1 = 0b11111100;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov sp, di"), Actual);
    }
    {
        u8 Op0 = 0b10001001;
        u8 Op1 = 0b11000101;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov bp, ax"), Actual);
    }
}

inline void
RunListing39Tests(memory_arena *Arena)
{
    {
        u8 Op0 = 0b10001001;
        u8 Op1 = 0b11011110;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov si, bx"), Actual);
    }
    
    {
        u8 Op0 = 0b10001000;
        u8 Op1 = 0b11000110;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov dh, al"), Actual);
    }
    
    {
        u8 Op0 = 0b10111001;
        u8 Op1 = 0b00001100;
        string Actual = Parse(Arena, Op0, Op1);
        AssertEqualString(String("mov cx, 12"), Actual);
    }
}

void
RunAllTests(memory_arena *Arena)
{
    RunListing38Tests(Arena);
    RunListing39Tests(Arena);
    
    
    string FileName = String("listing_39");
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
        
        string Line = Parse(Arena, Op0, Op1);
        PlatformConsoleOut("%S\n", Line);
    }
}
