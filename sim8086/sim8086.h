#ifndef SIM8086_H


typedef struct simulator_context
{
    u8 *InstructionStream;
    umm InstructionStreamAt;
    umm InstructionStreamSize;
    
} simulator_context;

typedef enum encoding_type
{
    Encoding_NOP,
    
    Encoding_Bits,
    
    Encoding_MOD,
    Encoding_REG,
    Encoding_RM,
    
    Encoding_S,
    Encoding_W,
    Encoding_D,
    Encoding_V,
    
    Encoding_DISP_LO,
    Encoding_DISP_HI,
    
    Encoding_DATA,
    Encoding_DATA_IF_W,
    
    Encoding_ADDR_LO,
    Encoding_ADDR_HI,
    
    Encoding_Count,
} encoding_type;

typedef enum register_byte_type
{
    RegisterByte_AL,
    RegisterByte_CL,
    RegisterByte_DL,
    RegisterByte_BL,
    RegisterByte_AH,
    RegisterByte_CH,
    RegisterByte_DH,
    RegisterByte_BH,
} register_byte_type;

inline string
RegisterByteToString(register_byte_type Type)
{
    string Result;
    switch(Type)
    {
        case RegisterByte_AL: { Result = String("al"); } break;
        case RegisterByte_CL: { Result = String("cl"); } break;
        case RegisterByte_DL: { Result = String("dl"); } break;
        case RegisterByte_BL: { Result = String("bl"); } break;
        case RegisterByte_AH: { Result = String("ah"); } break;
        case RegisterByte_CH: { Result = String("ch"); } break;
        case RegisterByte_DH: { Result = String("dh"); } break;
        case RegisterByte_BH: { Result = String("bh"); } break;
        default: { Result = String("; Invalid register byte"); } break;
    }
    return Result;
}

typedef enum register_word_type
{
    RegisterWord_AX,
    RegisterWord_CX,
    RegisterWord_DX,
    RegisterWord_BX,
    RegisterWord_SP,
    RegisterWord_BP,
    RegisterWord_SI,
    RegisterWord_DI,
} register_word_type;

inline string
RegisterWordToString(register_word_type Type)
{
    string Result;
    switch(Type)
    {
        case RegisterWord_AX: { Result = String("ax"); } break;
        case RegisterWord_CX: { Result = String("cx"); } break;
        case RegisterWord_DX: { Result = String("dx"); } break;
        case RegisterWord_BX: { Result = String("bx"); } break;
        case RegisterWord_SP: { Result = String("sp"); } break;
        case RegisterWord_BP: { Result = String("bp"); } break;
        case RegisterWord_SI: { Result = String("si"); } break;
        case RegisterWord_DI: { Result = String("di"); } break;
        default: { Result = String("; Invalid register word"); } break;
    }
    return Result;
}

typedef enum mod_type
{
    Mod_MemoryMode =    0b00,
    Mod_8BitDisplace =  0b01,
    Mod_16BitDisplace = 0b10,
    Mod_RegisterMode  = 0b11,
} mod_type;

typedef enum effective_address_type
{
    EffectiveAddress_BX_SI         = 0b000,
    EffectiveAddress_BX_DI         = 0b001,
    EffectiveAddress_BP_SI         = 0b010,
    EffectiveAddress_BP_DI         = 0b011,
    EffectiveAddress_SI            = 0b100,
    EffectiveAddress_DI            = 0b101,
    EffectiveAddress_DirectAddress = 0b110,
    EffectiveAddress_BP            = 0b110,
    EffectiveAddress_BX            = 0b111,
} effective_address_type;

inline string
EffectiveAddressToString(effective_address_type Type)
{
    string Result;
    switch(Type)
    {
        case EffectiveAddress_BX_SI: { Result = String("bx + si"); } break;
        case EffectiveAddress_BX_DI: { Result = String("bx + di"); } break;
        case EffectiveAddress_BP_SI: { Result = String("bp + si"); } break;
        case EffectiveAddress_BP_DI: { Result = String("bp + di"); } break;
        case EffectiveAddress_SI:    { Result = String("si"); } break;
        case EffectiveAddress_DI:    { Result = String("di"); } break;
        case EffectiveAddress_BP:    { Result = String("bp"); } break;
        case EffectiveAddress_BX:    { Result = String("bx"); } break;
        default: { Result = String("; Invalid effective address"); } break;
    }
    return Result;
}

typedef enum segment_register_type
{
    SegmentRegister_ES = 0b00,
    SegmentRegister_CS = 0b01,
    SegmentRegister_SS = 0b10,
    SegmentRegister_DS = 0b11,
} segment_register_type;

inline string
SegmentRegisterToString(segment_register_type Type)
{
    string Result;
    switch(Type)
    {
        case SegmentRegister_ES: { Result = String("es"); } break;
        case SegmentRegister_CS: { Result = String("cs"); } break;
        case SegmentRegister_SS: { Result = String("ss"); } break;
        case SegmentRegister_DS: { Result = String("ds"); } break;
        default: { Result = String("; Invalid segment register"); } break;
    }
    return Result;
}

typedef struct encoding
{
    encoding_type Type;
    u8 Size;
    u8 Value;
} encoding;

typedef enum instruction_type
{
    Instruction_NOP,
    
    Instruction_Mov,
    Instruction_MovImmediate,
    Instruction_Push,
    Instruction_PushRegister,
    Instruction_PushSegmentRegister,
} instruction_type;

inline string
InstructionToString(instruction_type Type)
{
    string Result;
    switch(Type)
    {
        case Instruction_MovImmediate:
        case Instruction_Mov:  { Result = String("mov"); } break;
        case Instruction_PushRegister:
        case Instruction_PushSegmentRegister:
        case Instruction_Push: { Result = String("push"); } break;
        default: { Result = String("; Invalid instruction"); } break;
    }
    return Result;
}

typedef struct instruction
{
    instruction_type Type;
    u8 OpCode;
    u8 Bits[Encoding_Count];
    
} instruction;

typedef struct instruction_table_entry
{
    instruction_type Type;
    u8 OpCode;
    u8 OpCodeSize;
    encoding Fields[8];
} instruction_table_entry;


#define SIM8086_H
#endif //SIM8086_H
