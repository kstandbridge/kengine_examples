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

typedef enum op_code
{
    OpCode_ProcessorControlSetCarry          = 0b11111001,
    OpCode_ProcessorControlSetDirection      = 0b11111101,
    OpCode_ProcessorControlClearCarry        = 0b11111000,
    OpCode_ProcessorControlCompletementCarry = 0b11110101,
    OpCode_ProcessorControlClearDirection    = 0b11111100,
    OpCode_ProcessorControlClearInterrupt    = 0b11111010,
    OpCode_ProcessorControlSetInterrupt      = 0b11111011,
    OpCode_ProcessorControlHalt              = 0b11110100,
    OpCode_ProcessorControlWait              = 0b10011011,
    
    OpCode_InterruptOnOverflow               = 0b11001110,
    OpCode_InterruptReturn                   = 0b11001111,
    OpCode_InterruptTypeSpecified            = 0b11001101,
    OpCode_InterruptType3                    = 0b11001100,
    
    OpCode_ReturnWithinSegment               = 0b11000011,
    OpCode_ReturnWithinSegAddingImmedToSP    = 0b11000010,
    OpCode_ReturnJumpOnEqualZero             = 0b01110100,
    OpCode_ReturnJumpOnLessNotGreaterOrEqual = 0b01111100,
    OpCode_ReturnJumpOnLessOrEqualNotGreater = 0b01111110,
    OpCode_ReturnJumpOnBelowNotAboveOrEqual  = 0b01110010,
    OpCode_ReturnJumpOnBelowOrEqualNotAbove  = 0b01110110,
    OpCode_ReturnJumpOnParityEven            = 0b01111010,
    OpCode_ReturnJumpOnOverflow              = 0b01110000,
    OpCode_ReturnJumpOnSign                  = 0b01111000,
    OpCode_ReturnJumpOnNotEqualNotZero       = 0b01110101,
    OpCode_ReturnJumpOnNotLessGreaterOrEqual = 0b01111101,
    OpCode_ReturnJumpOnNotLessOrEqualGreater = 0b01111111,
    OpCode_ReturnJumpOnNotBelowAboveOrEqual  = 0b01110011,
    OpCode_ReturnJumpOnNotBelowOrEqualAbove  = 0b01110111,
    OpCode_ReturnJumpOnNotParOdd             = 0b01111011,
    OpCode_ReturnJumpOnNotOverflow           = 0b01110001,
    OpCode_ReturnJumpOnNotSign               = 0b01111001,
    OpCode_ReturnLoopCXTimes                 = 0b11100010,
    OpCode_ReturnLoopWhileZeroEqual          = 0b11100001,
    OpCode_ReturnLoopWhileNotZeroEqual       = 0b11100000,
    OpCode_ReturnJumpOnCXZero                = 0b11100011,
    
    OpCode_IncrementASCIIAdjustForAdd        = 0b00110111,
    OpCode_IncrementDecimalAdjustForAdd      = 0b00100111,
    
    OpCode_CompareASCIIAdjustForSubtract     = 0b00111111,
    OpCode_CompareDecimalAdjustForSubtract   = 0b00101111,
    OpCode_CompareConvertByteToWord          = 0b10011000,
    OpCode_CompareConvertWordToDoubleWord    = 0b10011001,
    OpCode_CompareASCIIAdjustForMultiply     = 0b11010100,
    OpCode_CompareASCIIAdjustForDivide       = 0b11010101,
    
    OpCode_OutTranslateByteToAL              = 0b11010111,
    OpCode_OutLoadAHWithFlags                = 0b10011111,
    OpCode_OutStoreAHIntoFlags               = 0b10011110,
    OpCode_OutPushFlags                      = 0b10011100,
    OpCode_OutPopFlags                       = 0b10011101,
    
    OpCodeStringRepeat                       = 0b1111001,
    OpCodeStringMoveByteWord                 = 0b1010010,
    OpCodeStringCompareByteWord              = 0b1010011,
    OpCodeStringScanByteWord                 = 0b1010111,
    OpCodeStringLoadByteWdToALXL             = 0b1010110,
    OpCodeStringStoreByteWdFromALXL          = 0b1010101,
    
    OpInFixedPort                            = 0b1110010,
    OpInVariablePort                         = 0b1110110,
    
    OpOutFixedPort                           = 0b1110011,
    OpOutVariablePort                        = 0b1110111,
    
    OpPushRegister                           = 0b01010,
    OpPopRegister                            = 0b01011,
    
    OpXchgRegisterWithMemory                 = 0b10010,
    
    OpIncRegister                            = 0b01000,
    
    OpDecRegister                            = 0b01001,
    
} op_code;

