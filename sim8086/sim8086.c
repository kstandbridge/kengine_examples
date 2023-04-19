#define KENGINE_WIN32
#define KENGINE_TEST
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "sim8086.h"

#define B(Size, Bits) { Encoding_Bits, 3, Bits }
#define D { Encoding_D , 1 }
#define D { Encoding_D , 1 }
#define W { Encoding_W , 1 }
#define MOD { Encoding_MOD , 2 }
#define REG { Encoding_REG , 3 }
#define RM { Encoding_RM , 3 }
#define DISP_LO { Encoding_DISP_LO , 8 }
#define DISP_HI { Encoding_DISP_HI , 8 }
#define DATA { Encoding_DATA , 8 }
#define DATA_IF_W { Encoding_DATA_IF_W , 8 }
#define ADDR_LO { Encoding_ADDR_LO , 8 }
#define ADDR_HI { Encoding_ADDR_HI , 8 }

global instruction_table_entry GlobalInstructionTable[] = 
{
    { Instruction_Mov, 0b100010, 6, { D, W, MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_MovImmediate, 0b1100011, 7, { W, MOD, B(3, 0b000), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_Mov, 0b1011, 4, { W, REG, DATA, DATA_IF_W } },
    { Instruction_Mov, 0b101000, 6, { D, W, ADDR_LO, ADDR_HI } },
    
    { Instruction_Push, 0b1111111, 7, { W, MOD, B(3, 0b110), RM, DISP_LO, DISP_HI } },
    { Instruction_PushRegister, 0b01010, 5, { REG } },
    { Instruction_PushSegmentRegister, 0b000, 3, { MOD, B(3, 0b110) } }
};

inline u8
GetBits(u8 Input, u8 Position, u8 Count)
{
    // NOTE(kstandbridge): Position 8 is far left, 0 is far right
    u8 Result = (Input >> (Position + 1 - Count) & ~(~0 << Count));
    return Result;
}

internal instruction
GetNextInstruction(simulator_context *Context)
{
    instruction Result = {0};
    if(Context->InstructionStreamAt < Context->InstructionStreamSize)
    {
        u8 Op0 = Context->InstructionStream[Context->InstructionStreamAt++];
        umm StartingAt = Context->InstructionStreamAt;
        for(u32 TableIndex = 0;
            TableIndex< ArrayCount(GlobalInstructionTable);
            ++TableIndex)
        {
            s8 BitsAt = 7;
            instruction_table_entry TestEntry = GlobalInstructionTable[TableIndex];
            
            if((Op0 >> (8 - TestEntry.OpCodeSize)) != TestEntry.OpCode)
            {
                continue;
            }
            
            BitsAt -= TestEntry.OpCodeSize;
            if(BitsAt < 0)
            {
                BitsAt = 7;
                if(Context->InstructionStreamAt < Context->InstructionStreamSize)
                {
                    Op0 = Context->InstructionStream[Context->InstructionStreamAt++];
                }
                else
                {
                    Result.Type = Instruction_NOP;
                    Context->InstructionStreamAt = StartingAt;
                }
            }
            
            Result.OpCode = TestEntry.OpCode;
            Result.Type = TestEntry.Type;
            for(u32 FieldIndex = 0;
                FieldIndex < ArrayCount(TestEntry.Fields);
                ++FieldIndex)
            {
                encoding Field = TestEntry.Fields[FieldIndex];
                if(Field.Type == Encoding_NOP)
                {
                    // NOTE(kstandbridge): No more fields
                    break;
                }
                else if(Field.Type == Encoding_DATA_IF_W)
                {
                    if(Result.Bits[Encoding_W] == 0)
                    {
                        // NOTE(kstandbridge): We are at the next op
                        break;
                    }
                }
                else if(Field.Type == Encoding_DISP_LO)
                {
                    if((Result.Bits[Encoding_MOD] == Mod_MemoryMode) && 
                       (Result.Bits[Encoding_RM] == EffectiveAddress_DirectAddress))
                    {
                        // NOTE(kstandbridge): Except when R/M = 110, then 16-bit displacement follows
                    }
                    else if((Result.Bits[Encoding_MOD] != Mod_8BitDisplace) &&
                            (Result.Bits[Encoding_MOD] != Mod_16BitDisplace))
                    {
                        continue;
                    }
                }
                else if(Field.Type == Encoding_DISP_HI)
                {
                    if((Result.Bits[Encoding_MOD] == Mod_MemoryMode) && 
                       (Result.Bits[Encoding_RM] == EffectiveAddress_DirectAddress))
                    {
                        // NOTE(kstandbridge): Except when R/M = 110, then 16-bit displacement follows
                    }
                    else if(Result.Bits[Encoding_MOD] != Mod_16BitDisplace)
                    {
                        continue;
                    }
                }
                u8 Bits = GetBits(Op0, BitsAt, Field.Size);
                Result.Bits[Field.Type] = Bits;
                BitsAt -= Field.Size;
                if(BitsAt < 0)
                {
                    BitsAt = 7;
                    if(Context->InstructionStreamAt < Context->InstructionStreamSize)
                    {
                        Op0 = Context->InstructionStream[Context->InstructionStreamAt++];
                    }
                    else
                    {
                        break;
                    }
                }
                
                if(Field.Type == Encoding_Bits)
                {
                    if(Result.Bits[Field.Type] != Field.Value)
                    {
                        Result.Type = Instruction_NOP;
                        Context->InstructionStreamAt = StartingAt;
                        break;
                    }
                }
                
                
            }
            
            if(TestEntry.Type != Instruction_NOP)
            {
                break;
            }
            Context->InstructionStreamAt = StartingAt;
        }
        
    }
    
    return Result;
}

inline string
InstructionToAssembly(memory_arena *Arena, instruction Instruction)
{
    string Result;
    format_string_state State = BeginFormatString();
    
    string Op = InstructionToString(Instruction.Type);
    b32 IsWord = Instruction.Bits[Encoding_W];
    string Dest;
    
    if(Instruction.Type == Instruction_MovImmediate)
    {
        s16 Value;
        
        if(Instruction.Bits[Encoding_W])
        {
            u8 ValueLow = Instruction.Bits[Encoding_DATA];
            u8 ValueHigh = Instruction.Bits[Encoding_DATA_IF_W];
            u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
            Value = *(s16 *)&ValueWide;
            Dest = FormatString(Arena, "word %d", Value);
        }
        else
        {
            Value = *(s8 *)&Instruction.Bits[Encoding_DATA];
            Dest = FormatString(Arena, "byte %d", Value);
        }
        
        s16 Displacement = 0;
        
        if(Instruction.Bits[Encoding_W])
        {        
            u8 ValueLow = Instruction.Bits[Encoding_DISP_LO];
            u8 ValueHigh = Instruction.Bits[Encoding_DISP_HI];
            u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
            Displacement = *(s16 *)&ValueWide;
        }
        else
        {
            Displacement = *(s8 *)&Instruction.Bits[Encoding_DISP_LO];
        }
        
        string Src = EffectiveAddressToString(Instruction.Bits[Encoding_RM]);
        
        if(Displacement > 0)
        {
            AppendFormatString(&State, "%S [%S + %d], %S", Op, Src, Displacement, Dest);
        }
        else
        {
            AppendFormatString(&State, "%S [%S], %S", Op, Src, Dest);
        }
    }
    else if(Instruction.Type == Instruction_PushSegmentRegister)
    {
        AppendFormatString(&State, "%S %S", Op, SegmentRegisterToString(Instruction.Bits[Encoding_MOD]));
    }
    else if((Instruction.Bits[Encoding_ADDR_HI] > 0) ||
            (Instruction.Bits[Encoding_ADDR_LO] > 0))
    {
        u8 ValueLow = Instruction.Bits[Encoding_ADDR_LO];
        u8 ValueHigh = Instruction.Bits[Encoding_ADDR_HI];
        u16 Value = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
        
        Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
        if(Instruction.Bits[Encoding_D])
        {
            AppendFormatString(&State, "%S [%u], %S", Op, Value, Dest);
        }
        else
        {
            AppendFormatString(&State, "%S %S, [%u]", Op, Dest, Value);
        }
    }
    else
    {    
        if(Instruction.Bits[Encoding_DATA] > 0)
        {
            s16 Value;
            
            if(Instruction.Bits[Encoding_DATA_IF_W] > 0)
            {
                u8 ValueLow = Instruction.Bits[Encoding_DATA];
                u8 ValueHigh = Instruction.Bits[Encoding_DATA_IF_W];
                u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                Value = *(s16 *)&ValueWide;
            }
            else
            {
                Value = *(s8 *)&Instruction.Bits[Encoding_DATA];
            }
            Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
            AppendFormatString(&State, "%S %S, %d", Op, Dest, Value);
        }
        else
        {
            
            switch(Instruction.Bits[Encoding_MOD])
            {
                case Mod_RegisterMode:
                {
                    string Src = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_RM]) : RegisterByteToString(Instruction.Bits[Encoding_RM]);
                    Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
                    AppendFormatString(&State, "%S %S, %S", Op, Src, Dest);
                } break;
                case Mod_MemoryMode:
                {
                    if(Instruction.Bits[Encoding_RM] == EffectiveAddress_DirectAddress)
                    {
                        // NOTE(kstandbridge): Except when R/M = 110, then 16-bit displacement follows
                        
                        s16 Displacement = 0;
                        
                        u8 ValueLow = Instruction.Bits[Encoding_DISP_LO];
                        u8 ValueHigh = Instruction.Bits[Encoding_DISP_HI];
                        u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                        Displacement = *(s16 *)&ValueWide;
                        
                        Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
                        if(Instruction.Type == Instruction_Mov)
                        {
                            AppendFormatString(&State, "%S %S, [%d]", Op, Dest, Displacement);
                        }
                        else
                        {
                            AppendFormatString(&State, "%S word [%d]", Op, Displacement);
                        }
                    }
                    else
                    {
                        string Src = EffectiveAddressToString(Instruction.Bits[Encoding_RM]);
                        if(Instruction.Type == Instruction_Mov)
                        {                            
                            Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
                            if(Instruction.Bits[Encoding_D])
                            {
                                AppendFormatString(&State, "%S %S, [%S]", Op, Dest, Src);
                            }
                            else
                            {
                                AppendFormatString(&State, "%S [%S], %S", Op, Src, Dest);
                            }
                        }
                        else
                        {
                            if(Instruction.Type == Instruction_PushRegister)
                            {
                                Dest = RegisterWordToString(Instruction.Bits[Encoding_REG]);
                                AppendFormatString(&State, "%S %S", Op, Dest);
                            }
                            else if(Instruction.Type == Instruction_PushSegmentRegister)
                            {
                                Dest = SegmentRegisterToString(Instruction.Bits[Encoding_MOD]);
                                AppendFormatString(&State, "%S %S", Op, Dest);
                            }
                            else
                            {
                                if(Instruction.Bits[Encoding_W])
                                {
                                    AppendFormatString(&State, "%S word [%S]", Op, Src);
                                }
                                else
                                {
                                    AppendFormatString(&State, "TODO BYTE %S byte [%S], %d", Op, Src);
                                }
                            }
                        }
                    }
                } break;
                case Mod_8BitDisplace:
                case Mod_16BitDisplace:
                {
                    s16 Value;
                    if(Instruction.Bits[Encoding_MOD] == Mod_16BitDisplace)
                    {
                        u8 ValueLow = Instruction.Bits[Encoding_DISP_LO];
                        u8 ValueHigh = Instruction.Bits[Encoding_DISP_HI];
                        u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                        Value = *(s16 *)&ValueWide;
                    }
                    else
                    {
                        Assert(Instruction.Bits[Encoding_MOD] == Mod_8BitDisplace);
                        Value = *(s8 *)&Instruction.Bits[Encoding_DISP_LO];
                    }
                    
                    string Src;
                    if(Value > 0)
                    {
                        Src = FormatString(Arena, "[%S + %d]", EffectiveAddressToString(Instruction.Bits[Encoding_RM]), Value);
                    }
                    else if(Value < 0)
                    {
                        Value *= -1;
                        Src = FormatString(Arena, "[%S - %d]", EffectiveAddressToString(Instruction.Bits[Encoding_RM]), Value);
                    }
                    else
                    {
                        Assert(Value == 0);
                        Src = FormatString(Arena, "[%S]", EffectiveAddressToString(Instruction.Bits[Encoding_RM]));
                    }
                    
                    Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
                    if(Instruction.Bits[Encoding_D])
                    {
                        string Temp = Dest;
                        Dest = Src;
                        Src = Temp;
                    }
                    
                    if(Instruction.Type == Instruction_Mov)
                    {
                        AppendFormatString(&State, "%S %S, %S", Op, Src, Dest);
                    }
                    else
                    {
                        AppendFormatString(&State, "%S word %S", Op, Src);
                    }
                    
                } break;
                
                default:
                {
                    AppendFormatString(&State, "Invalid MOD");
                } break;
                
            }
        }
        
    }
    
    Result = EndFormatString(&State, Arena);
    return Result;
}

