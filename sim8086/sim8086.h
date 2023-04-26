#ifndef SIM8086_H

typedef enum register_byte_type
{
    RegisterByte_AL = 0b000,
    RegisterByte_CL = 0b001,
    RegisterByte_DL = 0b010,
    RegisterByte_BL = 0b011,
    RegisterByte_AH = 0b100,
    RegisterByte_CH = 0b101,
    RegisterByte_DH = 0b110,
    RegisterByte_BH = 0b111,
} register_byte_type;

typedef enum register_word_type
{
    RegisterWord_AX = 0b000,
    RegisterWord_CX = 0b001,
    RegisterWord_DX = 0b010,
    RegisterWord_BX = 0b011,
    RegisterWord_SP = 0b100,
    RegisterWord_BP = 0b101,
    RegisterWord_SI = 0b110,
    RegisterWord_DI = 0b111,
    
    RegisterWord_Count,
} register_word_type;

typedef enum segment_register_type
{
    SegmentRegister_ES = 0b00,
    SegmentRegister_CS = 0b01,
    SegmentRegister_SS = 0b10,
    SegmentRegister_DS = 0b11,
    
    SegmentRegister_Count,
} segment_register_type;

typedef enum register_flags
{
    Flag_CF = (1 << 0),
    Flag_PF = (1 << 1),
    Flag_AF = (1 << 2),
    Flag_ZF = (1 << 3),
    Flag_SF = (1 << 4),
    Flag_TF = (1 << 5),
    Flag_IF = (1 << 6),
    Flag_DF = (1 << 7),
    Flag_OF = (1 << 8),
} register_flags;

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

typedef struct simulator_context
{
    b32 IsNextOpLock;
    b32 IsNextOpSegment;
    u8 NextOpSegment;
    
    u8 *InstructionStream;
    umm InstructionStreamAt;
    umm InstructionStreamSize;
    
    u16 Registers[RegisterWord_Count];
    u16 SegmentRegisters[SegmentRegister_Count];
    
    register_flags Flags;
    
} simulator_context;

typedef enum encoding_type
{
    Encoding_None,
    
    Encoding_MOD,
    Encoding_REG,
    Encoding_RM,
    
    Encoding_DISP_LO,
    Encoding_DISP_HI,
    
    Encoding_DATA,
    Encoding_DATA_IF_W,
    
    Encoding_DATA_LO,
    Encoding_DATA_HI,
    
    Encoding_IP_INC8,
    
    Encoding_CS_LO,
    Encoding_CS_HI,
    
    Encoding_Count,
} encoding_type;

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

typedef struct encoding
{
    encoding_type Type;
    u8 Size;
    u8 Value;
} encoding;

