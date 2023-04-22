#include "sim8086.h"

#define B(Size, Bits) { Encoding_Bits, 3, Bits }

#define TYPE { Encoding_Type, 3 }

#define MOD { Encoding_MOD, 2 }
#define REG { Encoding_REG, 3 }
#define RM { Encoding_RM, 3 }

#define DISP_LO { Encoding_DISP_LO, 8 }
#define DISP_HI { Encoding_DISP_HI, 8 }

#define DATA { Encoding_DATA, 8 }
#define DATA_IF_W { Encoding_DATA_IF_W, 8 }

#define ADDR_LO { Encoding_ADDR_LO, 8 }
#define ADDR_HI { Encoding_ADDR_HI, 8 }

#define DATA_LO { Encoding_DATA_LO, 8 }
#define DATA_HI { Encoding_DATA_HI, 8 }

#define IP_INC8 { Encoding_IP_INC8, 8 }

#define IP_INC_LO { Encoding_IP_INC_LO, 8 }
#define IP_INC_HI { Encoding_IP_INC_HI, 8 }

#define CS_LO { Encoding_CS_LO, 8 }
#define CS_HI { Encoding_CS_HI, 8 }

global instruction_table_entry GlobalInstructionTable[] =
{
    { Instruction_Add,                    0b00000000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Add,                    0b00000001, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Add,                    0b00000010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Add,                    0b00000011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_AddAccumulator,         0b00000100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_AddAccumulator,         0b00000101, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_PushSegmentRegister,    0b00000110, 0, 0b00, { 0 } },
    { Instruction_PopSegmentRegister,     0b00000111, 0, 0b00, { 0 } },
    { Instruction_Or,                     0b00001000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Or,                     0b00001001, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Or,                     0b00001010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Or,                     0b00001011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_OrAccumulator,          0b00001100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_OrAccumulator,          0b00001101, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_PushSegmentRegister,    0b00001110, 0, 0b01, { 0 } },
    { Instruction_PopSegmentRegister,     0b00001111, 0, 0b01, { 0 } },
    { Instruction_Adc,                    0b00010000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Adc,                    0b00010001, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Adc,                    0b00010010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Adc,                    0b00010011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_AdcAccumulator,         0b00010100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_AdcAccumulator,         0b00010101, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_PushSegmentRegister,    0b00010110, 0, 0b10, { 0 } },
    { Instruction_PopSegmentRegister,     0b00010111, 0, 0b10, { 0 } },
    { Instruction_Sbb,                    0b00011000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sbb,                    0b00011001, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sbb,                    0b00011010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sbb,                    0b00011011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_SbbAccumulator,         0b00011100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_SbbAccumulator,         0b00011101, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_PushSegmentRegister,    0b00011110, 0, 0b11, { 0 } },
    { Instruction_PopSegmentRegister,     0b00011111, 0, 0b11, { 0 } },
    { Instruction_And,                    0b00100000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_And,                    0b00100001, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_And,                    0b00100010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_And,                    0b00100011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_AndAccumulator,         0b00100100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_AndAccumulator,         0b00100101, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_Segment,                0b00100110, 0, 0b00, { 0 } },
    { Instruction_Daa,                    0b00100111, 0, 0, { 0 } },
    { Instruction_Sub,                    0b00101000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sub,                    0b00101001, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sub,                    0b00101010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sub,                    0b00101011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_SubAccumulator,         0b00101100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_SubAccumulator,         0b00101101, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_Segment,                0b00101110, 0, 0b01, { 0 } },
    { Instruction_Das,                    0b00101111, 0, 0, { 0 } },
    { Instruction_Xor,                    0b00110000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xor,                    0b00110001, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xor,                    0b00110010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xor,                    0b00110011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_XorAccumulator,         0b00110100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_XorAccumulator,         0b00110101, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_Segment,                0b00110110, 0, 0b10, { 0 } },
    { Instruction_Aaa,                    0b00110111, 0, 0, { 0 } },
    { Instruction_Cmp,                    0b00111000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Cmp,                    0b00111001, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Cmp,                    0b00111010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Cmp,                    0b00111011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_CmpAccumulator,         0b00111100, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_CmpAccumulator,         0b00111101, Flag_W | 0, 0, { DATA, DATA_IF_W } },
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
    { Instruction_Immediate,              0b10000001, Flag_W | 0, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_Immediate,              0b10000010, Flag_S, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_Immediate,              0b10000011, Flag_W | Flag_S, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_Test,                   0b10000100, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Test,                   0b10000101, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xchg,                   0b10000110, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xchg,                   0b10000111, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Mov,                    0b10001000, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_Mov,                    0b10001001, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_Mov,                    0b10001010, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_Mov,                    0b10001011, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_MovRegisterSegment,     0b10001100, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Lea,                    0b10001101, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Pop,                    0b10001110, 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI } },
    { Instruction_Pop,                    0b10001111, Flag_W | 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI } },
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
    { Instruction_Mov,                    0b10100001, Flag_W | 0, 0, { ADDR_LO, ADDR_HI } },
    { Instruction_Mov,                    0b10100010, Flag_D, 0, { ADDR_LO, ADDR_HI } },
    { Instruction_Mov,                    0b10100011, Flag_W | Flag_D, 0, { ADDR_LO, ADDR_HI } },
    { Instruction_Movs,                   0b10100100, 0, 0, { 0 } },
    { Instruction_Movs,                   0b10100101, Flag_W | 0, 0, { 0 } },
    { Instruction_Cmps,                   0b10100110, 0, 0, { 0 } },
    { Instruction_Cmps,                   0b10100111, Flag_W | 0, 0, { 0 } },
    { Instruction_TestAccumulator,        0b10101000, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_TestAccumulator,        0b10101001, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_Stds,                   0b10101010, 0, 0, { 0 } },
    { Instruction_Stds,                   0b10101011, Flag_W | 0, 0, { 0 } },
    { Instruction_Lods,                   0b10101100, 0, 0, { 0 } },
    { Instruction_Lods,                   0b10101101, Flag_W | 0, 0, { 0 } },
    { Instruction_Scas,                   0b10101110, 0, 0, { 0 } },
    { Instruction_Scas,                   0b10101111, Flag_W | 0, 0, { 0 } },
    { Instruction_NOP,                    0b10110000, 0, 0, 0, { 0} },
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
    { Instruction_MovImmediateMemory,     0b11000110, 0, 0, { MOD, B(3, 0b000), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_MovImmediateMemory,     0b11000111, Flag_W | 0, 0, { MOD, B(3, 0b000), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_NOP,                    0b11001000, 0, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11001001, 0, 0, 0, { 0 } },
    { Instruction_RetfIntersegment,       0b11001010, 0, 0, { DATA_LO, DATA_HI } },
    { Instruction_Retf,                   0b11001011, 0, 0, { 0 } },
    { Instruction_Int3,                   0b11001100, 0, 0, { 0 } },
    { Instruction_Int,                    0b11001101, 0, 0, { DATA } },
    { Instruction_Into,                   0b11001110, 0, 0, { 0 } },
    { Instruction_Iret,                   0b11001111, 0, 0, { 0 } },
    { Instruction_Logic,                  0b11010000, 0, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
    { Instruction_Logic,                  0b11010001, Flag_W | 0, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
    { Instruction_Logic,                  0b11010010, Flag_Z, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
    { Instruction_Logic,                  0b11010011, Flag_W | Flag_Z, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
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
    { Instruction_In,                     0b11100101, Flag_W | 0, 0, { DATA } },
    { Instruction_Out,                    0b11100110, 0, 0, { DATA } },
    { Instruction_Out,                    0b11100111, Flag_W | 0, 0, { DATA } },
    { Instruction_CallDirectWithin,       0b11101000, 0, 0, { IP_INC_LO, IP_INC_HI } },
    { Instruction_JmpDirectWithin,        0b11101001, 0, 0, { IP_INC_LO, IP_INC_HI } },
    { Instruction_JmpDirectIntersegment,  0b11101010, 0, 0, { IP_INC_LO, IP_INC_HI, CS_LO, CS_HI } },
    { Instruction_NOP,                    0b11101011, 0, 0, 0, { 0 } },
    { Instruction_In,                     0b11101100, 0, 0, { 0 } },
    { Instruction_In,                     0b11101101, Flag_W | 0, 0, { 0 } },
    { Instruction_Out,                    0b11101110, 0, 0, { 0 } },
    { Instruction_Out,                    0b11101111, Flag_W | 0, 0, { 0 } },
    { Instruction_Lock,                   0b11110000, 0, 0, { 0 } },
    { Instruction_NOP,                    0b11110001, 0, 0, 0, { 0 } },
    { Instruction_Rep,                    0b11110010, 0, 0, { 0 } },
    { Instruction_Rep,                    0b11110011, Flag_Z, 0, { 0 } },
    { Instruction_Hlt,                    0b11110100, 0, 0, { 0 } },
    { Instruction_Cmc,                    0b11110101, 0, 0, { 0 } },
    { Instruction_Arithmetic,             0b11110110, 0, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_Arithmetic,             0b11110111, Flag_W | 0, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_Clc,                    0b11111000, 0, 0, { 0 } },
    { Instruction_Stc,                    0b11111001, 0, 0, { 0 } },
    { Instruction_Cli,                    0b11111010, 0, 0, { 0 } },
    { Instruction_Sti,                    0b11111011, 0, 0, { 0 } },
    { Instruction_Cld,                    0b11111100, 0, 0, { 0 } },
    { Instruction_Std,                    0b11111101, 0, 0, { 0 } },
    { Instruction_Control,                0b11111110, 0, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
    { Instruction_Control,                0b11111111, Flag_W | 0, 0, { MOD, TYPE, RM, DISP_LO, DISP_HI } },
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
        
        instruction_table_entry TestEntry = GlobalInstructionTable[Op0];
        
        s8 BitsAt = -1;
        Result.Type = TestEntry.Type;
        Result.Flags = TestEntry.Flags;
        Result.Generic = TestEntry.Generic;
        Result.HasFieldData = false;
        
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
            Result.HasFieldData = true;
            
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
                if((Result.Flags & Flag_W) == 0)
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
        // TODO(kstandbridge): Do I want to hardcode this?
        AppendFormatString(&State, "lock ");
        Context->IsNextOpLock = false;
    }
    
    string SegmentPrefix = String("");
    if(Context->IsNextOpSegment)
    {
        SegmentPrefix = FormatString(Arena, "%S:", SegmentRegisterToString(Context->NextOpSegment));
        Context->IsNextOpSegment = false;
    }
    
    if(Instruction.Type == Instruction_In)
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
    else if(Instruction.Type == Instruction_Rep)
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
    }
    else if(((Instruction.Type != Instruction_PushSegmentRegister) &&
             (Instruction.Type != Instruction_PopSegmentRegister) &&
             (Instruction.Type != Instruction_PushRegister) &&
             (Instruction.Type != Instruction_PopRegister) &&
             (Instruction.Type != Instruction_XchgWithAccumulator) &&
             (Instruction.Type != Instruction_Inc) &&
             (Instruction.Type != Instruction_Control) &&
             (Instruction.Type != Instruction_Dec)) &&
            (!Instruction.HasFieldData))
    {
        AppendFormatString(&State, "%S", Op);
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
    else if((Instruction.Type == Instruction_Ret) ||
            (Instruction.Type == Instruction_RetfIntersegment))
    {
        u8 ValueLow = Instruction.Bits[Encoding_DATA_LO];
        u8 ValueHigh = Instruction.Bits[Encoding_DATA_HI];
        u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
        s16 Value = *(s16 *)&ValueWide;
        
        AppendFormatString(&State, "%S %d", Op, Value);
    }
    else if((Instruction.Type == Instruction_JmpDirectWithin) ||
            (Instruction.Type == Instruction_CallDirectWithin))
    {
        u8 ValueLow = Instruction.Bits[Encoding_IP_INC_LO];
        u8 ValueHigh = Instruction.Bits[Encoding_IP_INC_HI];
        u16 ValueWide = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
        
        // NOTE(kstandbridge): We need to include the size of the op and encodings
        ValueWide += 3;
        
        AppendFormatString(&State, "%S %u", Op, ValueWide);
    }
    else if((Instruction.Type == Instruction_CallDirectIntersegment) ||
            (Instruction.Type == Instruction_JmpDirectIntersegment))
    {
        u8 ValueLow = Instruction.Bits[Encoding_IP_INC_LO];
        u8 ValueHigh = Instruction.Bits[Encoding_IP_INC_HI];
        u16 IPValue = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
        
        ValueLow = Instruction.Bits[Encoding_CS_LO];
        ValueHigh = Instruction.Bits[Encoding_CS_HI];
        u16 CSValue = ((ValueHigh & 0xFF) << 8) | (ValueLow & 0xFF);
        
        AppendFormatString(&State, "%S %u:%u", Op, CSValue, IPValue);
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
        AppendFormatString(&State, "%S %S", Op, SegmentRegisterToString(Instruction.SegmentRegister));
    }
    else if(Instruction.Type == Instruction_XchgWithAccumulator)
    {
        AppendFormatString(&State, "%S ax, %S", Op, RegisterWordToString(Instruction.RegisterWord));
    }
    else if((Instruction.Bits[Encoding_ADDR_HI] > 0) ||
            (Instruction.Bits[Encoding_ADDR_LO] > 0))
    {
        u8 ValueLow = Instruction.Bits[Encoding_ADDR_LO];
        u8 ValueHigh = Instruction.Bits[Encoding_ADDR_HI];
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
                              (Instruction.Type == Instruction_Call) ||
                              (Instruction.Type == Instruction_Jmp)) 
                    ? RegisterWordToString(Instruction.Bits[Encoding_RM]) : RegisterByteToString(Instruction.Bits[Encoding_RM]);
                
                if((Instruction.Type == Instruction_MovImmediate) ||
                   (Instruction.Type == Instruction_Immediate) ||
                   ((Instruction.Type == Instruction_Arithmetic) && 
                    (Instruction.Bits[Encoding_Type] == SubOp_Test)))
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
                    if((Instruction.Type == Instruction_Arithmetic) ||
                       (Instruction.Type == Instruction_Control))
                    {
                        AppendFormatString(&State, "%S %S", Op, Src);
                    }
                    else if(Instruction.Type == Instruction_Logic)
                    {
                        if(Instruction.Flags & Flag_Z)
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
                    else if(Instruction.Type == Instruction_Logic)
                    {
                        if(Instruction.Flags & Flag_Z)
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
                            ((Instruction.Bits[Encoding_Type]) == SubOp_Jmp) ||
                            ((Instruction.Bits[Encoding_Type]) == SubOp_Call))
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
                        (Instruction.Bits[Encoding_Type] == SubOp_Test)))
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
                            if(Instruction.Flags & Flag_Z)
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
                                (Instruction.Bits[Encoding_Type]) == SubOp_Jmp)
                        {
                            AppendFormatString(&State, "%S [%S]", Op, Src);
                        }
                        else if((Instruction.Type == Instruction_Control) &&
                                (Instruction.Bits[Encoding_Type]) == SubOp_IJmp)
                        {
                            AppendFormatString(&State, "%S far [%S]", Op, Src);
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
                    
                    if(Instruction.Flags & Flag_Z)
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
                         (Instruction.Bits[Encoding_Type] == SubOp_Test)))
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
                        (Instruction.Bits[Encoding_Type]) == SubOp_Call)
                {
                    AppendFormatString(&State, "%S %S", Op, Src);
                }
                else if((Instruction.Type == Instruction_MovRegisterSegment))
                {
                    Dest = SegmentRegisterToString(Instruction.Bits[Encoding_REG]);
                    AppendFormatString(&State, "%S %S, %S", Op, Src, Dest);
                }
                else if((Instruction.Type == Instruction_Control) &&
                        (Instruction.Bits[Encoding_Type]) == SubOp_ICall)
                {
                    // TODO(kstandbridge): why is this far?
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