inline string
Parse(memory_arena *Arena, u8 *At, umm Size)
{
    format_string_state State = BeginFormatString();
    for(umm Index = 0;
        Index < Size;)
    {
        u8 Op0 = At[Index++];
        
        string Label = String("label");
        string SegmentRegisterCode[] = { String("es"), String("cs"), String("ss"), String("ds") };
        
        switch(Op0)
        {
            case OpCode_ProcessorControlSetCarry:          { AppendFormatString(&State, "stc"); } break;
            case OpCode_ProcessorControlSetDirection:      { AppendFormatString(&State, "std"); } break;
            case OpCode_ProcessorControlClearCarry:        { AppendFormatString(&State, "clc"); } break;
            case OpCode_ProcessorControlCompletementCarry: { AppendFormatString(&State, "cmc"); } break;
            case OpCode_ProcessorControlClearDirection:    { AppendFormatString(&State, "cld"); } break;
            case OpCode_ProcessorControlClearInterrupt:    { AppendFormatString(&State, "cli"); } break;
            case OpCode_ProcessorControlSetInterrupt:      { AppendFormatString(&State, "sti"); } break;
            case OpCode_ProcessorControlHalt:              { AppendFormatString(&State, "hlt"); } break;
            case OpCode_ProcessorControlWait:              { AppendFormatString(&State, "wait"); } break;
            
            case OpCode_InterruptOnOverflow:               { AppendFormatString(&State, "into"); } break;
            case OpCode_InterruptReturn:                   { AppendFormatString(&State, "iret"); } break;
            case OpCode_InterruptType3:                    { AppendFormatString(&State, "int3"); } break;
            case OpCode_InterruptTypeSpecified:
            { 
                u8 Value = At[Index++];
                AppendFormatString(&State, "int %d", Value);
            } break;
            
            case OpCode_ReturnWithinSegment:               { AppendFormatString(&State, "ret"); } break;
            case OpCode_ReturnWithinSegAddingImmedToSP:
            {
                u8 ValueLow = At[Index++];
                u8 ValueHigh = At[Index++];
                u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                
                s16 Value = *(s16 *)&ValueWide;
                AppendFormatString(&State, "ret %d", Value);
            } break;
            case OpCode_ReturnJumpOnEqualZero:             { AppendFormatString(&State, "je %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnLessNotGreaterOrEqual: { AppendFormatString(&State, "jl %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnLessOrEqualNotGreater: { AppendFormatString(&State, "jle %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnBelowNotAboveOrEqual:  { AppendFormatString(&State, "jb %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnBelowOrEqualNotAbove:  { AppendFormatString(&State, "jbe %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnParityEven:            { AppendFormatString(&State, "jp %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnOverflow:              { AppendFormatString(&State, "jo %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnSign:                  { AppendFormatString(&State, "js %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnNotEqualNotZero:       { AppendFormatString(&State, "jne %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnNotLessGreaterOrEqual: { AppendFormatString(&State, "jnl %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnNotLessOrEqualGreater: { AppendFormatString(&State, "jg %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnNotBelowAboveOrEqual:  { AppendFormatString(&State, "jnb %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnNotBelowOrEqualAbove:  { AppendFormatString(&State, "ja %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnNotParOdd:             { AppendFormatString(&State, "jnp %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnNotOverflow:           { AppendFormatString(&State, "jno %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnNotSign:               { AppendFormatString(&State, "jns %S", Label); Index++; } break;
            case OpCode_ReturnLoopCXTimes:                 { AppendFormatString(&State, "loop %S", Label); Index++; } break;
            case OpCode_ReturnLoopWhileZeroEqual:          { AppendFormatString(&State, "loopz %S", Label); Index++; } break;
            case OpCode_ReturnLoopWhileNotZeroEqual:       { AppendFormatString(&State, "loopnz %S", Label); Index++; } break;
            case OpCode_ReturnJumpOnCXZero:                { AppendFormatString(&State, "jcxz %S", Label); Index++; } break;
            
            case OpCode_IncrementASCIIAdjustForAdd:        { AppendFormatString(&State, "aaa"); } break;
            case OpCode_IncrementDecimalAdjustForAdd:      { AppendFormatString(&State, "daa"); } break;
            
            case OpCode_CompareASCIIAdjustForSubtract:     { AppendFormatString(&State, "aas"); } break;
            case OpCode_CompareDecimalAdjustForSubtract:   { AppendFormatString(&State, "das"); } break;
            case OpCode_CompareConvertByteToWord:          { AppendFormatString(&State, "cbw"); } break;
            case OpCode_CompareConvertWordToDoubleWord:    { AppendFormatString(&State, "cwd"); } break;
            
            case OpCode_CompareASCIIAdjustForMultiply:
            {
                u8 Op1 = At[Index++];
                if(Op1 == 0b00001010)
                {
                    AppendFormatString(&State, "aam");
                }
                else
                {
                    AppendFormatString(&State, "Unknown op code %b, %b", Op0, Op1);
                }
            } break;
            
            case OpCode_CompareASCIIAdjustForDivide:
            {
                u8 Op1 = At[Index++];
                if(Op1 == 0b00001010)
                {
                    // NOTE(kstandbridge): AAD ASCII adjust for divide
                    AppendFormatString(&State, "aad");
                }
                else
                {
                    AppendFormatString(&State, "Unknown op code %b, %b", Op0, Op1);
                }
            } break;
            
            case OpCode_OutTranslateByteToAL:            { AppendFormatString(&State, "xlat"); } break;
            case OpCode_OutLoadAHWithFlags:              { AppendFormatString(&State, "lahf"); } break;
            case OpCode_OutStoreAHIntoFlags:             { AppendFormatString(&State, "sahf"); } break;
            case OpCode_OutPushFlags:                    { AppendFormatString(&State, "pushf"); } break;
            case OpCode_OutPopFlags:                     { AppendFormatString(&State, "popf"); } break;
            
            default:
            {
                
                switch(Op0 >> 1)
                {
                    case OpCodeStringRepeat:
                    {
                        u8 Op1 = At[Index++];
                        b32 IsWord = (Op1 & 1);
                        string W = (IsWord) ? String("w") : String("b");
                        
                        switch(Op1 >> 1)
                        {
                            case OpCodeStringMoveByteWord:        { AppendFormatString(&State, "rep movs%S", W); } break;
                            case OpCodeStringCompareByteWord:     { AppendFormatString(&State, "rep cmps%S", W); } break;
                            case OpCodeStringScanByteWord:        { AppendFormatString(&State, "rep scas%S", W); } break;
                            case OpCodeStringLoadByteWdToALXL:    { AppendFormatString(&State, "rep lods%S", W); } break;
                            case OpCodeStringStoreByteWdFromALXL: { AppendFormatString(&State, "rep stos%S", W); } break;
                            default:                              { AppendFormatString(&State, "TODO REP %b", Op1); }
                        }
                    } break;
                    
                    case OpInFixedPort:    { AppendFormatString(&State, "in al, %u", At[Index++]); } break;
                    case OpInVariablePort: { AppendFormatString(&State, "in %S, dx", (Op0 & 1) ? String("ax") : String("al")); } break;
                    
                    case OpOutFixedPort:    { AppendFormatString(&State, "out %u, ax", At[Index++]); } break;
                    case OpOutVariablePort: { AppendFormatString(&State, "out dx, %S", (Op0 & 1) ? String("ax") : String("al")); } break;
                    
                    default:
                    {
                        switch(Op0 >> 3)
                        {
                            case OpPushRegister:            { AppendFormatString(&State, "push %S", GetRegisterName(Op0 & 0b111, 1)); } break;
                            case OpPopRegister:             { AppendFormatString(&State, "pop %S", GetRegisterName(Op0 & 0b111, 1)); } break;
                            
                            case OpXchgRegisterWithMemory:  { AppendFormatString(&State, "xchg ax, %S", GetRegisterName(Op0 & 0b111, 1)); } break;
                            
                            case OpIncRegister:             { AppendFormatString(&State, "inc %S", GetRegisterName(Op0 & 0b111, 1)); } break;
                            
                            case OpDecRegister:             { AppendFormatString(&State, "dec %S", GetRegisterName(Op0 & 0b111, 1)); } break;
                            
                            
                            default:
                            {
                                if((Op0 >> 6 == 0b000) &&
                                   ((Op0 & 0b111) == 0b110) &&
                                   (((Op0 >> 3) & 0b111) < ArrayCount(SegmentRegisterCode)))
                                {
                                    // NOTE(kstandbridge): Push Segment Register
                                    u8 Reg =  (Op0 >> 3) & ((1<<3)-1);
                                    if(Reg > ArrayCount(SegmentRegisterCode))
                                    {
                                        AppendFormatString(&State, "; TODO SegmentRegisterCode overflow");
                                    }
                                    else
                                    {
                                        AppendFormatString(&State, "push %S", SegmentRegisterCode[Reg]);
                                    }
                                }
                                else if((Op0 >> 6 == 0b000) &&
                                        ((Op0 & 0b111) == 0b111))
                                {
                                    // NOTE(kstandbridge): Pop Segment Register
                                    u8 Reg =  (Op0 >> 3) & ((1<<3)-1);
                                    AppendFormatString(&State, "pop %S", SegmentRegisterCode[Reg]);
                                }
                                else
                                {
                                    b32 LockPrefix = ((Op0 >> 0) == 0b11110000);
                                    if(LockPrefix)
                                    {
                                        AppendFormatString(&State, "lock ");
                                        Op0 = At[Index++];
                                    }
                                    
                                    b32 Segment = ((Op0 >> 6 == 0b000) &&
                                                   ((Op0 & 0b111) == 0b110));
                                    string SegPrefix = String("");
                                    if(Segment)
                                    {
                                        u8 SegCode = (((Op0 >> 3) & 0b111));
                                        SegCode -= ArrayCount(SegmentRegisterCode);
                                        SegPrefix = SegmentRegisterCode[SegCode];
                                        Op0 = At[Index++];
                                    }
                                    
                                    u8 Op1 = At[Index++];
                                    b32 Logic =                                 ((Op0 >> 2) == 0b110100);
                                    b32 MovRegisterMemoryToFromRegister =       ((Op0 >> 2) == 0b100010);
                                    b32 AddRegisterMemoryToFromRegister =       ((Op0 >> 2) == 0b000000);
                                    b32 SubRegisterMemoryToFromRegister =       ((Op0 >> 2) == 0b001010);
                                    b32 SbbRegisterMemoryToFromRegister =       ((Op0 >> 2) == 0b000110);
                                    b32 CmpRegisterMemoryAndFromRegister =      ((Op0 >> 2) == 0b001110);
                                    b32 OrRegisterMemoryAndFromRegister =       ((Op0 >> 2) == 0b000010);
                                    b32 XorRegisterMemoryAndFromRegister =      ((Op0 >> 2) == 0b001100);
                                    b32 AdcRegisterMemoryWithRegisterToEither = ((Op0 >> 2) == 0b000100);
                                    b32 AndRegisterMemoryWithRegisterToEither = ((Op0 >> 2) == 0b001000);
                                    b32 XchgRegisterMemoryWithRegister =        ((Op0 >> 1) == 0b1000011);
                                    b32 TestRegisterMemoryAndRegister =         ((Op0 >> 1) == 0b1000010);
                                    b32 IncRegisterMemory =                     ((Op0 >> 1) == 0b1111111);
                                    b32 NegChangeSign =                         ((Op0 >> 1) == 0b1111011);
                                    b32 PushRegisterMemory =                    ((Op0 >> 0) == 0b11111111);
                                    b32 PopRegisterMemory =                     ((Op0 >> 0) == 0b10001111);
                                    b32 AddImmediateToAccumulator =             ((Op0 >> 2) == 0b000001);
                                    b32 AdcImmediateToAccumulator =             ((Op0 >> 2) == 0b000101);
                                    b32 AndImmediateToAccumulator =             ((Op0 >> 1) == 0b0010010);
                                    b32 OrImmediateToAccumulator =              ((Op0 >> 1) == 0b0000110);
                                    b32 XorImmediateToAccumulator =             ((Op0 >> 1) == 0b0011010);
                                    b32 CmpImmediateWithAccumulator =           ((Op0 >> 1) == 0b0011110);
                                    b32 SubImmediateFromAccumulator =           ((Op0 >> 1) == 0b0010110);
                                    b32 SbbImmediateFromAccumulator =           ((Op0 >> 1) == 0b0001110);
                                    b32 TestImmediateDataAndAccumulator =       ((Op0 >> 1) == 0b1010100);
                                    b32 MovImmediateToRegisterMemory =          ((Op0 >> 1) == 0b1100011);
                                    b32 AddImmediateToRegisterMemory =          ((Op0 >> 2) == 0b100000);
                                    b32 MovImmediateToRegister =                ((Op0 >> 4) == 0b1011);
                                    b32 MovMemoryToAccumulator =                ((Op0 >> 1) == 0b1010000);
                                    b32 MovAccumlatorToMemory =                 ((Op0 >> 1) == 0b1010001);
                                    b32 LoadEAtoRegister =                      ((Op0 >> 0) == 0b10001101);
                                    b32 LoadPointerToDS =                       ((Op0 >> 0) == 0b11000101);
                                    b32 LoadPointerToES =                       ((Op0 >> 0) == 0b11000100);
                                    
                                    if(AddImmediateToAccumulator ||
                                       AdcImmediateToAccumulator ||
                                       AndImmediateToAccumulator ||
                                       OrImmediateToAccumulator ||
                                       XorImmediateToAccumulator ||
                                       CmpImmediateWithAccumulator ||
                                       SubImmediateFromAccumulator ||
                                       SbbImmediateFromAccumulator ||
                                       TestImmediateDataAndAccumulator ||
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
                                        else if(AdcImmediateToAccumulator)
                                        {
                                            if(IsWord)
                                            {
                                                AppendFormatString(&State, "adc ax, %d", Value);
                                            }
                                            else
                                            {
                                                AppendFormatString(&State, "adc al, %d", Value);
                                            }
                                        }
                                        else if(AndImmediateToAccumulator)
                                        {
                                            if(IsWord)
                                            {
                                                AppendFormatString(&State, "and ax, %d", Value);
                                            }
                                            else
                                            {
                                                AppendFormatString(&State, "and al, %d", Value);
                                            }
                                        }
                                        else if(OrImmediateToAccumulator)
                                        {
                                            if(IsWord)
                                            {
                                                AppendFormatString(&State, "or ax, %d", Value);
                                            }
                                            else
                                            {
                                                AppendFormatString(&State, "or al, %d", Value);
                                            }
                                        }
                                        else if(XorImmediateToAccumulator)
                                        {
                                            if(IsWord)
                                            {
                                                AppendFormatString(&State, "xor ax, %d", Value);
                                            }
                                            else
                                            {
                                                AppendFormatString(&State, "xor al, %d", Value);
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
                                        else if (SbbImmediateFromAccumulator)
                                        {
                                            if(IsWord)
                                            {
                                                AppendFormatString(&State, "sbb ax, %d", Value);
                                            }
                                            else
                                            {
                                                AppendFormatString(&State, "sbb al, %d", Value);
                                            }
                                        }
                                        else if (TestImmediateDataAndAccumulator)
                                        {
                                            if(IsWord)
                                            {
                                                AppendFormatString(&State, "test ax, %d", Value);
                                            }
                                            else
                                            {
                                                AppendFormatString(&State, "test al, %d", Value);
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
                                    else if(Logic ||
                                            MovRegisterMemoryToFromRegister || 
                                            AddRegisterMemoryToFromRegister ||
                                            SubRegisterMemoryToFromRegister ||
                                            SbbRegisterMemoryToFromRegister ||
                                            CmpRegisterMemoryAndFromRegister ||
                                            OrRegisterMemoryAndFromRegister ||
                                            XorRegisterMemoryAndFromRegister ||
                                            XchgRegisterMemoryWithRegister ||
                                            TestRegisterMemoryAndRegister ||
                                            AdcRegisterMemoryWithRegisterToEither ||
                                            AndRegisterMemoryWithRegisterToEither ||
                                            IncRegisterMemory ||
                                            NegChangeSign ||
                                            PushRegisterMemory ||
                                            PopRegisterMemory ||
                                            MovImmediateToRegisterMemory ||
                                            AddImmediateToRegisterMemory ||
                                            LoadEAtoRegister ||
                                            LoadPointerToDS ||
                                            LoadPointerToES)
                                    {
                                        b32 IsWord = (Op0 & 1);
                                        
                                        // NOTE(kstandbridge): 
                                        
                                        b32 Direction = (AddRegisterMemoryToFromRegister || 
                                                         MovRegisterMemoryToFromRegister ||
                                                         SubRegisterMemoryToFromRegister ||
                                                         SbbRegisterMemoryToFromRegister ||
                                                         CmpRegisterMemoryAndFromRegister ||
                                                         OrRegisterMemoryAndFromRegister ||
                                                         XorRegisterMemoryAndFromRegister ||
                                                         AdcRegisterMemoryWithRegisterToEither ||
                                                         AndRegisterMemoryWithRegisterToEither) ? ((Op0 >> 1) & 1) : 0;
                                        
                                        if(XchgRegisterMemoryWithRegister || TestRegisterMemoryAndRegister)
                                        {
                                            Direction = (Op1 >> 7);
                                        }
                                        
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
                                                s16 Value;
                                                
                                                if((Op0 & 0b11) == 0b01)
                                                {
                                                    u8 ValueLow = At[Index++];
                                                    u8 ValueHigh = At[Index++];
                                                    u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                                                    
                                                    Value = *(u16 *)&ValueWide;
                                                }
                                                else
                                                {
                                                    Value = *(u8 *)&At[Index++];
                                                }
                                                
                                                if(Src == 0b000)
                                                {
                                                    AppendFormatString(&State, "add %S, %d", GetRegisterName(Dest, IsWord), Value);
                                                }
                                                else if(Src == 0b101)
                                                {
                                                    AppendFormatString(&State, "sub %S, %d", GetRegisterName(Dest, IsWord), Value);
                                                }
                                                else if(Src == 0b011)
                                                {
                                                    AppendFormatString(&State, "sbb %S, %d", GetRegisterName(Dest, IsWord), Value);
                                                }
                                                else if(Src == 0b111)
                                                {
                                                    AppendFormatString(&State, "cmp %S, %d", GetRegisterName(Dest, IsWord), Value);
                                                }
                                                else if(Src == 0b010)
                                                {
                                                    AppendFormatString(&State, "adc %S, %d", GetRegisterName(Dest, IsWord), Value);
                                                }
                                                else
                                                {
                                                    AppendFormatString(&State, "TODO");
                                                }
                                                
                                            }
                                            else if(MovRegisterMemoryToFromRegister)
                                            {
                                                AppendFormatString(&State, "mov %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                                            }
                                            else if(AddRegisterMemoryToFromRegister)
                                            {
                                                AppendFormatString(&State, "add %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                                            }
                                            else if(SubRegisterMemoryToFromRegister)
                                            {
                                                AppendFormatString(&State, "sub %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                                            }
                                            else if(SbbRegisterMemoryToFromRegister)
                                            {
                                                AppendFormatString(&State, "sbb %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                                            }
                                            else if(CmpRegisterMemoryAndFromRegister)
                                            {
                                                AppendFormatString(&State, "cmp %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                                            }
                                            else if(OrRegisterMemoryAndFromRegister)
                                            {
                                                AppendFormatString(&State, "or %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                                            }
                                            else if(XorRegisterMemoryAndFromRegister)
                                            {
                                                AppendFormatString(&State, "xor %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                                            }
                                            else if(XchgRegisterMemoryWithRegister)
                                            {
                                                if(Direction)
                                                {
                                                    AppendFormatString(&State, "xchg %S, %S", GetRegisterName(Src, IsWord), GetRegisterName(Dest, IsWord));
                                                }
                                                else
                                                {
                                                    AppendFormatString(&State, "xchg %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                                                }
                                            }
                                            else if(AdcRegisterMemoryWithRegisterToEither)
                                            {
                                                AppendFormatString(&State, "adc %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                                            }
                                            else if(TestRegisterMemoryAndRegister)
                                            {
                                                AppendFormatString(&State, "test %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                                            }
                                            else if(AndRegisterMemoryWithRegisterToEither)
                                            {
                                                AppendFormatString(&State, "and %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
                                            }
                                            else if(IncRegisterMemory)
                                            {
                                                if(Src == 0b001)
                                                {
                                                    AppendFormatString(&State, "dec %S", GetRegisterName(Dest, IsWord));
                                                }
                                                else if(Src == 0b000)
                                                {
                                                    AppendFormatString(&State, "inc %S", GetRegisterName(Dest, IsWord));
                                                }
                                                else if(Src == 0b010)
                                                {
                                                    AppendFormatString(&State, "call %S", GetRegisterName(Dest, IsWord));
                                                }
                                                else if(Src == 0b100)
                                                {
                                                    AppendFormatString(&State, "jmp %S", GetRegisterName(Dest, IsWord));
                                                }
                                                else
                                                {
                                                    AppendFormatString(&State, "TODO IncRegisterMemory %S", GetRegisterName(Dest, IsWord));
                                                }
                                            }
                                            else if(Logic)
                                            {
                                                b32 IsSpecifiedInClRegister = (1 == ((Op0 >> 1) & 1));
                                                string Count = (IsSpecifiedInClRegister) ? String("cl") : String("1");
                                                
                                                if(Src == 0b100)
                                                {
                                                    AppendFormatString(&State, "shl %S, %S", GetRegisterName(Dest, IsWord), Count);
                                                }
                                                else if(Src == 0b101)
                                                {
                                                    AppendFormatString(&State, "shr %S, %S", GetRegisterName(Dest, IsWord), Count);
                                                }
                                                else if(Src == 0b111)
                                                {
                                                    AppendFormatString(&State, "sar %S, %S", GetRegisterName(Dest, IsWord), Count);
                                                }
                                                else if(Src == 0b000)
                                                {
                                                    AppendFormatString(&State, "rol %S, %S", GetRegisterName(Dest, IsWord), Count);
                                                }
                                                else if(Src == 0b001)
                                                {
                                                    AppendFormatString(&State, "ror %S, %S", GetRegisterName(Dest, IsWord), Count);
                                                }
                                                else if(Src == 0b010)
                                                {
                                                    AppendFormatString(&State, "rcl %S, %S", GetRegisterName(Dest, IsWord), Count);
                                                }
                                                else if(Src == 0b011)
                                                {
                                                    AppendFormatString(&State, "rcr %S, %S", GetRegisterName(Dest, IsWord), Count);
                                                }
                                                else
                                                {
                                                    AppendFormatString(&State, "TODO %S, %S", GetRegisterName(Dest, IsWord), Count);
                                                }
                                            }
                                            else if(NegChangeSign)
                                            {
                                                if(Src == 0b100)
                                                {
                                                    AppendFormatString(&State, "mul %S", GetRegisterName(Dest, IsWord));
                                                }
                                                else if(Src == 0b101)
                                                {
                                                    AppendFormatString(&State, "imul %S", GetRegisterName(Dest, IsWord));
                                                }
                                                else if(Src == 0b110)
                                                {
                                                    AppendFormatString(&State, "div %S", GetRegisterName(Dest, IsWord));
                                                }
                                                else if(Src == 0b111)
                                                {
                                                    AppendFormatString(&State, "idiv %S", GetRegisterName(Dest, IsWord));
                                                }
                                                else if(Src == 0b010)
                                                {
                                                    AppendFormatString(&State, "not %S", GetRegisterName(Dest, IsWord));
                                                }
                                                else if(Src == 0b000)
                                                {
                                                    s8 Value = At[Index++];
                                                    AppendFormatString(&State, "test %S, %d", GetRegisterName(Dest, IsWord), Value);
                                                }
                                                else
                                                {
                                                    AppendFormatString(&State, "neg %S", GetRegisterName(Dest, IsWord));
                                                }
                                            }
                                            else
                                            {
                                                AppendFormatString(&State, "TODO %S, %S", GetRegisterName(Dest, IsWord), GetRegisterName(Src, IsWord));
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
                                                    
                                                    if(AndRegisterMemoryWithRegisterToEither)
                                                    {
                                                        AppendFormatString(&State, "and %S, [%d]", GetRegisterName(Src, IsWord), Value);
                                                    }
                                                    else if(CmpRegisterMemoryAndFromRegister)
                                                    {
                                                        if(Segment)
                                                        {
                                                            AppendFormatString(&State, "cmp %S, %S:[%d]", GetRegisterName(Src, IsWord), SegPrefix, Value);
                                                        }
                                                        else
                                                        {
                                                            AppendFormatString(&State, "cmp %S, [%d]", GetRegisterName(Src, IsWord), Value);
                                                        }
                                                    }
                                                    else if(Src == 0b111)
                                                    {
                                                        s8 Value2 = *(s8 *)&At[Index++];
                                                        AppendFormatString(&State, "cmp word [%d], %d", Value, Value2);
                                                    }
                                                    else if(Src == 0b010)
                                                    {
                                                        AppendFormatString(&State, "call [%u]", (u16)Value);
                                                    }
                                                    else if(Src == 0b100)
                                                    {
                                                        AppendFormatString(&State, "jmp [%u]", (u16)Value);
                                                    }
                                                    else if(Src == 0b110)
                                                    {
                                                        AppendFormatString(&State, "push word [%d]", Value);
                                                    }
                                                    else if(Src == 0b001)
                                                    {
                                                        if(Logic)
                                                        {
                                                            AppendFormatString(&State, "ror word [%d], 1", Value);
                                                        }
                                                        else if(OrRegisterMemoryAndFromRegister)
                                                        {
                                                            AppendFormatString(&State, "or cx, [%d]", Value);
                                                        }
                                                        else if(XorRegisterMemoryAndFromRegister)
                                                        {
                                                            AppendFormatString(&State, "xor cx, [%d]", Value);
                                                        }
                                                        else
                                                        {
                                                            AppendFormatString(&State, "dec word [%d]", Value);
                                                        }
                                                    }
                                                    else if(Src == 0b000)
                                                    {
                                                        if(PopRegisterMemory)
                                                        {
                                                            AppendFormatString(&State, "pop word [%d]", Value);
                                                        }
                                                        else
                                                        {
                                                            AppendFormatString(&State, "inc word [%d]", Value);
                                                        }
                                                    }
                                                    else if(NegChangeSign &&
                                                            (Src == 0b011))
                                                    {
                                                        AppendFormatString(&State, "neg word [%d]", Value);
                                                    }
                                                    else if(NegChangeSign &&
                                                            (Src == 0b101))
                                                    {
                                                        AppendFormatString(&State, "imul word [%d]", Value);
                                                    }
                                                    else
                                                    {
                                                        AppendFormatString(&State, "mov %S, [%d]", GetRegisterName(Src, IsWord), Value);
                                                    }
                                                }
                                                else
                                                {
                                                    if(Logic || XchgRegisterMemoryWithRegister)
                                                    {
                                                        u8 ValueLow = At[Index++];
                                                        u8 ValueHigh = At[Index++];
                                                        u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                                                        
                                                        s16 Value = *(u16 *)&ValueWide;
                                                        
                                                        b32 IsSpecifiedInClRegister = (1 == ((Op0 >> 1) & 1));
                                                        string Count = (IsSpecifiedInClRegister) ? String("cl") : String("1");
                                                        if(XchgRegisterMemoryWithRegister)
                                                        {
                                                            AppendFormatString(&State, "xchg [%d], %S", Value, GetRegisterName(Src, IsWord));
                                                        }
                                                        else if(Src == 0b010)
                                                        {
                                                            AppendFormatString(&State, "rcl byte [%d], %S", Value, Count);
                                                        }
                                                        else if(Src == 0b001)
                                                        {
                                                            AppendFormatString(&State, "ror byte [%d], %S", Value, Count);
                                                        }
                                                        else
                                                        {
                                                            AppendFormatString(&State, "TODO/logic byte [%d], %S", Value, GetRegisterName(Src, IsWord));
                                                        }
                                                    }
                                                    else
                                                    {
                                                        u8 Op2 = At[Index++];
                                                        s8 Value = *(u8 *)&Op2;
                                                        AppendFormatString(&State, "mov %S, [%d]", GetRegisterName(Src, IsWord), Value);
                                                    }
                                                }
                                            }
                                            else if(MovRegisterMemoryToFromRegister)
                                            {                
                                                if(Direction)
                                                {
                                                    if(Segment)
                                                    {
                                                        AppendFormatString(&State, "mov %S, %S:%S", GetRegisterName(Dest, IsWord), SegPrefix, Bytes[Src]);
                                                    }
                                                    else
                                                    {
                                                        AppendFormatString(&State, "mov %S, %S", GetRegisterName(Dest, IsWord), Bytes[Src]);
                                                    }
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
                                            else if(SbbRegisterMemoryToFromRegister)
                                            {                
                                                if(Direction)
                                                {
                                                    AppendFormatString(&State, "sbb %S, %S", GetRegisterName(Src, IsWord), Bytes[Dest]);
                                                }
                                                else
                                                {
                                                    AppendFormatString(&State, "sbb %S, %S", Bytes[Dest], GetRegisterName(Src, IsWord));
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
                                            else if(AdcRegisterMemoryWithRegisterToEither)
                                            {
                                                if(Direction)
                                                {
                                                    AppendFormatString(&State, "adc %S, %S", GetRegisterName(Src, IsWord), Bytes[Dest]);
                                                }
                                                else
                                                {
                                                    AppendFormatString(&State, "adc %S, %S", Bytes[Dest], GetRegisterName(Src, IsWord));
                                                }
                                            }
                                            
#if 0                    
                                            else if(AndRegisterMemoryWithRegisterToEither)
                                            {
                                                if(Direction)
                                                {
                                                    AppendFormatString(&State, "and %S, %S", GetRegisterName(Src, IsWord), Bytes[Dest]);
                                                }
                                                else
                                                {
                                                    AppendFormatString(&State, "and %S, %S", Bytes[Dest], GetRegisterName(Src, IsWord));
                                                }
                                            }
#endif
                                            
                                            else if(NegChangeSign)
                                            {
                                                if(Src == 0b000)
                                                {
                                                    s8 Value = At[Index++];
                                                    AppendFormatString(&State, "test byte %S, %d", Bytes[Dest], Value);
                                                }
                                                else if(Src == 0b111)
                                                {
                                                    AppendFormatString(&State, "idiv byte %S", Bytes[Dest]);
                                                }
                                                else
                                                {
                                                    AppendFormatString(&State, "imul byte %S", Bytes[Dest]);
                                                }
                                            }
#if 0                    
                                            else if(XchgRegisterMemoryWithRegister)
                                            {                
                                                if(Direction)
                                                {
                                                    AppendFormatString(&State, "xchg %S, %S", GetRegisterName(Src, IsWord), Bytes[Dest]);
                                                }
                                                else
                                                {
                                                    AppendFormatString(&State, "xchg %S, %S", Bytes[Dest], GetRegisterName(Src, IsWord));
                                                }
                                            }
#endif
                                            
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
                                                    else if(PushRegisterMemory)
                                                    {
                                                        AppendFormatString(&State, "push word %S", Bytes[Dest]);
                                                    }
                                                    else if(PopRegisterMemory)
                                                    {
                                                        AppendFormatString(&State, "pop word %S", Bytes[Dest]);
                                                    }
                                                    else if(Logic)
                                                    {
                                                        b32 IsSpecifiedInClRegister = (1 == ((Op0 >> 1) & 1));
                                                        string Count = (IsSpecifiedInClRegister) ? String("cl") : String("1");
                                                        AppendFormatString(&State, "rcr word %S, %S", Bytes[Dest], Count);
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
                                            else if(AddRegisterMemoryToFromRegister)
                                            {
                                                Op = String("add");
                                            }
                                            else if(AddImmediateToRegisterMemory)
                                            {
                                                if(Src == 0b100)
                                                {
                                                    Op = String("and");
                                                }
                                                else if(Src == 0b000)
                                                {
                                                    Op = String("add");
                                                }
                                                else if(Src == 0b001)
                                                {
                                                    Op = String("or");
                                                }
                                                else if(Src == 0b011)
                                                {
                                                    Op = String("sbb");
                                                }
                                                else
                                                {
                                                    Op = String("xor");
                                                }
                                            }
                                            else if(SubRegisterMemoryToFromRegister)
                                            {
                                                Op = String("sub");
                                            }
                                            else if(SbbRegisterMemoryToFromRegister)
                                            {
                                                Op = String("sbb");
                                            }
                                            else if(CmpRegisterMemoryAndFromRegister)
                                            {
                                                Op = String("cmp");
                                            }
                                            else if(OrRegisterMemoryAndFromRegister)
                                            {
                                                Op = String("or");
                                            }
                                            else if(XorRegisterMemoryAndFromRegister)
                                            {
                                                Op = String("xor");
                                            }
                                            else if(XchgRegisterMemoryWithRegister)
                                            {
                                                Op = String("xchg");
                                            }
                                            else if(TestRegisterMemoryAndRegister)
                                            {
                                                Op = String("test");
                                            }
                                            else if(AdcRegisterMemoryWithRegisterToEither)
                                            {
                                                Op = String("adc");
                                            }
                                            else if(AndRegisterMemoryWithRegisterToEither)
                                            {
                                                Op = String("and");
                                            }
                                            else if(IncRegisterMemory || PushRegisterMemory)
                                            {
                                                if(Src == 0b110)
                                                {
                                                    Op = String("push");
                                                }
                                                else if(Src == 0b001)
                                                {
                                                    Op = String("dec");
                                                }
                                                else if(Src == 0b010)
                                                {
                                                    Op = String("call");
                                                }
                                                else
                                                {
                                                    Op = String("inc");
                                                }
                                            }
                                            else if(Logic)
                                            {
                                                if(Src == 0b100)
                                                {
                                                    Op = String("shl");
                                                }
                                                else if(Src == 0b101)
                                                {
                                                    Op = String("shr");
                                                }
                                                else if(Src == 0b111)
                                                {
                                                    Op = String("sar");
                                                }
                                                else if(Src == 0b000)
                                                {
                                                    Op = String("rol");
                                                }
                                                else if(Src == 0b001)
                                                {
                                                    Op = String("ror");
                                                }
                                                else
                                                {
                                                    Op = String("TODO/Logic");
                                                }
                                            }
                                            else if(NegChangeSign)
                                            {
                                                if(Src == 0b100)
                                                {
                                                    Op = String("mul");
                                                }
                                                else if(Src == 0b110)
                                                {
                                                    Op = String("div");
                                                }
                                                else if(Src == 0b111)
                                                {
                                                    Op = String("idiv");
                                                }
                                                else if(Src == 0b010)
                                                {
                                                    Op = String("not");
                                                }
                                                else if(Src == 0b000)
                                                {
                                                    Op = String("test");
                                                }
                                                else
                                                {
                                                    Op = String("neg");
                                                }
                                            }
                                            else if(PopRegisterMemory)
                                            {
                                                Op = String("pop");
                                            }
                                            else if(LoadEAtoRegister)
                                            {
                                                Op = String("lea");
                                            }
                                            else if(LoadPointerToDS)
                                            {
                                                Op = String("lds");
                                            }
                                            else if(LoadPointerToES)
                                            {
                                                Op = String("les");
                                            }
                                            else
                                            {
                                                Op = String("TODO");
                                            }
                                            
                                            if(Direction)
                                            {
                                                if(Segment)
                                                {
                                                    AppendFormatString(&State, "%S %S, %S:[%S", Op, GetRegisterName(Src, IsWord), SegPrefix, Bytes[Dest]);
                                                }
                                                else
                                                {
                                                    AppendFormatString(&State, "%S %S, [%S", Op, GetRegisterName(Src, IsWord), Bytes[Dest]);
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
                                            }
                                            else
                                            {
                                                if(AddImmediateToRegisterMemory || 
                                                   PushRegisterMemory || 
                                                   PopRegisterMemory || 
                                                   IncRegisterMemory || 
                                                   NegChangeSign || 
                                                   Logic)
                                                {
                                                    if(IncRegisterMemory && 
                                                       (Src == 0b010))
                                                    {
                                                        AppendFormatString(&State, "%S [%S", Op, Bytes[Dest]);
                                                    }
                                                    else if(IsWord)
                                                    {
                                                        if(Segment)
                                                        {
                                                            AppendFormatString(&State, "%S word %S:[%S", Op, SegPrefix, Bytes[Dest]);
                                                        }
                                                        else
                                                        {
                                                            AppendFormatString(&State, "%S word [%S", Op, Bytes[Dest]);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        
                                                        if(Segment)
                                                        {
                                                            AppendFormatString(&State, "%S byte %S:[%S", Op, SegPrefix, Bytes[Dest]);
                                                        }
                                                        else
                                                        {
                                                            AppendFormatString(&State, "%S byte [%S", Op, Bytes[Dest]);
                                                        }
                                                    }
                                                }
                                                else if(LoadEAtoRegister || LoadPointerToDS || LoadPointerToES)
                                                {
                                                    AppendFormatString(&State, "%S %S, [%S", Op, GetRegisterName(Src, 1), Bytes[Dest]);
                                                }
                                                else
                                                {
                                                    if(Segment)
                                                    {
                                                        AppendFormatString(&State, "%S %S:[%S", Op, SegPrefix, Bytes[Dest]);
                                                    }
                                                    else
                                                    {
                                                        AppendFormatString(&State, "%S [%S", Op, Bytes[Dest]);
                                                    }
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
                                                   SbbRegisterMemoryToFromRegister ||
                                                   MovRegisterMemoryToFromRegister ||
                                                   CmpRegisterMemoryAndFromRegister ||
                                                   OrRegisterMemoryAndFromRegister ||
                                                   XorRegisterMemoryAndFromRegister ||
                                                   XchgRegisterMemoryWithRegister ||
                                                   TestRegisterMemoryAndRegister ||
                                                   AdcRegisterMemoryWithRegisterToEither ||
                                                   AndRegisterMemoryWithRegisterToEither)
                                                {
                                                    AppendFormatString(&State, ", %S", GetRegisterName(Src, IsWord));
                                                }
                                                else if(MovImmediateToRegisterMemory || 
                                                        AddImmediateToRegisterMemory ||
                                                        (NegChangeSign && Src == 0b000))
                                                {
                                                    if(IsWord)
                                                    {
                                                        if(AddImmediateToRegisterMemory && (Op0 >> 1) & 1)
                                                        {
                                                            u8 ValueUnsigned = At[Index++];
                                                            Value = *(u8 *)&ValueUnsigned;
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
                                                    else
                                                    {
                                                        Value = At[Index++];
                                                        AppendFormatString(&State, ", %d", Value);
                                                    }
                                                }
                                                else if(PushRegisterMemory || 
                                                        PopRegisterMemory || 
                                                        IncRegisterMemory ||
                                                        LoadEAtoRegister || 
                                                        LoadPointerToDS || 
                                                        NegChangeSign ||
                                                        LoadPointerToES)
                                                {
                                                    // NOTE(kstandbridge): Intentionally do nothing
                                                }
                                                else if(Logic)
                                                {
                                                    b32 IsSpecifiedInClRegister = (1 == ((Op0 >> 1) & 1));
                                                    string Count = (IsSpecifiedInClRegister) ? String("cl") : String("1");
                                                    AppendFormatString(&State, ", %S", Count);
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
                                        b32 Wide = (1 == ((Op0 >> 3) & 1));
                                        u8 Reg = (Op0 & 0b111);
                                        if(Wide)
                                        {
                                            u8 Op2 = At[Index++];
                                            u16 ValueWide = ((Op2 & 0xFF) << 8) | (Op1 & 0xFF);
                                            s16 Value = *(u16 *)&ValueWide;
                                            AppendFormatString(&State, "mov %S, %d", GetRegisterName(Reg, Wide), Value);
                                        }
                                        else
                                        {
                                            s8 Value = *(u8 *)&Op1;
                                            AppendFormatString(&State, "mov %S, %d", GetRegisterName(Reg, Wide), Value);
                                        }
                                        
                                    }
                                    else
                                    {
                                        AppendFormatString(&State, "; Unknown Op code %b", Op0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        AppendFormatString(&State,"\n" );
        if(Index > Size)
        {
            AppendFormatString(&State, "; pass overflow");
            break;
        }
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
    
    {
        u8 Op[] = { 0b01110100, 0b11111111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("je label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01111100, 0b11111111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jl label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01111110, 0b11111111 };
        string Actual = Parse(Arena, Op, sizeof(Op));
        AssertEqualString(String("jle label\n"), Actual);
    }
    
    {
        u8 Op[] = { 0b01110010, 0b11111111 };
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
}

inline void
RunListing42Tests(memory_arena *Arena)
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
    
#if 0
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
    RunListing38Tests(Arena);
    RunListing39Tests(Arena);
    RunListing40Tests(Arena);
    RunListing41Tests(Arena);
    RunListing42Tests(Arena);
    
#if 0    
    
#if 1
    string FileName = String("test");
#else
    string FileName = String("listing_42");
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
    
#endif
    
}