typedef enum instruction_type
{
    Instruction_NOP,
    
    // NOTE(kstandbridge): Categories
    Instruction_Immediate,
    Instruction_Logic,
    Instruction_Arithmetic,
    Instruction_Control,
    
    // NOTE(kstandbridge): Ops
    Instruction_Mov,
    Instruction_MovImmediateMemory,
    Instruction_MovImmediate,
    Instruction_MovRegisterSegment,
    
    Instruction_PushRegister,
    Instruction_PushSegmentRegister,
    
    Instruction_Pop,
    Instruction_PopRegister,
    Instruction_PopSegmentRegister,
    
    Instruction_Xchg,
    Instruction_XchgWithAccumulator,
    
    Instruction_In,
    
    Instruction_Out,
    
    Instruction_Xlat,
    
    Instruction_Lea,
    
    Instruction_Lds,
    
    Instruction_Les,
    
    Instruction_Lahf,
    
    Instruction_Sahf,
    
    Instruction_Pushf,
    
    Instruction_Popf,
    
    Instruction_Add,
    Instruction_AddAccumulator,
    
    Instruction_Adc,
    Instruction_AdcAccumulator,
    
    Instruction_Inc,
    
    Instruction_Aaa,
    
    Instruction_Daa,
    
    Instruction_Sub,
    Instruction_SubAccumulator,
    
    Instruction_Sbb,
    Instruction_SbbAccumulator,
    
    Instruction_Dec,
    
    Instruction_Cmp,
    Instruction_CmpAccumulator,
    
    Instruction_Aas,
    
    Instruction_Das,
    
    Instruction_Aam,
    
    Instruction_Aad,
    
    Instruction_Cbw,
    
    Instruction_Cwd,
    
    Instruction_And,
    Instruction_AndAccumulator,
    
    Instruction_Test,
    Instruction_TestAccumulator,
    
    Instruction_Or,
    Instruction_OrAccumulator,
    
    Instruction_Xor,
    Instruction_XorAccumulator,
    
    Instruction_Rep,
    
    Instruction_Movs,
    
    Instruction_Cmps,
    
    Instruction_Scas,
    
    Instruction_Lods,
    
    Instruction_Stds,
    
    Instruction_Call,
    Instruction_CallDirectIntersegment,
    Instruction_CallDirectWithin,
    
    Instruction_Jmp,
    Instruction_JmpDirectIntersegment,
    Instruction_JmpDirectWithin,
    
    Instruction_Ret,
    
    Instruction_Retf,
    Instruction_RetfIntersegment,
    
    Instruction_Je,
    
    Instruction_Jl,
    
    Instruction_Jle,
    
    Instruction_Jb,
    
    Instruction_Jbe,
    
    Instruction_Jp,
    
    Instruction_Jo,
    
    Instruction_Js,
    
    Instruction_Jne,
    
    Instruction_Jnl,
    
    Instruction_Jg,
    
    Instruction_Jnb,
    
    Instruction_Ja,
    
    Instruction_Jnp,
    
    Instruction_Jno,
    
    Instruction_Jns,
    
    Instruction_Loop,
    
    Instruction_Loopz,
    
    Instruction_Loopnz,
    
    Instruction_Jcxz,
    
    Instruction_Int,
    
    Instruction_Int3,
    
    Instruction_Into,
    
    Instruction_Iret,
    
    Instruction_Clc,
    
    Instruction_Cmc,
    
    Instruction_Stc,
    
    Instruction_Cld,
    
    Instruction_Std,
    
    Instruction_Cli,
    
    Instruction_Sti,
    
    Instruction_Hlt,
    
    Instruction_Wait,
    
    Instruction_Lock,
    
    Instruction_Segment,
    
} instruction_type;

typedef enum sub_op_type
{
    // NOTE(kstandbridge): Immediate
    SubOp_Add = 0b000,
    SubOp_Or  = 0b001,
    SubOp_Adc = 0b010,
    SubOp_Sbb = 0b011,
    SubOp_And = 0b100,
    SubOp_Sub = 0b101,
    SubOp_Xor = 0b110,
    SubOp_Cmp = 0b111,
    
    // NOTE(kstandbridge): Logic
    SubOp_Rol    = 0b000,
    SubOp_Ror    = 0b001,
    SubOp_Rcl    = 0b010,
    SubOp_Rcr    = 0b011,
    SubOp_Shl    = 0b100,
    SubOp_Shr    = 0b101,
    //SubOp_     = 0b110,
    SubOp_Sar    = 0b111,
    
    // NOTE(kstandbridge): Arithmetic
    SubOp_Test   = 0b000,
    //SubOp_     = 0b001,
    SubOp_Not    = 0b010,
    SubOp_Neg    = 0b011,
    SubOp_Mul    = 0b100,
    SubOp_IMul   = 0b101,
    SubOp_Div    = 0b110,
    SubOp_IDiv   = 0b111,
    
    // NOTE(kstandbridge): Control
    SubOp_Inc    = 0b000,
    SubOp_Dec    = 0b001,
    SubOp_Call   = 0b010,
    SubOp_ICall  = 0b011,
    SubOp_Jmp    = 0b100,
    SubOp_IJmp   = 0b101,
    SubOp_Push   = 0b110,
    //SubOp_     = 0b111,
    
} sub_op_type;

typedef enum instruction_flags
{
    Flag_S = (1 << 0),
    Flag_W = (1 << 1),
    Flag_D = (1 << 2),
    Flag_V = (1 << 3),
    Flag_Z = (1 << 4),
    
} instruction_flags;

typedef struct instruction
{
    instruction_type Type;
    
    instruction_flags Flags;
    
    union
    {    
        u32 Generic;
        
        register_byte_type RegisterByte;
        register_word_type RegisterWord;
        segment_register_type SegmentRegister;
    };
    
    u8 Bits[Encoding_Count];
    
} instruction;