inline string
Parse(memory_arena *Arena, u8 *Buffer, umm Size)
{
    // NOTE(kstandbridge): Test harness
    
    string Result;
    format_string_state StringState = BeginFormatString();
    
    simulator_context Context = 
    {
        .InstructionStream = Buffer,
        .InstructionStreamAt = 0,
        .InstructionStreamSize = Size
    };
    
    for(;;)
    {
        instruction Instruction = GetNextInstruction(&Context);
        if(Instruction.Type != Instruction_NOP)
        {
            string Assembly = InstructionToAssembly(Arena, Instruction);
            AppendFormatString(&StringState, "%S\n", Assembly);
        }
        else
        {
            break;
        }
    }
    
    if(StringState.BufferSize > 0)
    {
        Result = EndFormatString(&StringState, Arena);
    }
    else
    {
        Result = String("; no known byte codes");
    }
    return Result;
}

inline void
RunGetBitsTest(memory_arena *Arena)
{
    Arena; 
    {
        u8 Input = 0b11100000;
        u8 Expected = 0b111;
        u8 Actual = GetBits(Input, 7, 3);
        AssertEqualBits(Expected, Actual);
    }
    
    {
        u8 Input = 0b0000111;
        u8 Expected = 0b111;
        u8 Actual = GetBits(Input, 2, 3);
        AssertEqualBits(Expected, Actual);
    }
    
    {
        u8 Input = 0b0000011;
        u8 Expected = 0b11;
        u8 Actual = GetBits(Input, 1, 2);
        AssertEqualBits(Expected, Actual);
    }
    
    {
        u8 Input = 0b0000001;
        u8 Expected = 0b1;
        u8 Actual = GetBits(Input, 0, 1);
        AssertEqualBits(Expected, Actual);
    }
    
    {
        u8 Input = 0b11111110;
        u8 Expected = 0b0;
        u8 Actual = GetBits(Input, 0, 1);
        AssertEqualBits(Expected, Actual);
    }
    
    {
        u8 Input = 0b11111101;
        u8 Expected = 0b0;
        u8 Actual = GetBits(Input, 1, 1);
        AssertEqualBits(Expected, Actual);
    }
    
    {
        u8 Input = 0b11111100;
        u8 Expected = 0b00;
        u8 Actual = GetBits(Input, 1, 2);
        AssertEqualBits(Expected, Actual);
    }
}

