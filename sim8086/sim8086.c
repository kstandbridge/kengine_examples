#include "sim8086.h"

#define MOD { Encoding_MOD, 2 }
#define REG { Encoding_REG, 3 }
#define TYPE REG
#define RM { Encoding_RM, 3 }

#define DISP_LO { Encoding_DISP_LO, 8 }
#define DISP_HI { Encoding_DISP_HI, 8 }

#define DATA { Encoding_DATA, 8 }
#define DATA_IF_W { Encoding_DATA_IF_W, 8 }

#define ADDR_LO { Encoding_DATA_LO, 8 }
#define ADDR_HI { Encoding_DATA_HI, 8 }

#define DATA_LO { Encoding_DATA_LO, 8 }
#define DATA_HI { Encoding_DATA_HI, 8 }

#define IP_INC8 { Encoding_IP_INC8, 8 }

#define IP_INC_LO { Encoding_DATA_LO, 8 }
#define IP_INC_HI { Encoding_DATA_HI, 8 }

#define CS_LO { Encoding_CS_LO, 8 }
#define CS_HI { Encoding_CS_HI, 8 }

global instruction_table_entry GlobalInstructionTable[] =
{
    { Instruction_Add,                    0b00000000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Add,                    0b00000001, Flag_W, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Add,                    0b00000010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Add,                    0b00000011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_AddAccumulator,         0b00000100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_AddAccumulator,         0b00000101, Flag_W, 0, { DATA, DATA_IF_W } },
    { Instruction_PushSegmentRegister,    0b00000110, 0, 0b00, { 0 } },
    { Instruction_PopSegmentRegister,     0b00000111, 0, 0b00, { 0 } },
    { Instruction_Or,                     0b00001000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Or,                     0b00001001, Flag_W, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Or,                     0b00001010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Or,                     0b00001011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_OrAccumulator,          0b00001100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_OrAccumulator,          0b00001101, Flag_W, 0, { DATA, DATA_IF_W } },
    { Instruction_PushSegmentRegister,    0b00001110, 0, 0b01, { 0 } },
    { Instruction_PopSegmentRegister,     0b00001111, 0, 0b01, { 0 } },
    { Instruction_Adc,                    0b00010000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Adc,                    0b00010001, Flag_W, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Adc,                    0b00010010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Adc,                    0b00010011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_AdcAccumulator,         0b00010100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_AdcAccumulator,         0b00010101, Flag_W, 0, { DATA, DATA_IF_W } },
    { Instruction_PushSegmentRegister,    0b00010110, 0, 0b10, { 0 } },
    { Instruction_PopSegmentRegister,     0b00010111, 0, 0b10, { 0 } },
    { Instruction_Sbb,                    0b00011000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sbb,                    0b00011001, Flag_W, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sbb,                    0b00011010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sbb,                    0b00011011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_SbbAccumulator,         0b00011100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_SbbAccumulator,         0b00011101, Flag_W, 0, { DATA, DATA_IF_W } },
    { Instruction_PushSegmentRegister,    0b00011110, 0, 0b11, { 0 } },
    { Instruction_PopSegmentRegister,     0b00011111, 0, 0b11, { 0 } },
    { Instruction_And,                    0b00100000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_And,                    0b00100001, Flag_W, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_And,                    0b00100010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_And,                    0b00100011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_AndAccumulator,         0b00100100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_AndAccumulator,         0b00100101, Flag_W, 0, { DATA, DATA_IF_W } },
    { Instruction_Segment,                0b00100110, 0, 0b00, { 0 } },
    { Instruction_Daa,                    0b00100111, 0, 0, { 0 } },
    { Instruction_Sub,                    0b00101000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sub,                    0b00101001, Flag_W, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sub,                    0b00101010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sub,                    0b00101011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_SubAccumulator,         0b00101100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_SubAccumulator,         0b00101101, Flag_W, 0, { DATA, DATA_IF_W } },
    { Instruction_Segment,                0b00101110, 0, 0b01, { 0 } },
    { Instruction_Das,                    0b00101111, 0, 0, { 0 } },
    { Instruction_Xor,                    0b00110000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xor,                    0b00110001, Flag_W, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xor,                    0b00110010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xor,                    0b00110011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_XorAccumulator,         0b00110100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_XorAccumulator,         0b00110101, Flag_W, 0, { DATA, DATA_IF_W } },
    { Instruction_Segment,                0b00110110, 0, 0b10, { 0 } },
    { Instruction_Aaa,                    0b00110111, 0, 0, { 0 } },
    { Instruction_Cmp,                    0b00111000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Cmp,                    0b00111001, Flag_W, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Cmp,                    0b00111010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Cmp,                    0b00111011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_CmpAccumulator,         0b00111100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_CmpAccumulator,         0b00111101, Flag_W, 0, { DATA, DATA_IF_W } },
    { Instruction_Segment,                0b00111110, 0, 0b11, { 0 } },
    { Instruction_Aas,                    0b00111111, 0, 0, { 0 } },
    { Instruction_Inc,                    0b01000000, 0, 0b000,      { 0 } },
    { Instruction_Inc,                    0b01000001, 0, 0b001,      { 0 } },
    { Instruction_Inc,                    0b01000010, 0, 0b010,      { 0 } },
    { Instruction_Inc,                    0b01000011, 0, 0b011,      { 0 } },
    { Instruction_Inc,                    0b01000100, 0, 0b100,      { 0 } },
    { Instruction_Inc,                    0b01000101, 0, 0b101,      { 0 } },
    { Instruction_Inc,                    0b01000110, 0, 0b110,      { 0 } },
    { Instruction_Inc,                    0b01000111, 0, 0b111,      { 0 } },
    { Instruction_Dec,                    0b01001000, 0, 0b000,      { 0 } },
    { Instruction_Dec,                    0b01001001, 0, 0b001,      { 0 } },
    { Instruction_Dec,                    0b01001010, 0, 0b010,      { 0 } },
    { Instruction_Dec,                    0b01001011, 0, 0b011,      { 0 } },
    { Instruction_Dec,                    0b01001100, 0, 0b100,      { 0 } },
    { Instruction_Dec,                    0b01001101, 0, 0b101,      { 0 } },
    { Instruction_Dec,                    0b01001110, 0, 0b110,      { 0 } },
    { Instruction_Dec,                    0b01001111, 0, 0b111,      { 0 } },
    { Instruction_PushRegister,           0b01010000, 0, 0b000,      { 0 } },
    { Instruction_PushRegister,           0b01010001, 0, 0b001,      { 0 } },
    { Instruction_PushRegister,           0b01010010, 0, 0b010,      { 0 } },
    { Instruction_PushRegister,           0b01010011, 0, 0b011,      { 0 } },
    { Instruction_PushRegister,           0b01010100, 0, 0b100,      { 0 } },
    { Instruction_PushRegister,           0b01010101, 0, 0b101,      { 0 } },
    { Instruction_PushRegister,           0b01010110, 0, 0b110,      { 0 } },
    { Instruction_PushRegister,           0b01010111, 0, 0b111,      { 0 } },
    { Instruction_PopRegister,            0b01011000, 0, 0b000,      { 0 } },
    { Instruction_PopRegister,            0b01011001, 0, 0b001,      { 0 } },
    { Instruction_PopRegister,            0b01011010, 0, 0b010,      { 0 } },
    { Instruction_PopRegister,            0b01011011, 0, 0b011,      { 0 } },
    { Instruction_PopRegister,            0b01011100, 0, 0b100,      { 0 } },
    { Instruction_PopRegister,            0b01011101, 0, 0b101,      { 0 } },
    { Instruction_PopRegister,            0b01011110, 0, 0b110,      { 0 } },
    { Instruction_PopRegister,            0b01011111, 0, 0b111,      { 0 } },
    { Instruction_NOP,                    0b01100000, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01100001, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01100010, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01100011, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01100100, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01100101, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01100110, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01100111, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01101000, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01101001, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01101010, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01101011, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01101100, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01101101, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01101110, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b01101111, 0, 0, 0, { 0 } },
    { Instruction_Jo,                     0b01110000, 0, 0, { IP_INC8 } },
    { Instruction_Jno,                    0b01110001, 0, 0, { IP_INC8 } },
    { Instruction_Jb,                     0b01110010, 0, 0, { IP_INC8 } },
    { Instruction_Jnb,                    0b01110011, 0, 0, { IP_INC8 } },
    { Instruction_Je,                     0b01110100, 0, 0, { IP_INC8 } },
    { Instruction_Jne,                    0b01110101, 0, 0, { IP_INC8 } },
    { Instruction_Jbe,                    0b01110110, 0, 0, { IP_INC8 } },
    { Instruction_Ja,                     0b01110111, 0, 0, { IP_INC8 } },
    { Instruction_Js,                     0b01111000, 0, 0, { IP_INC8 } },
    { Instruction_Jns,                    0b01111001, 0, 0, { IP_INC8 } },
    { Instruction_Jp,                     0b01111010, 0, 0, { IP_INC8 } },
    { Instruction_Jnp,                    0b01111011, 0, 0, { IP_INC8 } },
    { Instruction_Jl,                     0b01111100, 0, 0, { IP_INC8 } },
    { Instruction_Jnl,                    0b01111101, 0, 0, { IP_INC8 } },
    { Instruction_Jle,                    0b01111110, 0, 0, { IP_INC8 } },
    { Instruction_Jg,                     0b01111111, 0, 0, { IP_INC8 } },
    { Instruction_Immediate,              0b10000000, 0, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_Immediate,              0b10000001, Flag_W, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_Immediate,              0b10000010, Flag_S, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA } },
    { Instruction_Immediate,              0b10000011, Flag_W | Flag_S, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA } },
    { Instruction_Test,                   0b10000100, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Test,                   0b10000101, Flag_W, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xchg,                   0b10000110, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xchg,                   0b10000111, Flag_W, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Mov,                    0b10001000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_Mov,                    0b10001001, Flag_W, 0, { MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_Mov,                    0b10001010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_Mov,                    0b10001011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_MovRegisterSegment,     0b10001100, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Lea,                    0b10001101, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_MovRegisterSegment,     0b10001110, Flag_D, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
    { Instruction_Pop,                    0b10001111, Flag_W, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
    { Instruction_XchgWithAccumulator,    0b10010000, 0, 0b000,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010001, 0, 0b001,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010010, 0, 0b010,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010011, 0, 0b011,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010100, 0, 0b100,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010101, 0, 0b101,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010110, 0, 0b110,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010111, 0, 0b111,      { 0 } },
    { Instruction_Cbw,                    0b10011000, 0, 0, { 0 } },
    { Instruction_Cwd,                    0b10011001, 0, 0, { 0 } },
    { Instruction_CallDirectIntersegment, 0b10011010, 0, 0, { IP_INC_LO, IP_INC_HI, CS_LO, CS_HI } },
    { Instruction_Wait,                   0b10011011, 0, 0, { 0 } },
    { Instruction_Pushf,                  0b10011100, 0, 0, { 0 } },
    { Instruction_Popf,                   0b10011101, 0, 0, { 0 } },
    { Instruction_Sahf,                   0b10011110, 0, 0, { 0 } },
    { Instruction_Lahf,                   0b10011111, 0, 0, { 0 } },
    { Instruction_Mov,                    0b10100000, 0, 0, { ADDR_LO, ADDR_HI } },
    { Instruction_Mov,                    0b10100001, Flag_W, 0, { ADDR_LO, ADDR_HI } },
    { Instruction_Mov,                    0b10100010, Flag_D, 0, { ADDR_LO, ADDR_HI } },
    { Instruction_Mov,                    0b10100011, Flag_W | Flag_D, 0, { ADDR_LO, ADDR_HI } },
    { Instruction_Movs,                   0b10100100, 0, 0, { 0 } },
    { Instruction_Movs,                   0b10100101, Flag_W, 0, { 0 } },
    { Instruction_Cmps,                   0b10100110, 0, 0, { 0 } },
    { Instruction_Cmps,                   0b10100111, Flag_W, 0, { 0 } },
    { Instruction_TestAccumulator,        0b10101000, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_TestAccumulator,        0b10101001, Flag_W, 0, { DATA, DATA_IF_W } },
    { Instruction_Stds,                   0b10101010, 0, 0, { 0 } },
    { Instruction_Stds,                   0b10101011, Flag_W, 0, { 0 } },
    { Instruction_Lods,                   0b10101100, 0, 0, { 0 } },
    { Instruction_Lods,                   0b10101101, Flag_W, 0, { 0 } },
    { Instruction_Scas,                   0b10101110, 0, 0, { 0 } },
    { Instruction_Scas,                   0b10101111, Flag_W, 0, { 0 } },
    { Instruction_MovImmediate,           0b10110000, 0, 0b000,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110001, 0, 0b001,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110010, 0, 0b010,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110011, 0, 0b011,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110100, 0, 0b100,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110101, 0, 0b101,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110110, 0, 0b110,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110111, 0, 0b111,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111000, Flag_W, 0b000, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111001, Flag_W, 0b001, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111010, Flag_W, 0b010, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111011, Flag_W, 0b011, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111100, Flag_W, 0b100, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111101, Flag_W, 0b101, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111110, Flag_W, 0b110, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111111, Flag_W, 0b111, { DATA, DATA_IF_W } },
    { Instruction_NOP,                    0b11000000, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11000001, 0, 0, 0, { 0 } },
    { Instruction_Ret,                    0b11000010, 0, 0, { DATA_LO, DATA_HI } },
    { Instruction_Ret,                    0b11000011, 0, 0, { 0 } },
    { Instruction_Les,                    0b11000100, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Lds,                    0b11000101, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_MovImmediateMemory,     0b11000110, 0, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_MovImmediateMemory,     0b11000111, Flag_W, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_NOP,                    0b11001000, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11001001, 0, 0, 0, { 0 } },
    { Instruction_RetfIntersegment,       0b11001010, 0, 0, { DATA_LO, DATA_HI } },
    { Instruction_Retf,                   0b11001011, 0, 0, { 0 } },
    { Instruction_Int3,                   0b11001100, 0, 0, { 0 } },
    { Instruction_Int,                    0b11001101, 0, 0, { DATA } },
    { Instruction_Into,                   0b11001110, 0, 0, { 0 } },
    { Instruction_Iret,                   0b11001111, 0, 0, { 0 } },
    { Instruction_Logic,                  0b11010000, 0, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
    { Instruction_Logic,                  0b11010001, Flag_W, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
    { Instruction_Logic,                  0b11010010, Flag_V, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
    { Instruction_Logic,                  0b11010011, Flag_W | Flag_V, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
    { Instruction_Aam,                    0b11010100, 0, 0, { DATA } },
    { Instruction_Aad,                    0b11010101, 0, 0, { DATA } },
    { Instruction_NOP,                    0b11010110, 0, 0, 0, { 0 } },
    { Instruction_Xlat,                   0b11010111, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11011000, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11011001, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11011010, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11011011, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11011100, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11011101, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11011110, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11011111, 0, 0, 0, { 0 } },
    { Instruction_Loopnz,                 0b11100000, 0, 0, { IP_INC8 } },
    { Instruction_Loopz,                  0b11100001, 0, 0, { IP_INC8 } },
    { Instruction_Loop,                   0b11100010, 0, 0, { IP_INC8 } },
    { Instruction_Jcxz,                   0b11100011, 0, 0, { IP_INC8 } },
    { Instruction_In,                     0b11100100, 0, 0, { DATA } },
    { Instruction_In,                     0b11100101, Flag_W, 0, { DATA } },
    { Instruction_Out,                    0b11100110, 0, 0, { DATA } },
    { Instruction_Out,                    0b11100111, Flag_W, 0, { DATA } },
    { Instruction_CallDirectWithin,       0b11101000, 0, 0, { IP_INC_LO, IP_INC_HI } },
    { Instruction_JmpDirectWithin,        0b11101001, 0, 0, { IP_INC_LO, IP_INC_HI } },
    { Instruction_JmpDirectIntersegment,  0b11101010, 0, 0, { IP_INC_LO, IP_INC_HI, CS_LO, CS_HI } },
    { Instruction_NOP,                    0b11101011, 0, 0, 0, { 0 } },
    { Instruction_In,                     0b11101100, 0, 0, { 0 } },
    { Instruction_In,                     0b11101101, Flag_W, 0, { 0 } },
    { Instruction_Out,                    0b11101110, 0, 0, { 0 } },
    { Instruction_Out,                    0b11101111, Flag_W, 0, { 0 } },
    { Instruction_Lock,                   0b11110000, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11110001, 0, 0, 0, { 0 } },
    { Instruction_Rep,                    0b11110010, 0, 0, { 0 } },
    { Instruction_Rep,                    0b11110011, Flag_V, 0, { 0 } },
    { Instruction_Hlt,                    0b11110100, 0, 0, { 0 } },
    { Instruction_Cmc,                    0b11110101, 0, 0, { 0 } },
    { Instruction_Arithmetic,             0b11110110, 0, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_Arithmetic,             0b11110111, Flag_W, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_Clc,                    0b11111000, 0, 0, { 0 } },
    { Instruction_Stc,                    0b11111001, 0, 0, { 0 } },
    { Instruction_Cli,                    0b11111010, 0, 0, { 0 } },
    { Instruction_Sti,                    0b11111011, 0, 0, { 0 } },
    { Instruction_Cld,                    0b11111100, 0, 0, { 0 } },
    { Instruction_Std,                    0b11111101, 0, 0, { 0 } },
    { Instruction_Control,                0b11111110, 0, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
    { Instruction_Control,                0b11111111, Flag_W, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
};

internal instruction
GetNextInstruction(simulator_context *Context)
{
    instruction Result = {0};
    if(Context->InstructionStreamAt < Context->InstructionStreamSize)
    {
        u8 Byte = Context->InstructionStream[Context->InstructionStreamAt++];
        
        instruction_table_entry TableEntry = GlobalInstructionTable[Byte];
        Assert(TableEntry.OpCode == Byte);
        
        s8 BitsAt = -1;
        Result.Type = TableEntry.Type;
        Result.Flags = TableEntry.Flags;
        Result.Generic = TableEntry.Generic;
        Result.Size = 1;
        
        for(u32 FieldIndex = 0;
            FieldIndex < ArrayCount(TableEntry.Fields);
            ++FieldIndex)
        {
            encoding Field = TableEntry.Fields[FieldIndex];
            if(Field.Type == Encoding_None)
            {
                // NOTE(kstandbridge): No more fields
                break;
            }
            
            if(Field.Type == Encoding_DATA_IF_W)
            {
                if((Result.Flags & Flag_W) == 0)
                {
                    // NOTE(kstandbridge): We are at the next op
                    break;
                }
            }
            else if((Field.Type == Encoding_DISP_LO) ||
                    (Field.Type == Encoding_DISP_HI))
            {
                if(Result.Bits[Encoding_MOD] == Mod_RegisterMode)
                {
                    continue;
                }
                else if((Result.Bits[Encoding_MOD] == Mod_MemoryMode) && 
                        (Result.Bits[Encoding_RM] == EffectiveAddress_DirectAddress))
                {
                    // NOTE(kstandbridge): Except when R/M = 110, then 16-bit displacement follows
                }
                else if((Field.Type == Encoding_DISP_LO) &&
                        (Result.Bits[Encoding_MOD] != Mod_8BitDisplace) &&
                        (Result.Bits[Encoding_MOD] != Mod_16BitDisplace))
                {
                    continue;
                }
                else if((Field.Type == Encoding_DISP_HI) &&
                        (Result.Bits[Encoding_MOD] != Mod_16BitDisplace))
                {
                    continue;
                }
            }
            
            if(BitsAt < 0)
            {
                BitsAt = 7;
                if(Context->InstructionStreamAt < Context->InstructionStreamSize)
                {
                    Byte = Context->InstructionStream[Context->InstructionStreamAt++];
                    ++Result.Size;
                }
                else
                {
                    break;
                }
            }
            
            u8 Bits = GetBits(Byte, BitsAt, Field.Size);
            Result.Bits[Field.Type] = Bits;
            BitsAt -= Field.Size;
        }
    }
    
    return Result;
}

inline string
InstructionToAssembly(memory_arena *Arena, simulator_context *Context, instruction Instruction)
{
    string Result;
    format_string_state State = BeginFormatString();
    
    string Op = InstructionToString(Instruction);
    b32 IsWord = ((Instruction.Flags & Flag_W) || 
                  (Instruction.Type == Instruction_Lea) || 
                  (Instruction.Type == Instruction_Lds) ||
                  (Instruction.Type == Instruction_Les));
    string Size = (IsWord) ? String("word") : String("byte");
    
    if(Context->IsNextOpLock)
    {
        AppendFormatString(&State, "lock ");
        Context->IsNextOpLock = false;
    }
    
    string SegmentPrefix = String("");
    if(Context->IsNextOpSegment)
    {
        SegmentPrefix = FormatString(Arena, "%S:", SegmentRegisterToString(Context->NextOpSegment));
        Context->IsNextOpSegment = false;
    }
    
    switch(Instruction.Type)
    {    
        case Instruction_In:
        case Instruction_Out:
        {
            string Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
            if((Instruction.Bits[Encoding_DATA] > 0))
            {
                u8 Value = Instruction.Bits[Encoding_DATA];
                if(Instruction.Type == Instruction_In)
                {
                    AppendFormatString(&State, "%S %S, %u", Op, Dest, Value);
                }
                else
                {
                    AppendFormatString(&State, "%S %u, %S", Op, Value, Dest);
                }
            }
            else
            {
                if(Instruction.Type == Instruction_In)
                {
                    AppendFormatString(&State, "%S %S, dx", Op, Dest);
                }
                else
                {
                    AppendFormatString(&State, "%S dx, %S", Op, Dest);
                }
            }
        } break;
        
        case Instruction_Rep:
        {
            instruction SubInstruction = GetNextInstruction(Context);
            if(SubInstruction.Type != Instruction_NOP)
            {
                string SubOp = InstructionToString(SubInstruction);
                string Suffix = (SubInstruction.Flags && Flag_W) ? String("w") : String("b");
                AppendFormatString(&State, "%S %S%S", Op, SubOp, Suffix);
            }
            else
            {
                AppendFormatString(&State, "%S ; ERROR expected sub instruction", Op);
            }
        } break;
        
        case Instruction_Je:
        case Instruction_Jl:
        case Instruction_Jle:
        case Instruction_Jb:
        case Instruction_Jbe:
        case Instruction_Jp:
        case Instruction_Jo:
        case Instruction_Js:
        case Instruction_Jne:
        case Instruction_Jnl:
        case Instruction_Jg:
        case Instruction_Jnb:
        case Instruction_Ja:
        case Instruction_Jnp:
        case Instruction_Jno:
        case Instruction_Jns:
        case Instruction_Loop:
        case Instruction_Loopz:
        case Instruction_Loopnz:
        case Instruction_Jcxz:
        {
            // NOTE(kstandbridge): We are just given an offset in the instruction stream to jmp
            s8 Value = *(s8 *)&Instruction.Bits[Encoding_IP_INC8];
            Value += Instruction.Size;
            AppendFormatString(&State, "%S $%d", Op, Value);
        } break;
        
        case Instruction_Ret:
        case Instruction_RetfIntersegment:
        {
            u8 ValueLow = Instruction.Bits[Encoding_DATA_LO];
            u8 ValueHigh = Instruction.Bits[Encoding_DATA_HI];
            u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
            s16 Value = *(s16 *)&ValueWide;
            
            if(Value)
            {
                AppendFormatString(&State, "%S %d", Op, Value);
            }
            else
            {
                AppendFormatString(&State, "%S", Op);
            }
        } break;
        
        case Instruction_JmpDirectWithin:
        case Instruction_CallDirectWithin:
        {
            u8 ValueLow = Instruction.Bits[Encoding_DATA_LO];
            u8 ValueHigh = Instruction.Bits[Encoding_DATA_HI];
            u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
            
            // NOTE(kstandbridge): We need to include the size of the op and encodings
            ValueWide += 3;
            
            AppendFormatString(&State, "%S %u", Op, ValueWide);
        } break;
        
        case Instruction_CallDirectIntersegment:
        case Instruction_JmpDirectIntersegment:
        {
            u8 ValueLow = Instruction.Bits[Encoding_DATA_LO];
            u8 ValueHigh = Instruction.Bits[Encoding_DATA_HI];
            u16 IPValue = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
            
            ValueLow = Instruction.Bits[Encoding_CS_LO];
            ValueHigh = Instruction.Bits[Encoding_CS_HI];
            u16 CSValue = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
            
            AppendFormatString(&State, "%S %u:%u", Op, CSValue, IPValue);
        } break;
        
        case Instruction_Aam:
        case Instruction_Aad:
        case Instruction_Int: // TODO(kstandbridge): I have not tested int for this, also test int with value 3
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
        } break;
        
        case Instruction_MovImmediateMemory:
        {
            s16 Value;
            
            if(Instruction.Flags && Flag_W)
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
            
            if(Instruction.Flags && Flag_W)
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
        } break;
        
        case Instruction_AddAccumulator:
        case Instruction_AdcAccumulator:
        case Instruction_SubAccumulator:
        case Instruction_SbbAccumulator:
        case Instruction_CmpAccumulator:
        case Instruction_AndAccumulator:
        case Instruction_TestAccumulator:
        case Instruction_OrAccumulator:
        case Instruction_XorAccumulator:
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
        }  break;
        
        case Instruction_PushSegmentRegister:
        case Instruction_PopSegmentRegister:
        {
            AppendFormatString(&State, "%S %S", Op, SegmentRegisterToString(Instruction.SegmentRegister));
        } break;
        
        case Instruction_XchgWithAccumulator:
        {
            AppendFormatString(&State, "%S ax, %S", Op, RegisterWordToString(Instruction.RegisterWord));
        } break;
        
        default:
        {
            if((Instruction.Bits[Encoding_DATA_HI] > 0) ||
               (Instruction.Bits[Encoding_DATA_LO] > 0))
            {
                u8 ValueLow = Instruction.Bits[Encoding_DATA_LO];
                u8 ValueHigh = Instruction.Bits[Encoding_DATA_HI];
                u16 Value = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                
                string Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_REG]) : RegisterByteToString(Instruction.Bits[Encoding_REG]);
                if(Instruction.Flags & Flag_D)
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
                switch(Instruction.Bits[Encoding_MOD])
                {
                    
                    case Mod_RegisterMode:
                    {
                        string Src = ((IsWord) || 
                                      (Instruction.Type == Instruction_MovRegisterSegment) ||
                                      (Instruction.Type == Instruction_Call) ||
                                      (Instruction.Type == Instruction_Jmp)) 
                            ? RegisterWordToString(Instruction.Bits[Encoding_RM]) : RegisterByteToString(Instruction.Bits[Encoding_RM]);
                        
                        if((Instruction.Type == Instruction_MovImmediate) ||
                           (Instruction.Type == Instruction_Immediate) ||
                           ((Instruction.Type == Instruction_Arithmetic) && 
                            (Instruction.Bits[Encoding_REG] == SubOp_Test)))
                        {
                            u16 Value;
                            s16 SignedValue;
                            if(Instruction.Bits[Encoding_DATA_IF_W] > 0)
                            {
                                u8 ValueLow = Instruction.Bits[Encoding_DATA];
                                u8 ValueHigh = Instruction.Bits[Encoding_DATA_IF_W];
                                Value = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
                                SignedValue = *(s16 *)&Value;
                            }
                            else
                            {
                                Value = Instruction.Bits[Encoding_DATA];
                                SignedValue = *(s8 *)&Instruction.Bits[Encoding_DATA];
                            }
                            
                            string Dest = (IsWord) ? RegisterWordToString(Instruction.Bits[Encoding_RM]) : RegisterByteToString(Instruction.Bits[Encoding_RM]);
                            
                            if(Instruction.Flags & Flag_S)
                            {                            
                                AppendFormatString(&State, "%S %S, %d", Op, Dest, SignedValue);
                            }
                            else
                            {
                                AppendFormatString(&State, "%S %S, %u", Op, Dest, Value);
                            }
                        }
                        else
                        {                
                            if((Instruction.Type == Instruction_Arithmetic) ||
                               (Instruction.Type == Instruction_Control))
                            {
                                AppendFormatString(&State, "%S %S", Op, Src);
                            }
                            else if(Instruction.Type == Instruction_Logic)
                            {
                                if(Instruction.Flags & Flag_V)
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
                            else if((Instruction.Type == Instruction_MovRegisterSegment))
                            {
                                string Dest = SegmentRegisterToString(Instruction.Bits[Encoding_REG]);
                                if(Instruction.Flags & Flag_D)
                                {
                                    AppendFormatString(&State, "%S %S, %S", Op, Dest, Src);
                                }
                                else
                                {
                                    AppendFormatString(&State, "%S %S, %S", Op, Src, Dest);
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
                            else if(Instruction.Type == Instruction_Logic)
                            {
                                if(Instruction.Flags & Flag_V)
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
                            else if((Instruction.Type == Instruction_Control) &&
                                    ((Instruction.Bits[Encoding_REG]) == SubOp_Jmp) ||
                                    ((Instruction.Bits[Encoding_REG]) == SubOp_Call))
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
                               (Instruction.Type == Instruction_Immediate) ||
                               ((Instruction.Type == Instruction_Arithmetic) && 
                                (Instruction.Bits[Encoding_REG] == SubOp_Test)))
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
                                    Src = (IsWord) ? RegisterWordToString(Instruction.RegisterWord) : RegisterByteToString(Instruction.RegisterByte);
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
                                if(Instruction.Flags & Flag_D)
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
                                    string Dest = RegisterWordToString(Instruction.RegisterWord);
                                    AppendFormatString(&State, "%S %S", Op, Dest);
                                }
                                else if(Instruction.Type == Instruction_PushSegmentRegister)
                                {
                                    string Dest = SegmentRegisterToString(Instruction.SegmentRegister);
                                    AppendFormatString(&State, "%S %S", Op, Dest);
                                }
                                else if(Instruction.Type == Instruction_Adc)
                                {
                                    AppendFormatString(&State, "%S dx, [%S]", Op, Src);
                                }
                                else if(Instruction.Type == Instruction_Logic)
                                {
                                    if(Instruction.Flags & Flag_V)
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
                                else if((Instruction.Type == Instruction_Control) &&
                                        (Instruction.Bits[Encoding_REG]) == SubOp_Jmp)
                                {
                                    AppendFormatString(&State, "%S [%S]", Op, Src);
                                }
                                else if((Instruction.Type == Instruction_Control) &&
                                        (Instruction.Bits[Encoding_REG]) == SubOp_IJmp)
                                {
                                    AppendFormatString(&State, "%S far [%S]", Op, Src);
                                }
                                else if((Instruction.Type == Instruction_Control) ||
                                        (Instruction.Type == Instruction_Arithmetic) ||
                                        (Instruction.Type == Instruction_Pop))
                                {
                                    AppendFormatString(&State, "%S %S [%S]", Op, Size, Src);
                                }
                                else
                                {
                                    AppendFormatString(&State, "%S", Op);
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
                        else if((Instruction.Flags & Flag_D) ||
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
                        else if(Instruction.Type == Instruction_Logic)
                        {
                            if(Instruction.Flags & Flag_V)
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
                        else if((Instruction.Type == Instruction_Immediate) ||
                                ((Instruction.Type == Instruction_Arithmetic) && 
                                 (Instruction.Bits[Encoding_REG] == SubOp_Test)))
                        {
                            u16 Data;
                            if(Instruction.Flags && Flag_W)
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
                        else if((Instruction.Type == Instruction_Control) &&
                                (Instruction.Bits[Encoding_REG]) == SubOp_Call)
                        {
                            AppendFormatString(&State, "%S %S", Op, Src);
                        }
                        else if((Instruction.Type == Instruction_MovRegisterSegment))
                        {
                            Dest = SegmentRegisterToString(Instruction.Bits[Encoding_REG]);
                            AppendFormatString(&State, "%S %S, %S", Op, Src, Dest);
                        }
                        else if((Instruction.Type == Instruction_Control) &&
                                (Instruction.Bits[Encoding_REG]) == SubOp_ICall)
                        {
                            AppendFormatString(&State, "%S far %S", Op, Src);
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
        } break;
        
    }
    
    Result = EndFormatString(&State, Arena);
    return Result;
}

inline simulator_context
GetSimulatorContext(u8 *InstructionStream, umm InstructionStreamSize)
{
    simulator_context Result =
    {
        .InstructionStream = InstructionStream,
        .InstructionStreamAt = 0,
        .InstructionStreamSize = InstructionStreamSize
    };
    return Result;
}
internal string
StreamToAssembly(memory_arena *Arena, u8 *InstructionStream, umm InstructionStreamSize)
{
    string Result;
    format_string_state StringState = BeginFormatString();
    
    simulator_context Context = GetSimulatorContext(InstructionStream, InstructionStreamSize);
    
    b32 First = false;
    for(;;)
    {
        instruction Instruction = GetNextInstruction(&Context);
        
        if(Instruction.Type == Instruction_Lock)
        {
            Context.IsNextOpLock = true;
        }
        else if(Instruction.Type == Instruction_Segment)
        {
            Context.IsNextOpSegment = true;
            Context.NextOpSegment = Instruction.SegmentRegister;
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

inline u16
SimulateArithmetic(simulator_context *Context, sub_op_type Op, u16 A, u16 B)
{
    u16 Result = A;
    
    b32 Auxiliary = false;
    
    if(Op == SubOp_Add)
    {
        if((GetBits(UnpackU16Low(A), 3, 1) == 0b1) &&
           (GetBits(UnpackU16Low(B), 3, 1) == 0b1))
        {
            Auxiliary = true;
        }
        
        Result += B;
    }
    else if((Op == SubOp_Sub) ||
            (Op == SubOp_Cmp))
    {
        Result -= B;
        
        u8 HighResult = GetBits(UnpackU16Low(Result), 3, 4);
        u8 HighA = GetBits(UnpackU16Low(A), 3, 4);
        if(HighResult > HighA)
        {
            Auxiliary = true;
        }
        
        if(A < B)
        {
            Context->Flags |= Flag_CF;
        }
    }
    
    if(Auxiliary)
    {
        Context->Flags |= Flag_AF;
    }
    
    u8 Low = UnpackU16Low(Result);
    u8 LowBits = CountSetBits(Low);
    if(LowBits % 2 == 0)
    {
        Context->Flags |= Flag_PF;
    }
    
    if((Result >> 15) == 1)
    {
        Context->Flags |= Flag_SF;
    }
    
    s16 ASigned = *(s16 *)&A;
    s16 ResultSigned = *(s16 *)&Result;
    b32 ANegative = ASigned < 0;
    b32 ResultNegative = ResultSigned < 0;
    
    if(Context->Flags & Flag_CF)
    {
        Context->Flags &= ~Flag_OF;
    }
    else if(ANegative != ResultNegative)
    {
        Context->Flags |= Flag_OF;
    }
    
    if(Result == 0)
    {
        Context->Flags |= Flag_ZF;
    }
    
    return Result;
}

internal instruction
SimulateStep(simulator_context *Context)
{
    instruction Result = GetNextInstruction(Context);
    
    if(Result.Type != Instruction_NOP)
    {
        local_persist memory_arena Arena;
        if(Context == 0)
        {
            string Assembly = InstructionToAssembly(&Arena, Context, Result);
            LogDebug("%S", Assembly);
            int x = 5;
            x;
        }
        
    }
    
    switch(Result.Type)
    {
        case Instruction_Jne:
        {
            if((Context->Flags & Flag_ZF) == 0)
            {
                s8 Offset = *(u8 *)&Result.Bits[Encoding_IP_INC8];
                Context->InstructionStreamAt += Offset;
            }
        } break;
        
        case Instruction_Immediate:
        {
            Context->Flags = 0;
            
            sub_op_type Op = Result.Bits[Encoding_REG];
            
            u8 HighPart = Result.Bits[Encoding_DATA_IF_W];
            u8 LowPart = Result.Bits[Encoding_DATA];
            u16 Source = PackU16(HighPart, LowPart);
            u16 Destination = Context->Registers[Result.Bits[Encoding_RM]];
            
            b32 Auxiliary = false;
            
            if(Result.Flags & Flag_S)
            {
                // NOTE(kstandbridge): This is not wide
                s8 SignedSource = *(u8 *)&Result.Bits[Encoding_DATA];
                s16 SignedDestination = *(u16 *)&Destination;
                s16 SignedOutput = SignedDestination;
                
                if(Op == SubOp_Add)
                {
                    if(SignedSource > 0)
                    {
                        if((GetBits(UnpackU16Low(SignedSource), 3, 1) == 0b1) &&
                           (GetBits(UnpackU16Low(SignedDestination), 3, 1) == 0b1))
                        {
                            Auxiliary = true;
                        }
                    }
                    else
                    {
                        if(GetBits(UnpackU16Low(SignedSource), 3, 1) != GetBits(UnpackU16Low(SignedDestination), 3, 1))
                        {
                            Auxiliary = true;
                        }
                    }
                    
                    SignedOutput = SignedSource + SignedDestination;
                    
                    
                    if(SignedOutput < SignedDestination)
                    {
                        Context->Flags |= Flag_CF;
                    }
                    
                }
                else if(Op == SubOp_Sub)
                {
                    SignedOutput = SignedDestination - SignedSource;
                }
                
                if(Auxiliary)
                {
                    Context->Flags |= Flag_AF;
                }
                
                u8 Low = UnpackU16Low(SignedOutput);
                u8 LowBits = CountSetBits(Low);
                if(LowBits % 2 == 0)
                {
                    Context->Flags |= Flag_PF;
                }
                
                if(Result.Flags == Flag_W)
                {
                    if((SignedOutput >> 15) == 1)
                    {
                        Context->Flags |= Flag_SF;
                    }
                }
                else
                {                    
                    if((SignedOutput >> 7) == 1)
                    {
                        Context->Flags |= Flag_SF;
                    }
                }
                
                b32 DestinationNegative = SignedDestination < 0;
                b32 OutputNegative = SignedOutput < 0;
                if(DestinationNegative != OutputNegative)
                {
                    Context->Flags |= Flag_OF;
                }
                
                if(SignedOutput == 0)
                {
                    Context->Flags |= Flag_ZF;
                }
                
                Context->Registers[Result.Bits[Encoding_RM]] = *(u16 *)&SignedOutput;
            }
            else
            {
                u16 Output = SimulateArithmetic(Context, Op, Destination, Source);
                
                Context->Registers[Result.Bits[Encoding_RM]] = Output;        
            }
            
            
        } break;
        
        case Instruction_Add:
        case Instruction_Sub:
        case Instruction_Cmp:
        {
            Context->Flags = 0;
            
            sub_op_type Op = SubOp_Cmp;
            u16 Source = Context->Registers[Result.Bits[Encoding_REG]];
            u16 Destination = Context->Registers[Result.Bits[Encoding_RM]];
            if(Result.Type == Instruction_Add)
            {
                Op = SubOp_Add;
            }
            else if(Result.Type == Instruction_Sub)
            {
                Op = SubOp_Sub;
            } 
            
            u16 Output = SimulateArithmetic(Context, Op, Destination, Source);
            
            if(Result.Type != Instruction_Cmp)
            {
                Context->Registers[Result.Bits[Encoding_RM]] = Output;
            }
            
        } break;
        
        case Instruction_MovImmediate:
        {
            Context->Flags = 0;
            
            if(Result.Flags & Flag_W)
            {
                Context->Registers[Result.RegisterWord] = PackU16(Result.Bits[Encoding_DATA_IF_W], Result.Bits[Encoding_DATA]);
            }
            else
            {
                u8 HighPart;
                u8 LowPart;
                
                u8 RegisterIndex = Result.RegisterByte;
                if(RegisterIndex > 4)
                {
                    RegisterIndex -= 4;
                    HighPart = Result.Bits[Encoding_DATA];
                    LowPart = UnpackU16Low(Context->Registers[RegisterIndex]);
                }
                else
                {
                    HighPart = UnpackU16High(Context->Registers[RegisterIndex]);
                    LowPart = Result.Bits[Encoding_DATA];
                }
                
                Context->Registers[RegisterIndex] = PackU16(HighPart, LowPart);
            }
        } break;
        
        case Instruction_MovRegisterSegment:
        {
            Context->Flags = 0;
            
            if(Result.Flags & Flag_D)
            {
                Context->SegmentRegisters[Result.Bits[Encoding_REG]] = Context->Registers[Result.Bits[Encoding_RM]];
            }
            else
            {
                Context->Registers[Result.Bits[Encoding_RM]] = Context->SegmentRegisters[Result.Bits[Encoding_REG]];
            }
        } break;
        
        case Instruction_Mov:
        {
            Context->Flags = 0;
            
            switch(Result.Bits[Encoding_MOD])
            {
                case Mod_MemoryMode:
                {
                } break;
                case Mod_8BitDisplace:
                {
                } break;
                case Mod_16BitDisplace:
                {
                } break;
                case Mod_RegisterMode:
                {
                    if(Result.Flags & Flag_W)
                    {                        
                        u16 Source = Context->Registers[Result.Bits[Encoding_REG]];
                        Context->Registers[Result.Bits[Encoding_RM]] = Source;
                    }
                    else
                    {
                        u8 DestIndex = Result.Bits[Encoding_RM];
                        u8 SourceIndex = Result.Bits[Encoding_REG];
                        if(SourceIndex > 3)
                        {
                            SourceIndex -= 4;
                            u8 LowPart = UnpackU16High(Context->Registers[SourceIndex]);
                            u8 HighPart = UnpackU16High(Context->Registers[DestIndex]);
                            Context->Registers[DestIndex] = PackU16(HighPart, LowPart);
                        }
                        else
                        {
                            u8 LowPart = UnpackU16Low(Context->Registers[SourceIndex]);
                            u8 HighPart;
                            if(DestIndex > 3)
                            {
                                DestIndex -= 4;
                                HighPart = UnpackU16Low(Context->Registers[DestIndex]);
                            }
                            else
                            {
                                HighPart = UnpackU16High(Context->Registers[DestIndex]);
                            }
                            
                            Context->Registers[DestIndex] = PackU16(LowPart, HighPart);
                        }
                    }
                } break;
                
                InvalidDefaultCase;
            }
        } break;
    }
    
    return Result;
}

internal void
Simulate(simulator_context *Context)
{
    for(;;)
    {
        instruction Instruction = SimulateStep(Context);
        if(Instruction.Type == Instruction_NOP)
        {
            break;
        } 
    }
}