inline string
InstructionToString(instruction Instruction)
{
    string Result;
    switch(Instruction.Type)
    {
        
        case Instruction_Immediate:
        {
            switch(Instruction.Bits[Encoding_REG])
            {
                case SubOp_Add: { Result = String("add"); } break;
                case SubOp_Or:  { Result = String("or");  } break;
                case SubOp_Adc: { Result = String("adc"); } break;
                case SubOp_Sbb: { Result = String("sbb"); } break;
                case SubOp_And: { Result = String("and"); } break;
                case SubOp_Sub: { Result = String("sub"); } break;
                case SubOp_Xor: { Result = String("xor"); } break;
                case SubOp_Cmp: { Result = String("cmp"); } break;
                default: { Result = String("; invalid encoding type"); } break;
            }
            
        } break;
        
        case Instruction_Logic:
        {
            switch(Instruction.Bits[Encoding_REG])
            {
                case SubOp_Rol:    { Result = String("rol"); } break;
                case SubOp_Ror:    { Result = String("ror"); } break;
                case SubOp_Rcl:    { Result = String("rcl"); } break;
                case SubOp_Rcr:    { Result = String("rcr"); } break;
                case SubOp_Shl:    { Result = String("shl"); } break;
                case SubOp_Shr:    { Result = String("shr"); } break;
                //case SubOp_:     { Result = String(""); } break;
                case SubOp_Sar:    { Result = String("sar"); } break;
                default: { Result = String("; invalid encoding type"); } break;
            }
            
        } break;
        
        case Instruction_Arithmetic:
        {
            switch(Instruction.Bits[Encoding_REG])
            {
                case SubOp_Test:   { Result = String("test"); }; break;
                //case SubOp_:     { Result = String(""); } break;
                case SubOp_Not:    { Result = String("not"); }; break;
                case SubOp_Neg:    { Result = String("neg"); }; break;
                case SubOp_Mul:    { Result = String("mul"); }; break;
                case SubOp_IMul:   { Result = String("imul"); }; break;
                case SubOp_Div:    { Result = String("div"); }; break;
                case SubOp_IDiv:   { Result = String("idiv"); }; break;
                default: { Result = String("; invalid encoding type"); } break;
            }
        } break;
        
        case Instruction_Control:
        {
            switch(Instruction.Bits[Encoding_REG])
            {
                case SubOp_Inc:   { Result = String("inc"); }; break;
                case SubOp_Dec:   { Result = String("dec"); }; break;
                case SubOp_Call:  { Result = String("call"); }; break;
                case SubOp_ICall: { Result = String("call"); }; break;
                case SubOp_Jmp:   { Result = String("jmp"); }; break;
                case SubOp_IJmp:  { Result = String("jmp"); }; break;
                case SubOp_Push:  { Result = String("push"); }; break;
                //case SubOp_     = 0b111,
                default: { Result = String("; invalid encoding type"); } break;
            }
        } break;
        
        
        case Instruction_MovImmediateMemory:
        case Instruction_MovImmediate:
        case Instruction_MovRegisterSegment:
        case Instruction_Mov:  { Result = String("mov"); } break;
        
        case Instruction_PushRegister:
        case Instruction_PushSegmentRegister: { Result = String("push"); } break;
        
        case Instruction_PopRegister:
        case Instruction_PopSegmentRegister:
        case Instruction_Pop:  { Result = String("pop"); } break;
        
        case Instruction_XchgWithAccumulator:
        case Instruction_Xchg: { Result = String("xchg"); } break;
        
        case Instruction_In:   { Result = String("in"); } break;
        
        case Instruction_Out:  { Result = String("out"); } break;
        
        case Instruction_Xlat: { Result = String("xlat"); } break;
        
        case Instruction_Lea: { Result = String("lea"); } break;
        
        case Instruction_Lds: { Result = String("lds"); } break;
        
        case Instruction_Les: { Result = String("les"); } break;
        
        case Instruction_Lahf: { Result = String("lahf"); } break;
        
        case Instruction_Sahf: { Result = String("sahf"); } break;
        
        case Instruction_Pushf: { Result = String("pushf"); } break;
        
        case Instruction_Popf: { Result = String("popf"); } break;
        
        case Instruction_AddAccumulator:
        case Instruction_Add: { Result = String("add"); } break;
        
        case Instruction_AdcAccumulator:
        case Instruction_Adc: { Result = String("adc"); } break;
        
        case Instruction_Inc: { Result = String("inc"); } break;
        
        case Instruction_Aaa: { Result = String("aaa"); } break;
        
        case Instruction_Daa: { Result = String("daa"); } break;
        
        case Instruction_SubAccumulator:
        case Instruction_Sub: { Result = String("sub"); } break;
        
        case Instruction_SbbAccumulator:
        case Instruction_Sbb: { Result = String("sbb"); } break;
        
        case Instruction_Dec: { Result = String("dec"); } break;
        
        case Instruction_CmpAccumulator:
        case Instruction_Cmp: { Result = String("cmp"); } break;
        
        case Instruction_Aas: { Result = String("aas"); } break;
        
        case Instruction_Das: { Result = String("das"); } break;
        
        case Instruction_Aam: { Result = String("aam"); } break;
        
        case Instruction_Aad: { Result = String("aad"); } break;
        
        case Instruction_Cbw: { Result = String("cbw"); } break;
        
        case Instruction_Cwd: { Result = String("cwd"); } break;
        
        case Instruction_AndAccumulator:
        case Instruction_And: { Result = String("and"); } break;
        
        case Instruction_TestAccumulator:
        case Instruction_Test: { Result = String("test"); } break;
        
        case Instruction_OrAccumulator:
        case Instruction_Or: { Result = String("or"); } break;
        
        case Instruction_XorAccumulator:
        case Instruction_Xor: { Result = String("xor"); } break;
        
        case Instruction_Rep: { Result = String("rep"); } break;
        
        case Instruction_Movs: { Result = String("movs"); } break;
        
        case Instruction_Cmps: { Result = String("cmps"); } break;
        
        case Instruction_Scas: { Result = String("scas"); } break;
        
        case Instruction_Lods: { Result = String("lods"); } break;
        
        case Instruction_Stds: { Result = String("stos"); } break;
        
        case Instruction_CallDirectWithin:
        case Instruction_CallDirectIntersegment:
        case Instruction_Call: { Result = String("call"); } break;
        
        case Instruction_JmpDirectIntersegment:
        case Instruction_JmpDirectWithin:
        case Instruction_Jmp: { Result = String("jmp"); } break;
        
        case Instruction_Ret: { Result = String("ret"); } break;
        
        case Instruction_Retf:
        case Instruction_RetfIntersegment: { Result = String("retf"); } break;
        
        case Instruction_Je: { Result = String("je"); } break;
        
        case Instruction_Jl: { Result = String("jl"); } break;
        
        case Instruction_Jle: { Result = String("jle"); } break;
        
        case Instruction_Jb: { Result = String("jb"); } break;
        
        case Instruction_Jbe: { Result = String("jbe"); } break;
        
        case Instruction_Jp: { Result = String("jp"); } break;
        
        case Instruction_Jo: { Result = String("jo"); } break;
        
        case Instruction_Js: { Result = String("js"); } break;
        
        case Instruction_Jne: { Result = String("jne"); } break;
        
        case Instruction_Jnl: { Result = String("jnl"); } break;
        
        case Instruction_Jg: { Result = String("jg"); } break;
        
        case Instruction_Jnb: { Result = String("jnb"); } break;
        
        case Instruction_Ja: { Result = String("ja"); } break;
        
        case Instruction_Jnp: { Result = String("jnp"); } break;
        
        case Instruction_Jno: { Result = String("jno"); } break;
        
        case Instruction_Jns: { Result = String("jns"); } break;
        
        case Instruction_Loop: { Result = String("loop"); } break;
        
        case Instruction_Loopz: { Result = String("loopz"); } break;
        
        case Instruction_Loopnz: { Result = String("loopnz"); } break;
        
        case Instruction_Jcxz: { Result = String("jcxz"); } break;
        
        case Instruction_Int: { Result = String("int"); } break;
        
        case Instruction_Int3: { Result = String("int3"); } break;
        
        case Instruction_Into: { Result = String("into"); } break;
        
        case Instruction_Iret: { Result = String("iret"); } break;
        
        case Instruction_Clc: { Result = String("clc"); } break;
        
        case Instruction_Cmc: { Result = String("cmc"); } break;
        
        case Instruction_Stc: { Result = String("stc"); } break;
        
        case Instruction_Cld: { Result = String("cld"); } break;
        
        case Instruction_Std: { Result = String("std"); } break;
        
        case Instruction_Cli: { Result = String("cli"); } break;
        
        case Instruction_Sti:  { Result = String("sti"); } break;
        
        case Instruction_Hlt:  { Result = String("hlt"); } break;
        
        case Instruction_Wait: { Result = String("wait"); } break;
        
        default: { Result = String("; Invalid instruction"); } break;
    }
    return Result;
}

typedef struct instruction_table_entry
{
    instruction_type Type;
    u8 OpCode;
    
    instruction_flags Flags;
    
    union
    {    
        u32 Generic;
        
        register_byte_type RegisterByte;
        register_word_type RegisterWord;
        segment_register_type SegmentRegister;
    };
    
    encoding Fields[7];
} instruction_table_entry;


#define SIM8086_H
#endif //SIM8086_H