inline void
RunDisassembleTests(memory_arena *Arena)
{
    Arena;
    
    {
        // NOTE(kstandbridge): mov si, bx
        u8 Buffer[] = { 0b10001001, 0b11011110 };
        simulator_context Context = 
        {
            .InstructionStream = Buffer,
            .InstructionStreamAt = 0,
            .InstructionStreamSize = sizeof(Buffer)
        };
        instruction Instruction = GetNextInstruction(&Context);
        AssertEqualU32(Instruction_Mov, Instruction.Type);
        AssertEqualBits(0b100010, Instruction.OpCode);
        AssertEqualBits(0b0, Instruction.Bits[Encoding_D]);
        AssertEqualBits(0b1, Instruction.Bits[Encoding_W]);
        AssertEqualBits(0b11, Instruction.Bits[Encoding_MOD]);
        AssertEqualBits(0b011, Instruction.Bits[Encoding_REG]);
        AssertEqualBits(0b110, Instruction.Bits[Encoding_RM]);
        
    }
    
    {
        // NOTE(kstandbridge): mov cl, 12
        u8 Buffer[] = { 0b10110001, 0b00001100 };
        simulator_context Context = 
        {
            .InstructionStream = Buffer,
            .InstructionStreamAt = 0,
            .InstructionStreamSize = sizeof(Buffer)
        };
        instruction Instruction = GetNextInstruction(&Context);
        AssertEqualBits(0b1011, Instruction.OpCode);
        AssertEqualBits(0b0, Instruction.Bits[Encoding_W]);
        AssertEqualBits(0b001, Instruction.Bits[Encoding_REG]);
        AssertEqualBits(0b00001100, Instruction.Bits[Encoding_DATA]);
    }
    
    {
        // NOTE(kstandbridge): push cs
        u8 Buffer[] = { 0b00001110 };
        simulator_context Context = 
        {
            .InstructionStream = Buffer,
            .InstructionStreamAt = 0,
            .InstructionStreamSize = sizeof(Buffer)
        };
        instruction Instruction = GetNextInstruction(&Context);
        AssertEqualU32(Instruction_PushSegmentRegister, Instruction.Type);
        AssertEqualBits(0b000, Instruction.OpCode);
        AssertEqualBits(0b01, Instruction.Bits[Encoding_MOD]);
        AssertEqualBits(0b110, Instruction.Bits[Encoding_Bits]);
    }
}

