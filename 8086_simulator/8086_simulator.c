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
Parse(memory_arena *Arena, u8 *At, umm Size)
{
    format_string_state State = BeginFormatString();
    for(umm Index = 0;
        Index < Size;)
    {
        u8 Op0 = At[Index++];
        u8 Op1 = At[Index++];
        
        b32 RegisterMemoryToFromRegister = ((Op0 >> 2) == 0b100010);
        b32 ImmediateToRegisterMemory = ((Op0 >> 1) == 0b1100011);
        b32 ImmediateToRegister = ((Op0 >> 4) == 0b1011);
        b32 MemoryToAccumulator = ((Op0 >> 1) == 0b1010000);
        b32 AccumlatorToMemory = ((Op0 >> 1) == 0b1010001);
        if(MemoryToAccumulator)
        {
            // NOTE(kstandbridge): MOV Memory to accumulator
            
            u8 ValueLow = Op1;
            u8 ValueHigh = At[Index++];
            u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
            
            AppendFormatString(&State, "mov ax, [%u]", ValueWide);
            
        }
        else if(AccumlatorToMemory)
        {
            // NOTE(kstandbridge): MOV Accumulator to Memory
            
            u8 ValueLow = Op1;
            u8 ValueHigh = At[Index++];
            u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
            
            AppendFormatString(&State, "mov [%u], ax", ValueWide);
            
        }
        else if(RegisterMemoryToFromRegister || 
                ImmediateToRegisterMemory)
        {
            // NOTE(kstandbridge): MOV Register/memory to/from register
            
            
            b32 IsWord = (Op0 & 1);
            
            // NOTE(kstandbridge): 
            b32 Direction = RegisterMemoryToFromRegister ? ((Op0 >> 1) & 1) : 0;
            
            u8 Mod = Op1 >> 6;
            if(Mod == 0b00)
            {
                // NOTE(kstandbridge): No displacement follows*
                
                string Bytes[] = 
                { 
                    String("[bx + si]"), String("[bx + di]"), String("[bp + si]"), String("[bp + di]"), String("[si]"), String("[di]"), String("DIRECT ADDRESS"), String("[bx]") 
                };
                
                u8 Dest = (Op1 >> 0) & ((1<<3)-1);
                u8 Src  = (Op1 >> 3) & ((1<<3)-1);
                
                if(Dest == 0b110)
                {
                    // NOTE(kstandbridge): DIRECT ADDRESS
                    if(IsWord)
                    {
                        u8 ValueLow = At[Index++];
                        u8 ValueHigh = At[Index++];
                        u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                        
                        s16 Value = *(u16 *)&ValueWide;
                        
                        AppendFormatString(&State, "mov %S, [%d]", GetRegisterName(Src, IsWord), Value);
                    }
                    else
                    {
                        u8 Op2 = At[Index++];
                        s8 Value = *(u8 *)&Op2;
                        
                        AppendFormatString(&State, "mov %S, [%d]", GetRegisterName(Src, IsWord), Value);
                    }
                    
                    
                }
                else if(RegisterMemoryToFromRegister)
                {                
                    if(Direction)
                    {
                        AppendFormatString(&State, "mov %S, %S", GetRegisterName(Dest, IsWord), Bytes[Src]);
                    }
                    else
                    {
                        AppendFormatString(&State, "mov %S, %S", Bytes[Dest], GetRegisterName(Src, IsWord));
                    }
                }
                else
                {
                    Assert(ImmediateToRegisterMemory);
                    
                    if(IsWord)
                    {
                        u8 ValueLow = At[Index++];
                        u8 ValueHigh = At[Index++];
                        u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                        
                        s16 Value = *(u16 *)&ValueWide;
                        
                        AppendFormatString(&State, "mov %S, word %d", Bytes[Dest], Value);
                    }
                    else
                    {
                        u8 Op2 = At[Index++];
                        s8 Value = *(u8 *)&Op2;
                        AppendFormatString(&State, "mov %S, byte %d", Bytes[Dest], Value);
                    }
                }
                
            }
            else if(Mod == 0b01)
            {
                // NOTE(kstandbridge): 8-bit displacement
                u8 Dest = (Op1 >> 0) & ((1<<3)-1);
                u8 Src =  (Op1 >> 3) & ((1<<3)-1);
                u8 Op2 = At[Index++];
                
                s8 Value = *(u8 *)&Op2;
                
                string Bytes[] = 
                { 
                    String("bx + si"), String("bx + di"), String("bp + si"), String("bp + di"), String("si"), String("di"), String("bp"), String("bx") 
                };
                
                if(Direction)
                {
                    AppendFormatString(&State, "mov %S, [%S", GetRegisterName(Src, IsWord), Bytes[Dest]);
                    
                    if(Value > 0)
                    {
                        AppendFormatString(&State, " + %d]", Value);
                    }
                    else if(Value < 0)
                    {
                        Value *= -1;
                        AppendFormatString(&State, " - %d]", Value);
                    }
                    else
                    {
                        AppendFormatString(&State, "]");
                    }
                }
                else
                {
                    AppendFormatString(&State, "mov [%S", Bytes[Dest]);
                    
                    if(Value > 0)
                    {
                        AppendFormatString(&State, " + %d]", Value);
                    }
                    else if(Value < 0)
                    {
                        Value *= -1;
                        AppendFormatString(&State, " - %d]", Value);
                    }
                    else
                    {
                        AppendFormatString(&State, "]");
                    }
                    AppendFormatString(&State, ", %S", GetRegisterName(Src, IsWord), Bytes[Dest]);
                }
            }
            else if(Mod == 0b10)
            {
                // NOTE(kstandbridge): 16-bit displacement
                u8 Dest = (Op1 >> 0) & ((1<<3)-1);
                u8 Src =  (Op1 >> 3) & ((1<<3)-1);
                u8 ValueLow = At[Index++];
                u8 ValueHigh = At[Index++];
                u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                
                s16 Value = *(u16 *)&ValueWide;
                
                string Bytes[] = 
                { 
                    String("bx + si"), String("bx + di"), String("bp + si"), String("bp + di"), String("si"), String("di"), String("bp"), String("bx") 
                };
                
                if(Direction)
                {
                    AppendFormatString(&State, "mov %S, [%S", GetRegisterName(Src, IsWord), Bytes[Dest]);
                    
                    if(Value > 0)
                    {
                        AppendFormatString(&State, " + %d]", Value);
                    }
                    else if(Value < 0)
                    {
                        Value *= -1;
                        AppendFormatString(&State, " - %d]", Value);
                    }
                    else
                    {
                        AppendFormatString(&State, "]");
                    }
                }
                else
                {
                    AppendFormatString(&State, "mov [%S", Bytes[Dest]);
                    
                    if(Value > 0)
                    {
                        AppendFormatString(&State, " + %d]", Value);
                    }
                    else if(Value < 0)
                    {
                        Value *= -1;
                        AppendFormatString(&State, " - %d]", Value);
                    }
                    else
                    {
                        AppendFormatString(&State, "]");
                    }
                    if(RegisterMemoryToFromRegister)
                    {
                        AppendFormatString(&State, ", %S", GetRegisterName(Src, IsWord));
                    }
                    else if(ImmediateToRegisterMemory)
                    {
                        if(IsWord)
                        {
                            ValueLow = At[Index++];
                            ValueHigh = At[Index++];
                            ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                            
                            Value = *(u16 *)&ValueWide;
                            
                            AppendFormatString(&State, ", word %d", Value);
                        }
                        else
                        {
                            AppendFormatString(&State, ", byte TODO");
                        }
                    }
                }
            }
            else if(Mod == 0b11)
            {
                // NOTE(kstandbridge): Register mode (no displacement)
                u8 Dest = (Op1 >> 0) & ((1<<3)-1);
                
                u8 Src =  (Op1 >> 3) & ((1<<3)-1);
                AppendFormatString(&State, "mov %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
            }
            else
            {
                AppendFormatString(&State, "; Unknown mod %b from Op code %b", Mod, Op0);
            }
            
        }
        else if(ImmediateToRegister)
        {
            // NOTE(kstandbridge): MOV Immediate to register
            u8 Wide = (Op0 >> 4) & ((1 << 1)-1);
            u8 Reg = (Op0 & 0b111);
            u8 Op2 = At[Index++];
            u16 Value = ((Op2 & 0xFF) << 8) | (Op1 & 0xFF);
            
            AppendFormatString(&State, "mov %S, %u", GetRegisterName(Reg, Wide), Value);
            
        }
        else
        {
            AppendFormatString(&State, "; Unknown Op code %b", Op0);
        }
        
        AppendFormatString(&State,"\n" );
    }
    
    string Result = EndFormatString(&State, Arena);
    
    return Result;
}

inline void
RunListing38Tests(memory_arena *Arena)
{    
    {
        u8 Op[] = { 0b10001001, 0b11011001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov cx, bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001000, 0b11100101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov ch, ah\n"), Actual);
    }
    {
        u8 Op[] = { 0b10001001, 0b11011010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov dx, bx\n"), Actual);
    }
    {
        u8 Op[] = { 0b10001001, 0b11011110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov si, bx\n"), Actual);
    }
    {
        u8 Op[] = { 0b10001001, 0b11111011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov bx, di\n"), Actual);
    }
    {
        u8 Op[] = { 0b10001000, 0b11001000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov al, cl\n"), Actual);
    }
    {
        u8 Op[] = { 0b10001000, 0b11101101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov ch, ch\n"), Actual);
    }
    {
        u8 Op[] = { 0b10001001, 0b11000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov bx, ax\n"), Actual);
    }
    {
        u8 Op[] = { 0b10001001, 0b11110011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov bx, si\n"), Actual);
    }
    {
        u8 Op[] = { 0b10001001, 0b11111100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov sp, di\n"), Actual);
    }
    {
        u8 Op[] = { 0b10001001, 0b11000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov bp, ax\n"), Actual);
    }
}

inline void
RunListing39Tests(memory_arena *Arena)
{
    {
        u8 Op[] = { 0b10001001, 0b11011110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov si, bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001000, 0b11000110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov dh, al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10111001, 0b00001100, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov cx, 12\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10111010, 0b01101100, 0b00001111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov dx, 3948\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001010, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov al, [bx + si]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001011, 0b00011011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov bx, [bp + di]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001011, 0b01010110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov dx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001010, 0b01100000, 0b00000100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov ah, [bx + si + 4]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001010, 0b10000000, 0b10000111, 0b00010011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov al, [bx + si + 4999]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001001, 0b00001001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov [bx + di], cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001000, 0b00001010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov [bp + si], cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001000, 0b01101110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov [bp], ch\n"), Actual);
    }
    
}

inline void
RunListing40Tests(memory_arena *Arena)
{
    
    {
        u8 Op[] = { 0b10001011, 0b01000001, 0b00100101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov ax, [bx + di + 37]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001011, 0b01000001, 0b11011011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov ax, [bx + di - 37]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001001, 0b10001100, 0b00101100, 0b00000001};
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov [si + 300], cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001001, 0b10001100, 0b11010100, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov [si - 300], cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001011, 0b01010111, 0b11100000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov dx, [bx - 32]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000110, 0b00000011, 0b00000111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov [bp + di], byte 7\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000111, 0b10000101, 0b10000101, 0b00000011, 0b01011011, 0b00000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov [di + 901], word 347\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001011, 0b00101110, 0b00000101, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov bp, [5]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001011, 0b00011110, 0b10000010, 0b00001101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov bx, [3458]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10100001, 0b11111011, 0b00001001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov ax, [2555]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10100001, 0b00010000, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov ax, [16]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10100011, 0b11111010, 0b00001001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov [2554], ax\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10100011, 0b00001111, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov [15], ax\n"), Actual);
    }
    
}

void
RunAllTests(memory_arena *Arena)
{
    RunListing38Tests(Arena);
    RunListing39Tests(Arena);
    RunListing40Tests(Arena);
    
    string FileName = String("listing_40");
    PlatformConsoleOut("\n; %S\n", FileName);
    
    PlatformConsoleOut("\nbits 16\n\n");
    
    string Source = Win32ReadEntireFile(Arena, FileName);
    AssertTrue(Source.Data != 0);
    
    string Output = Parse(Arena, Source.Data, Source.Size);
    PlatformConsoleOut("%S\n", Output);
}
