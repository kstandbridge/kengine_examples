#include "sim8086.h"

#define B(Size, Bits) { Encoding_Bits, 3, Bits }

#define MOD { Encoding_MOD, 2 }
#define REG { Encoding_REG, 3 }
#define RM { Encoding_RM, 3 }

#define S { Encoding_S, 1 }
#define W { Encoding_W, 1 }
#define D { Encoding_D, 1 }
#define V { Encoding_V, 1 }
#define Z { Encoding_Z, 1 }

#define DISP_LO { Encoding_DISP_LO, 8 }
#define DISP_HI { Encoding_DISP_HI, 8 }

#define DATA { Encoding_DATA, 8 }
#define DATA_IF_W { Encoding_DATA_IF_W, 8 }

#define ADDR_LO { Encoding_ADDR_LO, 8 }
#define ADDR_HI { Encoding_ADDR_HI, 8 }

#define DATA_LO { Encoding_DATA_LO, 8 }
#define DATA_HI { Encoding_DATA_HI, 8 }

#define IP_INC8 { Encoding_IP_INC8, 8 }

global instruction_table_entry GlobalInstructionTable[] = 
{
    { Instruction_Mov, 0b100010, 6, { D, W, MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_MovImmediateMemory, 0b1100011, 7, { W, MOD, B(3, 0b000), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_MovImmediate, 0b1011, 4, { W, REG, DATA, DATA_IF_W } },
    { Instruction_Mov, 0b101000, 6, { D, W, ADDR_LO, ADDR_HI } },
    
    { Instruction_Push, 0b1111111, 7, { W, MOD, B(3, 0b110), RM, DISP_LO, DISP_HI } },
    { Instruction_PushRegister, 0b01010, 5, { REG } },
    { Instruction_PushSegmentRegister, 0b000, 3, { MOD, B(3, 0b110) } },
    
    { Instruction_Pop, 0b1000111, 7, { W, MOD, B(3, 0b000), RM, DISP_LO, DISP_HI } },
    { Instruction_PopRegister, 0b01011, 5, { REG } },
    { Instruction_PopSegmentRegister, 0b000, 3, { MOD, B(3, 0b111) } },
    
    { Instruction_Xchg, 0b1000011, 7, { W, MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_XchgWithAccumulator, 0b10010, 5, { REG } },
    
    { Instruction_In,  0b1110010, 7, { W, DATA } },
    { Instruction_In,  0b1110110, 7, { W } },
    
    { Instruction_Out, 0b1110011, 7, { W, DATA } },
    { Instruction_Out, 0b1110111, 7, { W } },
    
    { Instruction_Xlat, 0b11010111, 8, { 0 } },
    
    { Instruction_Lea, 0b10001101, 8, { MOD, REG, RM, DISP_LO, DISP_HI } },
    
    { Instruction_Lds, 0b11000101, 8, { MOD, REG, RM, DISP_LO, DISP_HI } },
    
    { Instruction_Les, 0b11000100, 8, { MOD, REG, RM, DISP_LO, DISP_HI } },
    
    { Instruction_Lahf, 0b10011111, 8, { 0 } },
    
    { Instruction_Sahf, 0b10011110, 8, { 0 } },
    
    { Instruction_Pushf, 0b10011100, 8, { 0 } },
    
    { Instruction_Popf, 0b10011101, 8, { 0 } },
    
    { Instruction_Add, 0b000000, 6, { D, W, MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_AddImmediate, 0b100000, 6, { S, W, MOD, B(3, 0b000), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_AddAccumulator, 0b0000010, 7, { W, DATA, DATA_IF_W } },
    
    { Instruction_Adc, 0b000100, 6, { D, W, MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_AdcImmediate, 0b100000, 6, { S, W, MOD, B(3, 0b010), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_AdcAccumulator, 0b0001010, 7, { W, DATA, DATA_IF_W } },
    
    { Instruction_Inc, 0b1111111, 7, { W, MOD, B(3, 0b000), RM, DISP_LO, DISP_HI } },
    { Instruction_Inc, 0b01000, 5, { REG } },
    
    { Instruction_Aaa, 0b00110111, 8, { 0 } },
    
    { Instruction_Daa, 0b00100111, 8, { 0 } },
    
    { Instruction_Sub, 0b001010, 6, { D, W, MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_SubImmediate, 0b100000, 6, { S, W, MOD, B(3, 0b101), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_SubAccumulator, 0b0010110, 7, { W, DATA, DATA_IF_W } },
    
    { Instruction_Sbb, 0b000110, 6, { D, W, MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_SbbImmediate, 0b100000, 6, { S, W, MOD, B(3, 0b011), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_SbbAccumulator, 0b0001110, 7, { W, DATA, DATA_IF_W } },
    
    { Instruction_Dec, 0b1111111, 7, { W, MOD, B(3, 0b001), RM, DISP_LO, DISP_HI } },
    { Instruction_Dec, 0b01001, 5, { REG } },
    { Instruction_Neg, 0b1111011, 7, { W, MOD, B(3, 0b011), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Cmp, 0b001110, 6, { D, W, MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_CmpImmediate, 0b100000, 6, { S, W, MOD, B(3, 0b111), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_CmpAccumulator, 0b0011110, 7, { W, DATA, DATA_IF_W } },
    
    { Instruction_Aas, 0b00111111, 8, { 0 } },
    
    { Instruction_Das, 0b00101111, 8, { 0 } },
    
    { Instruction_Mul, 0b1111011, 7, { W, MOD, B(3, 0b100), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Imul, 0b1111011, 7, { W, MOD, B(3, 0b101), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Aam, 0b11010100, 8, { DATA } },
    
    { Instruction_Div, 0b1111011, 7, { W, MOD, B(3, 0b110), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Idiv, 0b1111011, 7, { W, MOD, B(3, 0b111), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Aad, 0b11010101, 8, { DATA } },
    
    { Instruction_Cbw, 0b10011000, 8, { 0 } },
    
    { Instruction_Cwd, 0b10011001, 8, { 0 } },
    
    { Instruction_Not, 0b1111011, 7, { W, MOD, B(3, 0b010), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Shl, 0b110100, 6, { V, W, MOD, B(3, 0b100), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Shr, 0b110100, 6, { V, W, MOD, B(3, 0b101), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Sar, 0b110100, 6, { V, W, MOD, B(3, 0b111), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Rol, 0b110100, 6, { V, W, MOD, B(3, 0b000), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Ror, 0b110100, 6, { V, W, MOD, B(3, 0b001), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Rcl, 0b110100, 6, { V, W, MOD, B(3, 0b010), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Rcr, 0b110100, 6, { V, W, MOD, B(3, 0b011), RM, DISP_LO, DISP_HI } },
    
    { Instruction_And, 0b001000, 6, { D, W, MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_AndImmediate, 0b1000000, 7, { W, MOD, B(3, 0b100), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_AndAccumulator, 0b0010010, 7, { W, DATA, DATA_IF_W } },
    
    { Instruction_Test, 0b1000010, 7, { W, MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_TestImmediate, 0b1111011, 7, { W, MOD, B(3, 0b000), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_TestAccumulator, 0b1010100, 7, { W, DATA, DATA_IF_W } },
    
    
    { Instruction_Or, 0b000010, 6, { D, W, MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_OrImmediate, 0b1000000, 7, { W, MOD, B(3, 0b001), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_OrAccumulator, 0b0000110, 7, { W, DATA, DATA_IF_W } },
    
    { Instruction_Xor, 0b001100, 6, { D, W, MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_XorImmediate, 0b1000000, 7, { W, MOD, B(3, 0b110), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_XorAccumulator, 0b0011010, 7, { W, DATA, DATA_IF_W } },
    
    { Instruction_Rep, 0b1111001, 7, { Z } },
    
    { Instruction_Movs, 0b1010010, 7, { W } },
    
    { Instruction_Cmps, 0b1010011, 7, { W } },
    
    { Instruction_Scas, 0b1010111, 7, { W } },
    
    { Instruction_Lods, 0b1010110, 7, { W } },
    
    { Instruction_Stds, 0b1010101, 7, { W } },
    
    { Instruction_Call, 0b11111111, 8, { MOD, B(3, 0b010), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Jmp, 0b11111111, 8, { MOD, B(3, 0b100), RM, DISP_LO, DISP_HI } },
    
    { Instruction_Ret, 0b11000010, 8, { DATA_LO, DATA_HI } },
    { Instruction_Ret, 0b11000011, 8, { 0 } },
    
    { Instruction_Je,     0b01110100, 8, { IP_INC8 } },
    { Instruction_Jl,     0b01111100, 8, { IP_INC8 } },
    { Instruction_Jle,    0b01111110, 8, { IP_INC8 } },
    { Instruction_Jb,     0b01110010, 8, { IP_INC8 } },
    { Instruction_Jbe,    0b01110110, 8, { IP_INC8 } },
    { Instruction_Jp,     0b01111010, 8, { IP_INC8 } },
    { Instruction_Jo,     0b01110000, 8, { IP_INC8 } },
    { Instruction_Js,     0b01111000, 8, { IP_INC8 } },
    { Instruction_Jne,    0b01110101, 8, { IP_INC8 } },
    { Instruction_Jnl,    0b01111101, 8, { IP_INC8 } },
    { Instruction_Jg,     0b01111111, 8, { IP_INC8 } },
    { Instruction_Jnb,    0b01110011, 8, { IP_INC8 } },
    { Instruction_Ja,     0b01110111, 8, { IP_INC8 } },
    { Instruction_Jnp,    0b01111011, 8, { IP_INC8 } },
    { Instruction_Jno,    0b01110001, 8, { IP_INC8 } },
    { Instruction_Jns,    0b01111001, 8, { IP_INC8 } },
    { Instruction_Loop,   0b11100010, 8, { IP_INC8 } },
    { Instruction_Loopz,  0b11100001, 8, { IP_INC8 } },
    { Instruction_Loopnz, 0b11100000, 8, { IP_INC8 } },
    { Instruction_Jcxz,   0b11100011, 8, { IP_INC8 } },
    { Instruction_Int,    0b11001101, 8, { DATA } },
    { Instruction_Int3,   0b11001100, 8, { 0 } },
    
    { Instruction_Into,   0b11001110, 8, { 0 } },
    { Instruction_Iret,   0b11001111, 8, { 0 } },
    { Instruction_Clc,    0b11111000, 8, { 0 } },
    { Instruction_Cmc,    0b11110101, 8, { 0 } },
    { Instruction_Stc,    0b11111001, 8, { 0 } },
    { Instruction_Cld,    0b11111100, 8, { 0 } },
    { Instruction_Std,    0b11111101, 8, { 0 } },
    { Instruction_Cli,    0b11111010, 8, { 0 } },
    { Instruction_Sti,    0b11111011, 8, { 0 } },
    { Instruction_Hlt,    0b11110100, 8, { 0 } },
    { Instruction_Wait,   0b10011011, 8, { 0 } },
    
    { Instruction_Lock,   0b11110000, 8, { 0 } },
    
    { Instruction_Segment, 0b001, 3, { MOD, B(3, 0b110) } },
#if 0    
    { Instruction_Esc,    0b, 8, { 0 } },
    
    
#endif
    
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
        umm StartingAt = Context->InstructionStreamAt;
        for(u32 TableIndex = 0;
            TableIndex < ArrayCount(GlobalInstructionTable);
            ++TableIndex)
        {
            u8 Op0 = Context->InstructionStream[Context->InstructionStreamAt++];
            
            s8 BitsAt = 7;
            instruction_table_entry TestEntry = GlobalInstructionTable[TableIndex];
            
            if((Op0 >> (8 - TestEntry.OpCodeSize)) != TestEntry.OpCode)
            {
                Context->InstructionStreamAt = StartingAt;
                continue;
            }
            
            BitsAt -= TestEntry.OpCodeSize;
            Result.OpCode = TestEntry.OpCode;
            Result.OpCodeSize = TestEntry.OpCodeSize;
            Result.Type = TestEntry.Type;
            for(u32 FieldIndex = 0;
                FieldIndex < ArrayCount(TestEntry.Fields);
                ++FieldIndex)
            {
                encoding Field = TestEntry.Fields[FieldIndex];
                if(Field.Type == Encoding_None)
                {
                    // NOTE(kstandbridge): No more fields
                    break;
                }
                
                
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
                if(Field.Type == Encoding_DATA_IF_W)
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
                
                if(Field.Type == Encoding_Bits)
                {
                    if(Result.Bits[Field.Type] != Field.Value)
                    {
                        Result.Type = Instruction_NOP;
                        break;
                    }
                }
            }
            
            if(Result.Type != Instruction_NOP)
            {
                break;
            }
            Context->InstructionStreamAt = StartingAt;
        }
        
    }
    
    return Result;
}

inline string
InstructionToAssembly(memory_arena *Arena, simulator_context *Context, instruction Instruction)
{
    string Result;
    format_string_state State = BeginFormatString();
    
    string Op = InstructionToString(Instruction.Type);
    b32 IsWord = ((Instruction.Bits[Encoding_W]) || 
                  (Instruction.Type == Instruction_Lea) || 
                  (Instruction.Type == Instruction_Lds) ||
                  (Instruction.Type == Instruction_Les));
    string Size = (IsWord) ? String("word") : String("byte");
    
    b32 NoFieldData = (Instruction.OpCodeSize == 8);
    if(NoFieldData)
    {
        for(u32 FieldIndex = 0;
            FieldIndex < ArrayCount(Instruction.Bits);
            ++FieldIndex)
        {
            if(Instruction.Bits[FieldIndex] != 0)
            {
                NoFieldData = false;
                break;
            }
        }
    }
    
    if(Context->IsNextOpLock)
    {
        AppendFormatString(&State, "%S ", InstructionToString(Instruction_Lock));
        Context->IsNextOpLock = false;
    }
    
    string SegmentPrefix = String("");
    if(Context->IsNextOpSegment)
    {
        SegmentPrefix = FormatString(Arena, "%S:", SegmentRegisterToString(Context->NextOpSegment));
        Context->IsNextOpSegment = false;
    }
    
    if(NoFieldData)
    {
        AppendFormatString(&State, "%S", Op);
    }
    else if(Instruction.Type == Instruction_Rep)
    {
        instruction SubInstruction = GetNextInstruction(Context);
        if(SubInstruction.Type != Instruction_NOP)
        {
            string SubOp = InstructionToString(SubInstruction.Type);
            string Suffix = (SubInstruction.Bits[Encoding_W]) ? String("w") : String("b");
            AppendFormatString(&State, "%S %S%S", Op, SubOp, Suffix);
        }
        else
        {
            AppendFormatString(&State, "%S ; ERROR expected sub instruction", Op);
        }
    }
    else if((Instruction.Type == Instruction_Je) ||
            (Instruction.Type == Instruction_Jl) ||
            (Instruction.Type == Instruction_Jle) ||
            (Instruction.Type == Instruction_Jb) ||
            (Instruction.Type == Instruction_Jbe) ||
            (Instruction.Type == Instruction_Jp) ||
            (Instruction.Type == Instruction_Jo) ||
            (Instruction.Type == Instruction_Js) ||
            (Instruction.Type == Instruction_Jne) ||
            (Instruction.Type == Instruction_Jnl) ||
            (Instruction.Type == Instruction_Jg) ||
            (Instruction.Type == Instruction_Jnb) ||
            (Instruction.Type == Instruction_Ja) ||
            (Instruction.Type == Instruction_Jnp) ||
            (Instruction.Type == Instruction_Jno) ||
            (Instruction.Type == Instruction_Jns) ||
            (Instruction.Type == Instruction_Loop) ||
            (Instruction.Type == Instruction_Loopz) ||
            (Instruction.Type == Instruction_Loopnz) ||
            (Instruction.Type == Instruction_Jcxz))
    {
        
#if 0
        // NOTE(kstandbridge): We are just given an offset in the instruction stream to jmp
        s8 Value = *(s8 *)&Instruction.Bits[Encoding_IP_INC8];
        AppendFormatString(&State, "%S %d", Op, Value);
#else
        // TODO(kstandbridge): Better testing of jumps to go back to the correct offset
        AppendFormatString(&State, "%S label", Op);
#endif
        
    }
    else if((Instruction.Type == Instruction_Ret))
    {
        u8 ValueLow = Instruction.Bits[Encoding_DATA_LO];
        u8 ValueHigh = Instruction.Bits[Encoding_DATA_HI];
        u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
        s16 Value = *(s16 *)&ValueWide;
        
        AppendFormatString(&State, "%S %d", Op, Value);
    }
    else if((Instruction.Type == Instruction_Aam) ||
            (Instruction.Type == Instruction_Aad) ||
            (Instruction.Type == Instruction_Int)) // TODO(kstandbridge): I have no tested int for this, also test int with value 3
    {
        // NOTE(kstandbridge): Reverse engineering I found the default is 10:
        // amm 255 ; 0b11010100, 0b11111111
        // aam     ; 0b11010100, 0b00001010
        // aam 10  ; 0b11010100, 0b00001010
        // aam 11  ; 0b11010100, 0b00001011
        u8 Value = Instruction.Bits[Encoding_DATA];
        if(Value == 10)
        {
            AppendFormatString(&State, "%S", Op);
        }
        else
        {
            AppendFormatString(&State, "%S %u", Op, Value);
        }
    }
    else if(Instruction.Type == Instruction_MovImmediateMemory)
    {
        s16 Value;
        
        if(Instruction.Bits[Encoding_W])
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
            AppendFormatString(&State, "%S [%S + %d], %S %d", Op, Src, Displacement, Size, Value);
        }
        else
        {
            AppendFormatString(&State, "%S [%S], %S %d", Op, Src, Size, Value);
        }
    }
    else if((Instruction.Type == Instruction_AddAccumulator) ||
            (Instruction.Type == Instruction_AdcAccumulator) ||
            (Instruction.Type == Instruction_SubAccumulator) ||
            (Instruction.Type == Instruction_SbbAccumulator) ||
            (Instruction.Type == Instruction_CmpAccumulator) ||
            (Instruction.Type == Instruction_AndAccumulator) ||
            (Instruction.Type == Instruction_TestAccumulator) ||
            (Instruction.Type == Instruction_OrAccumulator) ||
            (Instruction.Type == Instruction_XorAccumulator))
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
        string Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_RM]) : RegisterByteToString(Instruction.Bits[Encoding_RM]);
        AppendFormatString(&State, "%S %S, %d", Op, Dest, Value);
    } 
    else if((Instruction.Type == Instruction_PushSegmentRegister) ||
            (Instruction.Type == Instruction_PopSegmentRegister))
    {
        AppendFormatString(&State, "%S %S", Op, SegmentRegisterToString(Instruction.Bits[Encoding_MOD]));
    }
    else if(Instruction.Type == Instruction_XchgWithAccumulator)
    {
        AppendFormatString(&State, "%S ax, %S", Op, RegisterWordToString(Instruction.Bits[Encoding_REG]));
    }
    else if((Instruction.Bits[Encoding_ADDR_HI] > 0) ||
            (Instruction.Bits[Encoding_ADDR_LO] > 0))
    {
        u8 ValueLow = Instruction.Bits[Encoding_ADDR_LO];
        u8 ValueHigh = Instruction.Bits[Encoding_ADDR_HI];
        u16 Value = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
        
        string Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
        if(Instruction.Bits[Encoding_D])
        {
            AppendFormatString(&State, "%S [%u], %S", Op, Value, Dest);
        }
        else
        {
            AppendFormatString(&State, "%S %S, [%u]", Op, Dest, Value);
        }
    }
    else if(Instruction.Type == Instruction_In)
    {
        string Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
        if((Instruction.Bits[Encoding_DATA] > 0))
        {
            u8 Value = Instruction.Bits[Encoding_DATA];
            AppendFormatString(&State, "%S %S, %u", Op, Dest, Value);
        }
        else
        {
            AppendFormatString(&State, "%S %S, dx", Op, Dest);
        }
    }
    else if(Instruction.Type == Instruction_Out)
    {
        string Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
        if((Instruction.Bits[Encoding_DATA] > 0))
        {
            u8 Value = Instruction.Bits[Encoding_DATA];
            AppendFormatString(&State, "%S %u, %S", Op, Value, Dest);
        }
        else
        {
            AppendFormatString(&State, "%S dx, %S", Op, Dest);
        }
    }
    else
    {
        switch(Instruction.Bits[Encoding_MOD])
        {
            
            case Mod_RegisterMode:
            {
                string Src = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_RM]) : RegisterByteToString(Instruction.Bits[Encoding_RM]);
                
                if((Instruction.Type == Instruction_MovImmediate) ||
                   (Instruction.Type == Instruction_AddImmediate) ||
                   (Instruction.Type == Instruction_AdcImmediate) ||
                   (Instruction.Type == Instruction_SubImmediate) ||
                   (Instruction.Type == Instruction_SbbImmediate) ||
                   (Instruction.Type == Instruction_CmpImmediate) ||
                   (Instruction.Type == Instruction_TestImmediate))
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
                    
                    string Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_RM]) : RegisterByteToString(Instruction.Bits[Encoding_RM]);
                    AppendFormatString(&State, "%S %S, %d", Op, Dest, Value);
                }
                else
                {                
                    if((Instruction.Type == Instruction_Inc) ||
                       (Instruction.Type == Instruction_Dec) ||
                       (Instruction.Type == Instruction_Neg) ||
                       (Instruction.Type == Instruction_Mul) ||
                       (Instruction.Type == Instruction_Imul) ||
                       (Instruction.Type == Instruction_Div) ||
                       (Instruction.Type == Instruction_Idiv) ||
                       (Instruction.Type == Instruction_Not) ||
                       (Instruction.Type == Instruction_Call) ||
                       (Instruction.Type == Instruction_Jmp))
                    {
                        AppendFormatString(&State, "%S %S", Op, Src);
                    }
                    else if((Instruction.Type == Instruction_Shl) ||
                            (Instruction.Type == Instruction_Shr) ||
                            (Instruction.Type == Instruction_Sar) ||
                            (Instruction.Type == Instruction_Rol) ||
                            (Instruction.Type == Instruction_Ror) ||
                            (Instruction.Type == Instruction_Rcl) ||
                            (Instruction.Type == Instruction_Rcr))
                    {
                        if(Instruction.Bits[Encoding_V])
                        {
                            // NOTE(kstandbridge): V = 1 Shift/rotate count is specified in CL register
                            AppendFormatString(&State, "%S %S, cl", Op, Src);
                        }
                        else
                        {
                            // NOTE(kstandbridge): V = 0 Shift/rotate count is one
                            AppendFormatString(&State, "%S %S, 1", Op, Src);
                        }
                    }
                    else
                    {
                        string Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
                        
                        // NOTE(kstandbridge): xchg requires the direction flip
                        if(Instruction.Type == Instruction_Xchg)
                        {
                            string Temp = Dest;
                            Dest = Src;
                            Src = Temp;
                        } 
                        
                        AppendFormatString(&State, "%S %S, %S", Op, Src, Dest);
                    }
                }
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
                    
                    string Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
                    if((Instruction.Type == Instruction_Mov) ||
                       (Instruction.Type == Instruction_And) ||
                       (Instruction.Type == Instruction_Cmp) ||
                       (Instruction.Type == Instruction_Or) ||
                       (Instruction.Type == Instruction_Xor))
                    {
                        AppendFormatString(&State, "%S %S, %S[%d]", Op, Dest, SegmentPrefix, Displacement);
                    }
                    else if((Instruction.Type == Instruction_Shl) ||
                            (Instruction.Type == Instruction_Shr) ||
                            (Instruction.Type == Instruction_Sar) ||
                            (Instruction.Type == Instruction_Rol) ||
                            (Instruction.Type == Instruction_Ror) ||
                            (Instruction.Type == Instruction_Rcl) ||
                            (Instruction.Type == Instruction_Rcr))
                    {
                        if(Instruction.Bits[Encoding_V])
                        {
                            // NOTE(kstandbridge): V = 1 Shift/rotate count is specified in CL register
                            AppendFormatString(&State, "%S %S [%d], cl", Op, Size, Displacement);
                        }
                        else
                        {
                            // NOTE(kstandbridge): V = 0 Shift/rotate count is one
                            AppendFormatString(&State, "%S %S [%d], 1", Op, Size, Displacement);
                        }
                        
                    }
                    else if((Instruction.Type == Instruction_Xchg))
                    {
                        // TODO(kstandbridge): Figure out why this needs the al suffix
                        AppendFormatString(&State, "%S [%u], al", Op, ValueWide);
                    }
                    else if((Instruction.Type == Instruction_Call) ||
                            (Instruction.Type == Instruction_Jmp))
                    {
                        AppendFormatString(&State, "%S [%u]", Op, ValueWide);
                    }
                    else
                    {
                        AppendFormatString(&State, "%S %S [%d]", Op, Size, Displacement);
                    }
                }
                else
                {
                    string Src = EffectiveAddressToString(Instruction.Bits[Encoding_RM]);
                    
                    if((Instruction.Type == Instruction_MovImmediate) ||
                       (Instruction.Type == Instruction_CmpImmediate) ||
                       (Instruction.Type == Instruction_TestImmediate) )
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
                        
                        if(Instruction.Type == Instruction_MovImmediate)
                        {
                            Src = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
                            AppendFormatString(&State, "%S %S, %d", Op, Src, Value);
                        }
                        else
                        {
                            AppendFormatString(&State, "%S %S [%S], %d", Op, Size, Src, Value);
                        }
                    } 
                    else if((Instruction.Type == Instruction_Mov) ||
                            (Instruction.Type == Instruction_Add) ||
                            (Instruction.Type == Instruction_Adc) ||
                            (Instruction.Type == Instruction_Sub) ||
                            (Instruction.Type == Instruction_Sbb) ||
                            (Instruction.Type == Instruction_Cmp))
                    {                            
                        string Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
                        if(Instruction.Bits[Encoding_D])
                        {
                            AppendFormatString(&State, "%S %S, %S[%S]", Op, Dest, SegmentPrefix, Src);
                        }
                        else
                        {
                            AppendFormatString(&State, "%S [%S], %S", Op, Src, Dest);
                        }
                    }
                    else
                    {
                        if((Instruction.Type == Instruction_PushRegister) ||
                           (Instruction.Type == Instruction_PopRegister) ||
                           (Instruction.Type == Instruction_Inc) ||
                           (Instruction.Type == Instruction_Dec))
                        {
                            string Dest = RegisterWordToString(Instruction.Bits[Encoding_REG]);
                            AppendFormatString(&State, "%S %S", Op, Dest);
                        }
                        else if(Instruction.Type == Instruction_PushSegmentRegister)
                        {
                            string Dest = SegmentRegisterToString(Instruction.Bits[Encoding_MOD]);
                            AppendFormatString(&State, "%S %S", Op, Dest);
                        }
                        else if(Instruction.Type == Instruction_Adc)
                        {
                            AppendFormatString(&State, "%S dx, [%S]", Op, Src);
                        }
                        else if((Instruction.Type == Instruction_Shl) ||
                                (Instruction.Type == Instruction_Shr) ||
                                (Instruction.Type == Instruction_Sar) ||
                                (Instruction.Type == Instruction_Rol) ||
                                (Instruction.Type == Instruction_Ror) ||
                                (Instruction.Type == Instruction_Rcl) ||
                                (Instruction.Type == Instruction_Rcr))
                        {
                            if(Instruction.Bits[Encoding_V])
                            {
                                // NOTE(kstandbridge): V = 1 Shift/rotate count is specified in CL register
                                AppendFormatString(&State, "%S %S [%S], cl", Op, Size, Src);
                            }
                            else
                            {
                                // NOTE(kstandbridge): V = 0 Shift/rotate count is one
                                AppendFormatString(&State, "%S %S [%S], 1", Op, Size, Src);
                            }
                        }
                        else
                        {
                            AppendFormatString(&State, "%S %S [%S]", Op, Size, Src);
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
                    Src = FormatString(Arena, "%S[%S + %d]", SegmentPrefix, EffectiveAddressToString(Instruction.Bits[Encoding_RM]), Value);
                }
                else if(Value < 0)
                {
                    Value *= -1;
                    Src = FormatString(Arena, "%S[%S - %d]", SegmentPrefix, EffectiveAddressToString(Instruction.Bits[Encoding_RM]), Value);
                }
                else
                {
                    Assert(Value == 0);
                    Src = FormatString(Arena, "%S[%S]", SegmentPrefix, EffectiveAddressToString(Instruction.Bits[Encoding_RM]));
                }
                
                string Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
                
                
                // NOTE(kstandbridge): xchg and test seems to require this order on 16 bit displacements but not 8 bit??
                if(((Instruction.Type == Instruction_Xchg) || 
                    (Instruction.Type == Instruction_Test)) &&
                   (Instruction.Bits[Encoding_MOD] == Mod_16BitDisplace))
                {
                    string Temp = Dest;
                    Dest = Src;
                    Src = Temp;
                } 
                else if((Instruction.Bits[Encoding_D]) ||
                        (Instruction.Type == Instruction_Lea) ||
                        (Instruction.Type == Instruction_Lds) ||
                        (Instruction.Type == Instruction_Les))
                {
                    string Temp = Dest;
                    Dest = Src;
                    Src = Temp;
                }
                
                if((Instruction.Type == Instruction_Mov) ||
                   (Instruction.Type == Instruction_Xchg) ||
                   (Instruction.Type == Instruction_Add) ||
                   (Instruction.Type == Instruction_Adc) ||
                   (Instruction.Type == Instruction_Sub) ||
                   (Instruction.Type == Instruction_Sbb) ||
                   (Instruction.Type == Instruction_Cmp) ||
                   (Instruction.Type == Instruction_And) ||
                   (Instruction.Type == Instruction_Lea) ||
                   (Instruction.Type == Instruction_Lds) ||
                   (Instruction.Type == Instruction_Les) ||
                   (Instruction.Type == Instruction_Test) ||
                   (Instruction.Type == Instruction_Or) ||
                   (Instruction.Type == Instruction_Xor))
                {
                    AppendFormatString(&State, "%S %S, %S", Op, Src, Dest);
                }
                else if((Instruction.Type == Instruction_Shl) ||
                        (Instruction.Type == Instruction_Shr) ||
                        (Instruction.Type == Instruction_Sar) ||
                        (Instruction.Type == Instruction_Rol) ||
                        (Instruction.Type == Instruction_Ror) ||
                        (Instruction.Type == Instruction_Rcl) ||
                        (Instruction.Type == Instruction_Rcr))
                {
                    
                    if(Instruction.Bits[Encoding_V])
                    {
                        // NOTE(kstandbridge): V = 1 Shift/rotate count is specified in CL register
                        AppendFormatString(&State, "%S %S %S, cl", Op, Size, Src);
                    }
                    else
                    {
                        // NOTE(kstandbridge): V = 0 Shift/rotate count is one
                        AppendFormatString(&State, "%S %S %S, 1", Op, Size, Src);
                    }
                }
                else if((Instruction.Type == Instruction_AndImmediate) ||
                        (Instruction.Type == Instruction_OrImmediate) ||
                        (Instruction.Type == Instruction_TestImmediate) ||
                        (Instruction.Type == Instruction_SbbImmediate) ||
                        (Instruction.Type == Instruction_XorImmediate))
                {
                    u16 Data;
                    if(Instruction.Bits[Encoding_W])
                    {
                        u8 ValueLow = Instruction.Bits[Encoding_DATA];
                        u8 ValueHigh = Instruction.Bits[Encoding_DATA_IF_W];
                        Data = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                    }
                    else
                    {
                        Data = *(u8 *)&Instruction.Bits[Encoding_DATA];
                    }
                    
                    AppendFormatString(&State, "%S %S %S, %u", Op, Size, Src, Data);
                }
                else if(Instruction.Type == Instruction_Call)
                {
                    AppendFormatString(&State, "%S %S", Op, Src);
                }
                else
                {
                    AppendFormatString(&State, "%S %S %S", Op, Size, Src);
                }
                
            } break;
            
            default:
            {
                AppendFormatString(&State, "Invalid MOD");
            } break;
            
        }
    }
    
    
    
    Result = EndFormatString(&State, Arena);
    return Result;
}

internal string
StreamToAssembly(memory_arena *Arena, u8 *Buffer, umm Size)
{
    string Result;
    format_string_state StringState = BeginFormatString();
    
    simulator_context Context = 
    {
        .InstructionStream = Buffer,
        .InstructionStreamAt = 0,
        .InstructionStreamSize = Size
    };
    
    b32 First = false;
    for(;;)
    {
        instruction Instruction = GetNextInstruction(&Context);
        
        
        // TODO(kstandbridge): Update the context with this instruction
        if(Instruction.Type == Instruction_Lock)
        {
            Context.IsNextOpLock = true;
        }
        else if(Instruction.Type == Instruction_Segment)
        {
            Context.IsNextOpSegment = true;
            Context.NextOpSegment = Instruction.Bits[Encoding_MOD];
        }
        else if(Instruction.Type != Instruction_NOP)
        {
            string Assembly = InstructionToAssembly(Arena, &Context, Instruction);
            
            if(!First)
            {
                First = true;
            }
            else
            {
                AppendFormatString(&StringState, "\n");
            }
            
            AppendFormatString(&StringState, "%S", Assembly);
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