inline void
RunDisassembleToAssemblyTests(memory_arena *Arena)
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
        u8 Op[] = { 0b10110001, 0b00001100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov cl, 12\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10110101, 0b11110100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov ch, -12\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10111001, 0b00001100, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov cx, 12\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10111001, 0b11110100, 0b11111111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov cx, -12\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10111010, 0b01101100, 0b00001111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov dx, 3948\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10111010, 0b10010100, 0b11110000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov dx, -3948\n"), Actual);
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
    
    {
        u8 Op[] = { 0b10001011, 0b01000001, 0b11011011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov ax, [bx + di - 37]\n"), Actual);
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
    
    {
        u8 Op[] = { 0b11111111, 0b00110010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("push word [bp + si]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b00110110, 0b10111000, 0b00001011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("push word [3000]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b01110001, 0b11100010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("push word [bx + di - 30]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01010001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("push cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01010000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("push ax\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01010010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("push dx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00001110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("push cs\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001111, 0b00000010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("pop word [bp + si]\n"), Actual);
    }
#if 0
    
    {
        u8 Op[] = { 0b10001111, 0b00000110, 0b00000011, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("pop word [3]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001111, 0b10000001, 0b01001000, 0b11110100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("pop word [bx + di - 3000]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01011100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("pop sp\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01011111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("pop di\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01011110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("pop si\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00011111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("pop ds\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000111, 0b10000110, 0b00011000, 0b11111100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xchg ax, [bp - 1000]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000111, 0b01101111, 0b00110010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xchg [bx + 50], bp\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10010000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xchg ax, ax\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10010010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xchg ax, dx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10010100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xchg ax, sp\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10010110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xchg ax, si\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10010111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xchg ax, di\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000111, 0b11001010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xchg cx, dx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000111, 0b11110001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xchg si, cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000110, 0b11001100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xchg cl, ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11100100, 0b11001000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("in al, 200\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11101100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("in al, dx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11101101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("in ax, dx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11100111, 0b00101100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("out 44, ax\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11101110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("out dx, al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xlat\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001101, 0b10000001, 0b10001100, 0b00000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("lea ax, [bx + di + 1420]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001101, 0b01011110, 0b11001110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("lea bx, [bp - 50]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001101, 0b10100110, 0b00010101, 0b11111100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("lea sp, [bp - 1003]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001101, 0b01111000, 0b11111001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("lea di, [bx + si - 7]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000101, 0b10000001, 0b10001100, 0b00000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("lds ax, [bx + di + 1420]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000101, 0b01011110, 0b11001110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("lds bx, [bp - 50]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000101, 0b10100110, 0b00010101, 0b11111100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("lds sp, [bp - 1003]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000101, 0b01111000, 0b11111001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("lds di, [bx + si - 7]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000100, 0b10000001, 0b10001100, 0b00000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("les ax, [bx + di + 1420]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000100, 0b01011110, 0b11001110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("les bx, [bp - 50]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000100, 0b10100110, 0b00010101, 0b11111100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("les sp, [bp - 1003]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000100, 0b01111000, 0b11111001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("les di, [bx + si - 7]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10011111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("lahf\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10011110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sahf\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10011100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("pushf\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10011101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("popf\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000011, 0b01001110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add cx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000011, 0b00010000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add dx, [bx + si]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000000, 0b10100011, 0b10001000, 0b00010011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add [bp + di + 5000], ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000000, 0b00000111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add [bx], al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000001, 0b11000100, 0b10001000, 0b00000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add sp, 392\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b11000110, 0b00000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add si, 5\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000101, 0b11101000, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add ax, 1000\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000000, 0b11000100, 0b00011110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add ah, 30\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000100, 0b00001001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add al, 9\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000001, 0b11011001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add cx, bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00000000, 0b11000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("add ch, al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00010011, 0b01001110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("adc cx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00010011, 0b00010000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("adc dx, [bx + si]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00010000, 0b10100011, 0b10001000, 0b00010011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("adc [bp + di + 5000], ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00010000, 0b00000111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("adc [bx], al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000001, 0b11010100, 0b10001000, 0b00000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("adc sp, 392\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b11010110, 0b00000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("adc si, 5\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00010101, 0b11101000, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("adc ax, 1000\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000000, 0b11010100, 0b00011110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("adc ah, 30\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00010100, 0b00001001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("adc al, 9\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00010001, 0b11011001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("adc cx, bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00010000, 0b11000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("adc ch, al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc ax\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111110, 0b11000110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc dh\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111110, 0b11000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111110, 0b11000100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01000100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc sp\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01000111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc di\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111110, 0b10000110, 0b11101010, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc byte [bp + 1002]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b01000111, 0b00100111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc word [bx + 39]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111110, 0b01000000, 0b00000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc byte [bx + si + 5]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b10000011, 0b11000100, 0b11011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc word [bp + di - 10044]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b00000110, 0b10000101, 0b00100100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc word [9349]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111110, 0b01000110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("inc byte [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("aaa\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("daa\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101011, 0b01001110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub cx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101011, 0b00010000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub dx, [bx + si]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101000, 0b10100011, 0b10001000, 0b00010011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub [bp + di + 5000], ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101000, 0b00000111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub [bx], al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000001, 0b11101100, 0b10001000, 0b00000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub sp, 392\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b11101110, 0b00000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub si, 5\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101101, 0b11101000, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub ax, 1000\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000000, 0b11101100, 0b00011110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub ah, 30\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101100, 0b00001001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub al, 9\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101001, 0b11011001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub cx, bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101000, 0b11000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sub ch, al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00011011, 0b01001110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb cx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00011011, 0b00010000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb dx, [bx + si]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00011000, 0b10100011, 0b10001000, 0b00010011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb [bp + di + 5000], ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00011000, 0b00000111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb [bx], al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000001, 0b11011100, 0b10001000, 0b00000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb sp, 392\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000011, 0b11011110, 0b00000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb si, 5\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00011101, 0b11101000, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb ax, 1000\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000000, 0b11011100, 0b00011110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb ah, 30\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00011100, 0b00001001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb al, 9\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00011001, 0b11011001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb cx, bx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00011000, 0b11000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb ch, al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01001000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec ax\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01001001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111110, 0b11001110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec dh\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111110, 0b11001000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111110, 0b11001100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01001100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec sp\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01001111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec di\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111110, 0b10001110, 0b11101010, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec byte [bp + 1002]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b01001111, 0b00100111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec word [bx + 39]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111110, 0b01001000, 0b00000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec byte [bx + si + 5]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b10001011, 0b11000100, 0b11011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec word [bp + di - 10044]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b00001110, 0b10000101, 0b00100100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec word [9349]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111110, 0b01001110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("dec byte [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b11011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg ax\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b11011001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b11011110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg dh\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b11011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b11011100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b11011100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg sp\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b11011111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg di\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b10011110, 0b11101010, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg byte [bp + 1002]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b01011111, 0b00100111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg word [bx + 39]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b01011000, 0b00000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg byte [bx + si + 5]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b10011011, 0b11000100, 0b11011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg word [bp + di - 10044]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b00011110, 0b10000101, 0b00100100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg word [9349]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("neg byte [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00111001, 0b11001011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp bx, cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00111010, 0b10110110, 0b10000110, 0b00000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp dh, [bp + 390]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00111001, 0b01110110, 0b00000010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp [bp + 2], si\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000000, 0b11111011, 0b00010100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp bl, 20\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000000, 0b00111111, 0b00100010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp byte [bx], 34\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00111101, 0b01100101, 0b01011101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp ax, 23909\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00111111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("aas\n"), Actual);
    }
    
    
    {
        u8 Op[] = { 0b00101111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("das\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b11100000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mul al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b11100001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mul cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b01100110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mul word [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b10100001, 0b11110100, 0b00000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mul byte [bx + di + 500]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b11101101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("imul ch\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b11101010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("imul dx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b00101111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("imul byte [bx]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b00101110, 0b00001011, 0b00100101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("imul word [9483]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010100, 0b00001010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("aam\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b11110011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("div bl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b11110100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("div sp\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b10110000, 0b10101110, 0b00001011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("div byte [bx + si + 2990]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b10110011, 0b11101000, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("div word [bp + di + 1000]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b11111000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("idiv ax\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("idiv si\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b00111010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("idiv byte [bp + si]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b10111111, 0b11101101, 0b00000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("idiv word [bx + 493]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010101, 0b00001010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("aad\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10011000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cbw\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10011001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cwd\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b11010100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("not ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b11010011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("not bl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b11010100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("not sp\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b11010110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("not si\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110111, 0b01010110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("not word [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b10010110, 0b10110001, 0b00100110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("not byte [bp + 9905]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010000, 0b11100100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("shl ah, 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010001, 0b11101000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("shr ax, 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010001, 0b11111011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sar bx, 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010001, 0b11000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rol cx, 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010000, 0b11001110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("ror dh, 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010001, 0b11010100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rcl sp, 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010001, 0b11011101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rcr bp, 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010001, 0b01100110, 0b00000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("shl word [bp + 5], 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010000, 0b10101000, 0b00111001, 0b11111111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("shr byte [bx + si - 199], 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010000, 0b10111001, 0b11010100, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sar byte [bx + di - 300], 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010001, 0b01000110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rol word [bp], 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010001, 0b00001110, 0b01001010, 0b00010011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("ror word [4938], 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010000, 0b00010110, 0b00000011, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rcl byte [3], 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010001, 0b00011111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rcr word [bx], 1\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010010, 0b11100100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("shl ah, cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010011, 0b11101000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("shr ax, cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010011, 0b11111011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sar bx, cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010011, 0b11000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rol cx, cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010010, 0b11001110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("ror dh, cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010011, 0b11010100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rcl sp, cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010011, 0b11011101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rcr bp, cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010011, 0b01100110, 0b00000101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("shl word [bp + 5], cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010011, 0b10101000, 0b00111001, 0b11111111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("shr word [bx + si - 199], cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010010, 0b10111001, 0b11010100, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sar byte [bx + di - 300], cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010010, 0b01000110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rol byte [bp], cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010010, 0b00001110, 0b01001010, 0b00010011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("ror byte [4938], cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010010, 0b00010110, 0b00000011, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rcl byte [3], cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11010011, 0b00011111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rcr word [bx], cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100000, 0b11100000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and al, ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100000, 0b11001101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and ch, cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100001, 0b11110101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and bp, si\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100001, 0b11100111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and di, sp\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100100, 0b01011101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and al, 93\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100101, 0b10101000, 0b01001111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and ax, 20392\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100000, 0b01101010, 0b00001010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and [bp + si + 10], ch\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100001, 0b10010001, 0b11101000, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and [bx + di + 1000], dx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100011, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and bx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100011, 0b00001110, 0b00100000, 0b00010001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and cx, [4384]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000000, 0b01100110, 0b11011001, 0b11101111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and byte [bp - 39], 239\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000001, 0b10100000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and word [bx + si - 4332], 10328\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000101, 0b11001011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("test bx, cx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000100, 0b10110110, 0b10000110, 0b00000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("test dh, [bp + 390]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000101, 0b01110110, 0b00000010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("test [bp + 2], si\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b11000011, 0b00010100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("test bl, 20\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110110, 0b00000111, 0b00100010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("test byte [bx], 34\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10101001, 0b01100101, 0b01011101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("test ax, 23909\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00001000, 0b11100000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or al, ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00001000, 0b11001101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or ch, cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00001001, 0b11110101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or bp, si\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00001001, 0b11100111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or di, sp\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00001100, 0b01011101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or al, 93\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00001101, 0b10101000, 0b01001111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or ax, 20392\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00001000, 0b01101010, 0b00001010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or [bp + si + 10], ch\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00001001, 0b10010001, 0b11101000, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or [bx + di + 1000], dx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00001011, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or bx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00001011, 0b00001110, 0b00100000, 0b00010001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or cx, [4384]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000000, 0b01001110, 0b11011001, 0b11101111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or byte [bp - 39], 239\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000001, 0b10001000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or word [bx + si - 4332], 10328\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110000, 0b11100000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor al, ah\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110000, 0b11001101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor ch, cl\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110001, 0b11110101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor bp, si\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110001, 0b11100111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor di, sp\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110100, 0b01011101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor al, 93\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110101, 0b10101000, 0b01001111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor ax, 20392\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110000, 0b01101010, 0b00001010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor [bp + si + 10], ch\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110001, 0b10010001, 0b11101000, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor [bx + di + 1000], dx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110011, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor bx, [bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110011, 0b00001110, 0b00100000, 0b00010001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor cx, [4384]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000000, 0b01110110, 0b11011001, 0b11101111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor byte [bp - 39], 239\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000001, 0b10110000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor word [bx + si - 4332], 10328\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110011, 0b10100100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rep movsb\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110011, 0b10100110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rep cmpsb\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110011, 0b10101110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rep scasb\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110011, 0b10101100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rep lodsb\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110011, 0b10100101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rep movsw\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110011, 0b10100111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rep cmpsw\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110011, 0b10101111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rep scasw\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110011, 0b10101101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rep lodsw\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110011, 0b10101010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rep stosb\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110011, 0b10101011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("rep stosw\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b00010110, 0b00100001, 0b10011001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("call [39201]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b01010110, 0b10011100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("call [bp - 100]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b11010100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("call sp\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b11010000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("call ax\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b11100000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jmp ax\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b11100111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jmp di\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b00100110, 0b00001100, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jmp [12]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b00100110, 0b00101011, 0b00010001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jmp [4395]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000010, 0b11111001, 0b11111111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("ret -7\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000010, 0b11110100, 0b00000001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("ret 500\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("ret\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01110100, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("je label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01111100, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jl label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01111110, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jle label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01110010, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jb label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01110110, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jbe label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01111010, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jp label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01110000, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jo label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01111000, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("js label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01110101, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jne label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01111101, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jnl label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01111111, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jg label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01110011, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jnb label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01110111, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("ja label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01111011, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jnp label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01110001, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jno label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01111001, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jns label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11100010, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("loop label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11100001, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("loopz label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11100000, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("loopnz label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11100011, 0b11111110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jcxz label\n"), Actual);
    }
    
    
    {
        u8 Op[] = { 0b11001101, 0b00001101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("int 13\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11001100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("int3\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11001110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("into\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11001111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("iret\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("clc\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmc\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("stc\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cld\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("std\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cli\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sti\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("hlt\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10011011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("wait\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110000, 0b11110110, 0b10010110, 0b10110001, 0b00100110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("lock not byte [bp + 9905]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000110, 0b00000110, 0b01100100, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xchg [100], al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110000, 0b10000110, 0b00000110, 0b01100100, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("lock xchg [100], al\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101110, 0b10001010, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov al, cs:[bx + si]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00111110, 0b10001011, 0b00011011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov bx, ds:[bp + di]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100110, 0b10001011, 0b01010110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov dx, es:[bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110110, 0b10001010, 0b01100000, 0b00000100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov ah, ss:[bx + si + 4]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00110110, 0b00100000, 0b01101010, 0b00001010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("and ss:[bp + si + 10], ch\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00111110, 0b00001001, 0b10010001, 0b11101000, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("or ds:[bx + di + 1000], dx\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100110, 0b00110011, 0b01011110, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("xor bx, es:[bp]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00100110, 0b00111011, 0b00001110, 0b00100000, 0b00010001 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("cmp cx, es:[4384]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101110, 0b11110110, 0b01000110, 0b11011001, 0b11101111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("test byte cs:[bp - 39], 239\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10000001, 0b10011000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb word [bx + si - 4332], 10328\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b00101110, 0b10000001, 0b10011000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("sbb word cs:[bx + si - 4332], 10328\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11110000, 0b00101110, 0b11110110, 0b10010110, 0b10110001, 0b00100110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("lock not byte cs:[bp + 9905]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10011010, 0b11001000, 0b00000001, 0b01111011, 0b00000000 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("call 123:456\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11101010, 0b00100010, 0b00000000, 0b00010101, 0b00000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jmp 789:34\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b10001100, 0b01000000, 0b00111011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("mov [bx+si+59],es\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11101001, 0b00111001, 0b00001010 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jmp 2620\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11101000, 0b00011001, 0b00101110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("call 11804\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11001010, 0b10010100, 0b01000100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("retf 17556\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000010, 0b10011000, 0b01000100 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("ret 17560\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11001011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("retf\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11000011 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("ret\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b01010010, 0b11000110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("call [bp+si-0x3a]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b01011010, 0b11000110 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("call far [bp+si-0x3a]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b00100101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jmp [di]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11111111, 0b00101101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jmp far [di]\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b11101010, 0b10001000, 0b01110111, 0b01100110, 0b01010101 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jmp 21862:30600\n"), Actual);
    }
#endif
    
}

void
RunAllTests(memory_arena *Arena)
{
    RunGetBitsTest(Arena);
    RunDisassembleTests(Arena);
    RunDisassembleToAssemblyTests(Arena);
}
