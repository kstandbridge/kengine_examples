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
        
        string Label = String("label");
        if(Op0 == 0b1110100)
        {
            AppendFormatString(&State, "je %S", Label);
        }
        else if(Op0 == 0b01111100)
        {
            AppendFormatString(&State, "jl %S", Label);
        }
        else if(Op0 == 0b01111110)
        {
            AppendFormatString(&State, "jle %S", Label);
        }
        else if(Op0 == 0b01110010)
        {
            AppendFormatString(&State, "jb %S", Label);
        }
        else if(Op0 == 0b01110110)
        {
            AppendFormatString(&State, "jbe %S", Label);
        }
        else if(Op0 == 0b01111010)
        {
            AppendFormatString(&State, "jp %S", Label);
        }
        else if(Op0 == 0b01110000)
        {
            AppendFormatString(&State, "jo %S", Label);
        }
        else if(Op0 == 0b01111000)
        {
            AppendFormatString(&State, "js %S", Label);
        }
        else if(Op0 == 0b01110101)
        {
            AppendFormatString(&State, "jne %S", Label);
        }
        else if(Op0 == 0b01111101)
        {
            AppendFormatString(&State, "jnl %S", Label);
        }
        else if(Op0 == 0b01111111)
        {
            AppendFormatString(&State, "jg %S", Label);
        }
        else if(Op0 == 0b01110011)
        {
            AppendFormatString(&State, "jnb %S", Label);
        }
        else if(Op0 == 0b01110111)
        {
            AppendFormatString(&State, "ja %S", Label);
        }
        else if(Op0 == 0b01111011)
        {
            AppendFormatString(&State, "jnp %S", Label);
        }
        else if(Op0 == 0b01110001)
        {
            AppendFormatString(&State, "jno %S", Label);
        }
        else if(Op0 == 0b01111001)
        {
            AppendFormatString(&State, "jns %S", Label);
        }
        else if(Op0 == 0b11100010)
        {
            AppendFormatString(&State, "loop %S", Label);
        }
        else if(Op0 == 0b11100001)
        {
            AppendFormatString(&State, "loopz %S", Label);
        }
        else if(Op0 == 0b11100000)
        {
            AppendFormatString(&State, "loopnz %S", Label);
        }
        else if(Op0 == 0b11100011)
        {
            AppendFormatString(&State, "jcxz %S", Label);
        }
        else
        {
            
            b32 MovRegisterMemoryToFromRegister =  ((Op0 >> 2) == 0b100010);
            b32 AddRegisterMemoryToFromRegister =  ((Op0 >> 2) == 0b000000);
            b32 SubRegisterMemoryToFromRegister =  ((Op0 >> 2) == 0b001010);
            b32 CmpRegisterMemoryAndFromRegister = ((Op0 >> 2) == 0b001110);
            b32 AddImmediateToAccumulator =        ((Op0 >> 2) == 0b000001);
            b32 CmpImmediateWithAccumulator =      ((Op0 >> 1) == 0b0011110);
            b32 SubImmediateFromAccumulator =      ((Op0 >> 1) == 0b0010110);
            b32 MovImmediateToRegisterMemory =     ((Op0 >> 1) == 0b1100011);
            b32 AddImmediateToRegisterMemory =     ((Op0 >> 2) == 0b100000);
            b32 MovImmediateToRegister =           ((Op0 >> 4) == 0b1011);
            b32 MovMemoryToAccumulator =           ((Op0 >> 1) == 0b1010000);
            b32 MovAccumlatorToMemory =            ((Op0 >> 1) == 0b1010001);
            
            if(AddImmediateToAccumulator ||
               CmpImmediateWithAccumulator ||
               SubImmediateFromAccumulator ||
               MovMemoryToAccumulator ||
               MovAccumlatorToMemory)
            {
                b32 IsWord = (Op0 & 1);
                
                s16 Value;
                
                if(IsWord)
                {
                    u8 ValueLow = Op1;
                    u8 ValueHigh = At[Index++];
                    u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                    
                    Value = *(s16 *)&ValueWide;
                }
                else
                {
                    Value = *(s8 *)&Op1;
                }
                
                if(AddImmediateToAccumulator)
                {
                    if(IsWord)
                    {
                        AppendFormatString(&State, "add ax, %d", Value);
                    }
                    else
                    {
                        AppendFormatString(&State, "add al, %d", Value);
                    }
                }
                else if (SubImmediateFromAccumulator)
                {
                    if(IsWord)
                    {
                        AppendFormatString(&State, "sub ax, %d", Value);
                    }
                    else
                    {
                        AppendFormatString(&State, "sub al, %d", Value);
                    }
                }
                else if(CmpImmediateWithAccumulator)
                {
                    AppendFormatString(&State, "cmp ax, %d", Value);
                }
                else if(MovAccumlatorToMemory)
                {
                    AppendFormatString(&State, "mov [%d], ax", Value);
                }
                else if(MovMemoryToAccumulator)
                {
                    AppendFormatString(&State, "mov ax, [%d]", Value);
                }
                else
                {
                    AppendFormatString(&State, "TODO [%d], ax", Value);
                }
                
            }
            else if(MovRegisterMemoryToFromRegister || 
                    AddRegisterMemoryToFromRegister ||
                    SubRegisterMemoryToFromRegister ||
                    CmpRegisterMemoryAndFromRegister ||
                    MovImmediateToRegisterMemory ||
                    AddImmediateToRegisterMemory)
            {
                b32 IsWord = (Op0 & 1);
                
                // NOTE(kstandbridge): 
                b32 Direction = (AddRegisterMemoryToFromRegister || 
                                 MovRegisterMemoryToFromRegister ||
                                 SubRegisterMemoryToFromRegister ||
                                 CmpRegisterMemoryAndFromRegister) ? ((Op0 >> 1) & 1) : 0;
                
                u8 Mod = Op1 >> 6;
                
                b32 NoDisplacement = (Mod == 0b00);
                b32 Displace8Bit = (Mod == 0b01);
                b32 Displace16Bit = (Mod == 0b10);
                b32 RegisterMode = (Mod == 0b11);
                
                if(RegisterMode)
                {
                    u8 Dest = (Op1 >> 0) & ((1<<3)-1);
                    u8 Src =  (Op1 >> 3) & ((1<<3)-1);
                    if(AddImmediateToRegisterMemory)
                    {                    
                        u8 Op2 = At[Index++];
                        s8 Value = *(u8 *)&Op2;
                        
                        if(Src == 0b000)
                        {
                            AppendFormatString(&State, "add %S, %d", GetRegisterName(Dest, IsWord), Value);
                        }
                        else if(Src == 0b101)
                        {
                            AppendFormatString(&State, "sub %S, %d", GetRegisterName(Dest, IsWord), Value);
                        }
                        else if(Src == 0b111)
                        {
                            AppendFormatString(&State, "cmp %S, %d", GetRegisterName(Dest, IsWord), Value);
                        }
                        else
                        {
                            AppendFormatString(&State, "TODO");
                        }
                        
                    }
                    else if(AddRegisterMemoryToFromRegister)
                    {
                        AppendFormatString(&State, "add %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                    }
                    else if(SubRegisterMemoryToFromRegister)
                    {
                        AppendFormatString(&State, "sub %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                    }
                    else if(CmpRegisterMemoryAndFromRegister)
                    {
                        AppendFormatString(&State, "cmp %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                    }
                    else
                    {
                        AppendFormatString(&State, "mov %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                    }
                }
                else if(NoDisplacement)
                {
                    string Bytes[] = 
                    { 
                        String("[bx + si]"), String("[bx + di]"), String("[bp + si]"), String("[bp + di]"), String("[si]"), String("[di]"), String("DIRECT ADDRESS"), String("[bx]") 
                    };
                    
                    u8 Dest = (Op1 >> 0) & ((1<<3)-1);
                    u8 Src  = (Op1 >> 3) & ((1<<3)-1);
                    b32 DirectAccess = (Dest == 0b110);
                    if(DirectAccess)
                    {
                        if(IsWord)
                        {
                            u8 ValueLow = At[Index++];
                            u8 ValueHigh = At[Index++];
                            u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                            
                            s16 Value = *(u16 *)&ValueWide;
                            
                            if(Src == 0b111)
                            {
                                s8 Value2 = *(s8 *)&At[Index++];
                                AppendFormatString(&State, "cmp word [%d], %d", Value, Value2);
                            }
                            else
                            {
                                AppendFormatString(&State, "mov %S, [%d]", GetRegisterName(Src, IsWord), Value);
                            }
                        }
                        else
                        {
                            u8 Op2 = At[Index++];
                            s8 Value = *(u8 *)&Op2;
                            
                            AppendFormatString(&State, "mov %S, [%d]", GetRegisterName(Src, IsWord), Value);
                        }
                    }
                    else if(MovRegisterMemoryToFromRegister)
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
                    else if(AddRegisterMemoryToFromRegister)
                    {                
                        if(Direction)
                        {
                            AppendFormatString(&State, "add %S, %S", GetRegisterName(Src, IsWord), Bytes[Dest]);
                        }
                        else
                        {
                            AppendFormatString(&State, "add %S, %S", Bytes[Dest], GetRegisterName(Src, IsWord));
                        }
                    }
                    else if(SubRegisterMemoryToFromRegister)
                    {                
                        if(Direction)
                        {
                            AppendFormatString(&State, "sub %S, %S", GetRegisterName(Src, IsWord), Bytes[Dest]);
                        }
                        else
                        {
                            AppendFormatString(&State, "sub %S, %S", Bytes[Dest], GetRegisterName(Src, IsWord));
                        }
                    }
                    else if(CmpRegisterMemoryAndFromRegister)
                    {                
                        if(Direction)
                        {
                            AppendFormatString(&State, "cmp %S, %S", GetRegisterName(Src, IsWord), Bytes[Dest]);
                        }
                        else
                        {
                            AppendFormatString(&State, "cmp %S, %S", Bytes[Dest], GetRegisterName(Src, IsWord));
                        }
                    }
                    else
                    {
                        if(IsWord)
                        {
                            if(MovImmediateToRegisterMemory)
                            {
                                u8 ValueLow = At[Index++];
                                u8 ValueHigh = At[Index++];
                                u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                                
                                s16 Value = *(u16 *)&ValueWide;
                                AppendFormatString(&State, "mov %S, word %d", Bytes[Dest], Value);
                            }
                            else if(AddImmediateToRegisterMemory)
                            {
                                s8 Value = *(u8 *)&At[Index++];
                                if(Src == 0b000)
                                {
                                    AppendFormatString(&State, "add word %S, %d", Bytes[Dest], Value);
                                }
                                else if(Src == 0b101)
                                {
                                    AppendFormatString(&State, "sub word %S, %d", Bytes[Dest], Value);
                                }
                                else
                                {
                                    AppendFormatString(&State, "TODO %S, word %d", Bytes[Dest], Value);
                                }
                            }
                            else
                            {
                                AppendFormatString(&State, "TODO 42, word 42");
                            }
                        }
                        else
                        {
                            u8 Op2 = At[Index++];
                            s8 Value = *(u8 *)&Op2;
                            if(MovImmediateToRegisterMemory)
                            {
                                AppendFormatString(&State, "mov %S, byte %d", Bytes[Dest], Value);
                            }
                            else if(AddImmediateToRegisterMemory)
                            {
                                if(Src == 0b000)
                                {
                                    AppendFormatString(&State, "add byte %S, %d", Bytes[Dest], Value);
                                }
                                else if(Src == 0b101)
                                {
                                    AppendFormatString(&State, "sub byte %S, %d", Bytes[Dest], Value);
                                }
                                else if(Src == 0b111)
                                {
                                    AppendFormatString(&State, "cmp byte %S, %d", Bytes[Dest], Value);
                                }
                                else
                                {
                                    AppendFormatString(&State, "TODO");
                                }
                            }
                            else
                            {
                                AppendFormatString(&State, "TODO %S, byte %d", Bytes[Dest], Value);
                            }
                        }
                    }
                    
                }
                else if((Displace8Bit) ||
                        (Displace16Bit))
                {
                    u8 Dest = (Op1 >> 0) & ((1<<3)-1);
                    u8 Src =  (Op1 >> 3) & ((1<<3)-1);
                    
                    s16 Value;
                    if(Displace8Bit)
                    {
                        u8 Op2 = At[Index++];
                        s8 Value_ = *(u8 *)&Op2;
                        Value = Value_;
                    }
                    else
                    {
                        Assert(Displace16Bit);
                        u8 ValueLow = At[Index++];
                        u8 ValueHigh = At[Index++];
                        u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                        
                        Value = *(u16 *)&ValueWide;
                    }
                    
                    string Bytes[] = 
                    { 
                        String("bx + si"), String("bx + di"), String("bp + si"), String("bp + di"), String("si"), String("di"), String("bp"), String("bx") 
                    };
                    
                    string Op;
                    if(MovRegisterMemoryToFromRegister || MovImmediateToRegisterMemory)
                    {
                        Op = String("mov");
                    }
                    else if(AddRegisterMemoryToFromRegister || AddImmediateToRegisterMemory)
                    {
                        Op = String("add");
                    }
                    else if(SubRegisterMemoryToFromRegister)
                    {
                        Op = String("sub");
                    }
                    
                    else if(CmpRegisterMemoryAndFromRegister)
                    {
                        Op = String("cmp");
                    }
                    else
                    {
                        Op = String("TODO");
                    }
                    
                    
                    if(Direction)
                    {
                        AppendFormatString(&State, "%S %S, [%S", Op, GetRegisterName(Src, IsWord), Bytes[Dest]);
                        
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
                        if(AddImmediateToRegisterMemory)
                        {
                            AppendFormatString(&State, "%S word [%S", Op, Bytes[Dest]);
                        }
                        else
                        {
                            AppendFormatString(&State, "%S [%S", Op, Bytes[Dest]);
                        }
                        
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
                        if(AddRegisterMemoryToFromRegister || 
                           SubRegisterMemoryToFromRegister ||
                           MovRegisterMemoryToFromRegister ||
                           CmpRegisterMemoryAndFromRegister)
                        {
                            AppendFormatString(&State, ", %S", GetRegisterName(Src, IsWord));
                        }
                        else if(MovImmediateToRegisterMemory || AddImmediateToRegisterMemory)
                        {
                            if(IsWord)
                            {
                                
                                if(AddImmediateToRegisterMemory && (Op0 >> 1) & 1)
                                {
                                    u8 ValueUnsigned = At[Index++];
                                    Value = *(u16 *)&ValueUnsigned;
                                }
                                else
                                {
                                    u8 ValueLow = At[Index++];
                                    u8 ValueHigh = At[Index++];
                                    u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                                    Value = *(u16 *)&ValueWide;
                                }
                                
                                
                                if(MovImmediateToRegisterMemory)
                                {
                                    AppendFormatString(&State, ", word %d", Value);
                                }
                                else
                                {
                                    AppendFormatString(&State, ", %d", Value);
                                }
                            }
                        }
                        else
                        {
                            AppendFormatString(&State, "TODO");
                        }
                    }
                }
                else
                {
                    AppendFormatString(&State, "; Unknown mod %b from Op code %b", Mod, Op0);
                }
                
            }
            else if(MovImmediateToRegister)
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

inline void
RunListing41Tests(memory_arena *Arena)
{    
    {
        u8 Op[] = { 0b00000011, 0b00011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add bx, [bx + si]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000011, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add bx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b11000110, 0b00000010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add si, 2\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b11000101, 0b00000010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add bp, 2\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b11000001, 0b00001000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add cx, 8\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000011, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add bx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000011, 0b01001111, 0b00000010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add cx, [bx + 2]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000010, 0b01111010, 0b00000100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add bh, [bp + si + 4]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000011, 0b01111011, 0b00000110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add di, [bp + di + 6]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000001, 0b00011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add [bx + si], bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000001, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add [bp], bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000001, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add [bp], bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000001, 0b01001111, 0b00000010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add [bx + 2], cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000000, 0b01111010, 0b00000100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add [bp + si + 4], bh\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000001, 0b01111011, 0b00000110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add [bp + di + 6], di\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000000, 0b00000111, 0b00100010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add byte [bx], 34\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b10000010, 0b11101000, 0b00000011, 0b00011101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add word [bp + si + 1000], 29\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000011, 0b01000110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add ax, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000010, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add al, [bx + si]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000001, 0b11011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add ax, bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000000, 0b11100000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add al, ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000101, 0b11101000, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add ax, 1000\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000100, 0b11100010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add al, -30\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000100, 0b00001001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add al, 9\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101011, 0b00011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub bx, [bx + si]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101011, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub bx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b11101110, 0b00000010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub si, 2\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b11101101, 0b00000010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub bp, 2\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b11101001, 0b00001000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub cx, 8\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101011, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub bx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101011, 0b01001111, 0b00000010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub cx, [bx + 2]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101010, 0b01111010, 0b00000100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub bh, [bp + si + 4]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101011, 0b01111011, 0b00000110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub di, [bp + di + 6]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101001, 0b00011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub [bx + si], bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101001, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub [bp], bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101001, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub [bp], bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101001, 0b01001111, 0b00000010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub [bx + 2], cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000000, 0b00101111, 0b00100010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub byte [bx], 34\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b00101001, 0b00011101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub word [bx + di], 29\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101101, 0b11101000, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub ax, 1000\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00111011, 0b00011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp bx, [bx + si]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00111011, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp bx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b11111110, 0b00000010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp si, 2\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00111001, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp [bp], bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000000, 0b00111111, 0b00100010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp byte [bx], 34\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b00111110, 0b11100010, 0b00010010, 0b00011101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp word [4834], 29\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00111001, 0b11011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp ax, bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00111101, 0b11101000, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp ax, 1000\n"), Actual);
    }
    
}

void
RunAllTests(memory_arena *Arena)
{
    RunListing38Tests(Arena);
    RunListing39Tests(Arena);
    RunListing40Tests(Arena);
    RunListing41Tests(Arena);
    
#if 1
    string FileName = String("test");
#else
    string FileName = String("listing_41");
#endif
    
    string Source = Win32ReadEntireFile(Arena, FileName);
    AssertTrue(Source.Data != 0);
    
    PlatformConsoleOut("\n");
    b32 IsFirst = true;
    for(u32 Index = 0;
        Index < Source.Size;
        ++Index)
    {
        if(IsFirst)
        {
            IsFirst = false;
        }
        else
        {
            PlatformConsoleOut(", ");
        }
        u8 Byte = Source.Data[Index];
        PlatformConsoleOut("%b", Byte);
    }
    PlatformConsoleOut("\n");
    
    PlatformConsoleOut("\n; %S\n", FileName);
    PlatformConsoleOut("\nbits 16\n\n");
    string Output = Parse(Arena, Source.Data, Source.Size);
    PlatformConsoleOut("%S\n", Output);
    
}
