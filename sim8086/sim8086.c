#include "sim8086.h"

#define B(Size, Bits) { Encoding_Bits, 3, Bits }

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
    { Instruction_PopRegister,            0b01011000, 8, 0, 0b000,      { 0 } },
    { Instruction_PopRegister,            0b01011001, 8, 0, 0b001,      { 0 } },
    { Instruction_PopRegister,            0b01011010, 8, 0, 0b010,      { 0 } },
    { Instruction_PopRegister,            0b01011011, 8, 0, 0b011,      { 0 } },
    { Instruction_PopRegister,            0b01011100, 8, 0, 0b100,      { 0 } },
    { Instruction_PopRegister,            0b01011101, 8, 0, 0b101,      { 0 } },
    { Instruction_PopRegister,            0b01011110, 8, 0, 0b110,      { 0 } },
    { Instruction_PopRegister,            0b01011111, 8, 0, 0b111,      { 0 } },
    { Instruction_PushRegister,           0b01010000, 8, 0, 0b000,      { 0 } },
    { Instruction_PushRegister,           0b01010001, 8, 0, 0b001,      { 0 } },
    { Instruction_PushRegister,           0b01010010, 8, 0, 0b010,      { 0 } },
    { Instruction_PushRegister,           0b01010011, 8, 0, 0b011,      { 0 } },
    { Instruction_PushRegister,           0b01010100, 8, 0, 0b100,      { 0 } },
    { Instruction_PushRegister,           0b01010101, 8, 0, 0b101,      { 0 } },
    { Instruction_PushRegister,           0b01010110, 8, 0, 0b110,      { 0 } },
    { Instruction_PushRegister,           0b01010111, 8, 0, 0b111,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010000, 8, 0, 0b000,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010001, 8, 0, 0b001,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010010, 8, 0, 0b010,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010011, 8, 0, 0b011,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010100, 8, 0, 0b100,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010101, 8, 0, 0b101,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010110, 8, 0, 0b110,      { 0 } },
    { Instruction_XchgWithAccumulator,    0b10010111, 8, 0, 0b111,      { 0 } },
    { Instruction_Inc,                    0b01000000, 8, 0, 0b000,      { 0 } },
    { Instruction_Inc,                    0b01000001, 8, 0, 0b001,      { 0 } },
    { Instruction_Inc,                    0b01000010, 8, 0, 0b010,      { 0 } },
    { Instruction_Inc,                    0b01000011, 8, 0, 0b011,      { 0 } },
    { Instruction_Inc,                    0b01000100, 8, 0, 0b100,      { 0 } },
    { Instruction_Inc,                    0b01000101, 8, 0, 0b101,      { 0 } },
    { Instruction_Inc,                    0b01000110, 8, 0, 0b110,      { 0 } },
    { Instruction_Inc,                    0b01000111, 8, 0, 0b111,      { 0 } },
    { Instruction_Dec,                    0b01001000, 8, 0, 0b000,      { 0 } },
    { Instruction_Dec,                    0b01001001, 8, 0, 0b001,      { 0 } },
    { Instruction_Dec,                    0b01001010, 8, 0, 0b010,      { 0 } },
    { Instruction_Dec,                    0b01001011, 8, 0, 0b011,      { 0 } },
    { Instruction_Dec,                    0b01001100, 8, 0, 0b100,      { 0 } },
    { Instruction_Dec,                    0b01001101, 8, 0, 0b101,      { 0 } },
    { Instruction_Dec,                    0b01001110, 8, 0, 0b110,      { 0 } },
    { Instruction_Dec,                    0b01001111, 8, 0, 0b111,      { 0 } },
    { Instruction_MovImmediate,           0b10110000, 8, 0, 0b000,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110001, 8, 0, 0b001,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110010, 8, 0, 0b010,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110011, 8, 0, 0b011,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110100, 8, 0, 0b100,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110101, 8, 0, 0b101,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110110, 8, 0, 0b110,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10110111, 8, 0, 0b111,      { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111000, 8, Flag_W, 0b000, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111001, 8, Flag_W, 0b001, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111010, 8, Flag_W, 0b010, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111011, 8, Flag_W, 0b011, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111100, 8, Flag_W, 0b100, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111101, 8, Flag_W, 0b101, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111110, 8, Flag_W, 0b110, { DATA, DATA_IF_W } },
    { Instruction_MovImmediate,           0b10111111, 8, Flag_W, 0b111, { DATA, DATA_IF_W } },
    { Instruction_PushSegmentRegister,    0b00000110, 8, 0, 0b00, { 0 } },
    { Instruction_PushSegmentRegister,    0b00001110, 8, 0, 0b01, { 0 } },
    { Instruction_PushSegmentRegister,    0b00010110, 8, 0, 0b10, { 0 } },
    { Instruction_PushSegmentRegister,    0b00011110, 8, 0, 0b11, { 0 } },
    { Instruction_PopSegmentRegister,     0b00000111, 8, 0, 0b00, { 0 } },
    { Instruction_PopSegmentRegister,     0b00001111, 8, 0, 0b01, { 0 } },
    { Instruction_PopSegmentRegister,     0b00010111, 8, 0, 0b10, { 0 } },
    { Instruction_PopSegmentRegister,     0b00011111, 8, 0, 0b11, { 0 } },
    { Instruction_Segment,                0b00100110, 8, 0, 0b00, { 0 } },
    { Instruction_Segment,                0b00101110, 8, 0, 0b01, { 0 } },
    { Instruction_Segment,                0b00110110, 8, 0, 0b10, { 0 } },
    { Instruction_Segment,                0b00111110, 8, 0, 0b11, { 0 } },
    { Instruction_AddImmediate,           0b10000000, 8, 0, 0,          { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_AddImmediate,           0b10000001, 8, Flag_W | 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_AddImmediate,           0b10000010, 8, Flag_S, 0,     { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_AddImmediate,           0b10000011, 8, Flag_W | Flag_S, 0,  { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_AdcImmediate,           0b10000000, 8, 0, 0, { MOD, B(3, 0b10), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_AdcImmediate,           0b10000001, 8, Flag_W | 0, 0, { MOD, B(3, 0b10), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_AdcImmediate,           0b10000010, 8, Flag_S, 0, { MOD, B(3, 0b10), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_AdcImmediate,           0b10000011, 8, Flag_W | Flag_S, 0, { MOD, B(3, 0b10), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_SubImmediate,           0b10000000, 8, 0, 0, { MOD, B(3, 0b101), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_SubImmediate,           0b10000001, 8, Flag_W | 0, 0, { MOD, B(3, 0b101), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_SubImmediate,           0b10000010, 8, Flag_S, 0, { MOD, B(3, 0b101), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_SubImmediate,           0b10000011, 8, Flag_W | Flag_S, 0, { MOD, B(3, 0b101), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_SbbImmediate,           0b10000000, 8, 0, 0, { MOD, B(3, 0b11), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_SbbImmediate,           0b10000001, 8, Flag_W | 0, 0, { MOD, B(3, 0b11), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_SbbImmediate,           0b10000010, 8, Flag_S, 0, { MOD, B(3, 0b11), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_SbbImmediate,           0b10000011, 8, Flag_W | Flag_S, 0, { MOD, B(3, 0b11), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_CmpImmediate,           0b10000000, 8, 0, 0, { MOD, B(3, 0b111), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_CmpImmediate,           0b10000001, 8, Flag_W | 0, 0, { MOD, B(3, 0b111), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_CmpImmediate,           0b10000010, 8, Flag_S, 0, { MOD, B(3, 0b111), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_CmpImmediate,           0b10000011, 8, Flag_W | Flag_S, 0, { MOD, B(3, 0b111), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_Mov,                    0b10001000, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_Mov,                    0b10001001, 8, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_Mov,                    0b10001010, 8, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_Mov,                    0b10001011, 8, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI  } },
    { Instruction_Mov,                    0b10100000, 8, 0, 0, { ADDR_LO, ADDR_HI } },
    { Instruction_Mov,                    0b10100001, 8, Flag_W | 0, 0, { ADDR_LO, ADDR_HI } },
    { Instruction_Mov,                    0b10100010, 8, Flag_D, 0, { ADDR_LO, ADDR_HI } },
    { Instruction_Mov,                    0b10100011, 8, Flag_W | Flag_D, 0, { ADDR_LO, ADDR_HI } },
    { Instruction_Add,                    0b00000000, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Add,                    0b00000001, 8, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Add,                    0b00000010, 8, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Add,                    0b00000011, 8, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sub,                    0b00101000, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sub,                    0b00101001, 8, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sub,                    0b00101010, 8, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sub,                    0b00101011, 8, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Adc,                    0b00010000, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Adc,                    0b00010001, 8, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Adc,                    0b00010010, 8, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Adc,                    0b00010011, 8, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sbb,                    0b00011000, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sbb,                    0b00011001, 8, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sbb,                    0b00011010, 8, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Sbb,                    0b00011011, 8, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Cmp,                    0b00111000, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Cmp,                    0b00111001, 8, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Cmp,                    0b00111010, 8, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Cmp,                    0b00111011, 8, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_And,                    0b00100000, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_And,                    0b00100001, 8, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_And,                    0b00100010, 8, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_And,                    0b00100011, 8, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Or,                     0b00001000, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Or,                     0b00001001, 8, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Or,                     0b00001010, 8, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Or,                     0b00001011, 8, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xor,                    0b00110000, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xor,                    0b00110001, 8, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xor,                    0b00110010, 8, Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xor,                    0b00110011, 8, Flag_W | Flag_D, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Shl,                    0b11010000, 8, 0, 0, { MOD, B(3, 0b100), RM, DISP_LO, DISP_HI } },
    { Instruction_Shl,                    0b11010001, 8, Flag_W | 0, 0, { MOD, B(3, 0b100), RM, DISP_LO, DISP_HI } },
    { Instruction_Shl,                    0b11010010, 8, Flag_Z, 0, { MOD, B(3, 0b100), RM, DISP_LO, DISP_HI } },
    { Instruction_Shl,                    0b11010011, 8, Flag_W | Flag_Z, 0, { MOD, B(3, 0b100), RM, DISP_LO, DISP_HI } },
    { Instruction_Shr,                    0b11010000, 8, 0, 0, { MOD, B(3, 0b101), RM, DISP_LO, DISP_HI } },
    { Instruction_Shr,                    0b11010001, 8, Flag_W | 0, 0, { MOD, B(3, 0b101), RM, DISP_LO, DISP_HI } },
    { Instruction_Shr,                    0b11010010, 8, Flag_Z, 0, { MOD, B(3, 0b101), RM, DISP_LO, DISP_HI } },
    { Instruction_Shr,                    0b11010011, 8, Flag_W | Flag_Z, 0, { MOD, B(3, 0b101), RM, DISP_LO, DISP_HI } },
    { Instruction_Sar,                    0b11010000, 8, 0, 0, { MOD, B(3, 0b111), RM, DISP_LO, DISP_HI } },
    { Instruction_Sar,                    0b11010001, 8, Flag_W | 0, 0, { MOD, B(3, 0b111), RM, DISP_LO, DISP_HI } },
    { Instruction_Sar,                    0b11010010, 8, Flag_Z, 0, { MOD, B(3, 0b111), RM, DISP_LO, DISP_HI } },
    { Instruction_Sar,                    0b11010011, 8, Flag_W | Flag_Z, 0, { MOD, B(3, 0b111), RM, DISP_LO, DISP_HI } },
    { Instruction_Rol,                    0b11010000, 8, 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI } },
    { Instruction_Rol,                    0b11010001, 8, Flag_W | 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI } },
    { Instruction_Rol,                    0b11010010, 8, Flag_Z, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI } },
    { Instruction_Rol,                    0b11010011, 8, Flag_W | Flag_Z, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI } },
    { Instruction_Ror,                    0b11010000, 8, 0, 0, { MOD, B(3, 0b01), RM, DISP_LO, DISP_HI } },
    { Instruction_Ror,                    0b11010001, 8, Flag_W | 0, 0, { MOD, B(3, 0b01), RM, DISP_LO, DISP_HI } },
    { Instruction_Ror,                    0b11010010, 8, Flag_Z, 0, { MOD, B(3, 0b01), RM, DISP_LO, DISP_HI } },
    { Instruction_Ror,                    0b11010011, 8, Flag_W | Flag_Z, 0, { MOD, B(3, 0b01), RM, DISP_LO, DISP_HI } },
    { Instruction_Rcl,                    0b11010000, 8, 0, 0, { MOD, B(3, 0b10), RM, DISP_LO, DISP_HI } },
    { Instruction_Rcl,                    0b11010001, 8, Flag_W | 0, 0, { MOD, B(3, 0b10), RM, DISP_LO, DISP_HI } },
    { Instruction_Rcl,                    0b11010010, 8, Flag_Z, 0, { MOD, B(3, 0b10), RM, DISP_LO, DISP_HI } },
    { Instruction_Rcl,                    0b11010011, 8, Flag_W | Flag_Z, 0, { MOD, B(3, 0b10), RM, DISP_LO, DISP_HI } },
    { Instruction_Rcr,                    0b11010000, 8, 0, 0, { MOD, B(3, 0b11), RM, DISP_LO, DISP_HI } },
    { Instruction_Rcr,                    0b11010001, 8, Flag_W | 0, 0, { MOD, B(3, 0b11), RM, DISP_LO, DISP_HI } },
    { Instruction_Rcr,                    0b11010010, 8, Flag_Z, 0, { MOD, B(3, 0b11), RM, DISP_LO, DISP_HI } },
    { Instruction_Rcr,                    0b11010011, 8, Flag_W | Flag_Z, 0, { MOD, B(3, 0b11), RM, DISP_LO, DISP_HI } },
    { Instruction_Not,                    0b11110110, 8, 0, 0, { MOD, B(3, 0b10), RM, DISP_LO, DISP_HI } },
    { Instruction_Not,                    0b11110111, 8, Flag_W | 0, 0, { MOD, B(3, 0b10), RM, DISP_LO, DISP_HI } },
    { Instruction_AndImmediate,           0b10000000, 8, 0, 0, { MOD, B(3, 0b100), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_AndImmediate,           0b10000001, 8, Flag_W | 0, 0, { MOD, B(3, 0b100), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_AndAccumulator,         0b00100100, 8, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_AndAccumulator,         0b00100101, 8, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_Test,                   0b10000100, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Test,                   0b10000101, 8, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_TestImmediate,          0b11110110, 8, 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_TestImmediate,          0b11110111, 8, Flag_W | 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_TestAccumulator,        0b10101000, 8, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_TestAccumulator,        0b10101001, 8, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_OrImmediate,            0b10000000, 8, 0, 0, { MOD, B(3, 0b01), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_OrImmediate,            0b10000001, 8, Flag_W | 0, 0, { MOD, B(3, 0b01), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_OrAccumulator,          0b00001100, 8, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_OrAccumulator,          0b00001101, 8, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_XorImmediate,           0b10000000, 8, 0, 0, { MOD, B(3, 0b110), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_XorImmediate,           0b10000001, 8, Flag_W | 0, 0, { MOD, B(3, 0b110), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_XorAccumulator,         0b00110100, 8, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_XorAccumulator,         0b00110101, 8, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_Movs,                   0b10100100, 8, 0, 0, { 0 } },
    { Instruction_Movs,                   0b10100101, 8, Flag_W | 0, 0, { 0 } },
    { Instruction_Cmps,                   0b10100110, 8, 0, 0, { 0 } },
    { Instruction_Cmps,                   0b10100111, 8, Flag_W | 0, 0, { 0 } },
    { Instruction_Scas,                   0b10101110, 8, 0, 0, { 0 } },
    { Instruction_Scas,                   0b10101111, 8, Flag_W | 0, 0, { 0 } },
    { Instruction_Lods,                   0b10101100, 8, 0, 0, { 0 } },
    { Instruction_Lods,                   0b10101101, 8, Flag_W | 0, 0, { 0 } },
    { Instruction_Stds,                   0b10101010, 8, 0, 0, { 0 } },
    { Instruction_Stds,                   0b10101011, 8, Flag_W | 0, 0, { 0 } },
    { Instruction_MovImmediateMemory,     0b11000110, 8, 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_MovImmediateMemory,     0b11000111, 8, Flag_W | 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI, DATA, DATA_IF_W } },
    { Instruction_Push,                   0b11111110, 8, 0, 0, { MOD, B(3, 0b110), RM, DISP_LO, DISP_HI } },
    { Instruction_Push,                   0b11111111, 8, Flag_W | 0, 0, { MOD, B(3, 0b110), RM, DISP_LO, DISP_HI } },
    { Instruction_Pop,                    0b10001110, 8, 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI } },
    { Instruction_Pop,                    0b10001111, 8, Flag_W | 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI } },
    { Instruction_Xchg,                   0b10000110, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xchg,                   0b10000111, 8, Flag_W | 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_In,                     0b11100100, 8, 0, 0, { DATA } },
    { Instruction_In,                     0b11100101, 8, Flag_W | 0, 0, { DATA } },
    { Instruction_In,                     0b11101100, 8, 0, 0, { 0 } },
    { Instruction_In,                     0b11101101, 8, Flag_W | 0, 0, { 0 } },
    { Instruction_Out,                    0b11100110, 8, 0, 0, { DATA } },
    { Instruction_Out,                    0b11100111, 8, Flag_W | 0, 0, { DATA } },
    { Instruction_Out,                    0b11101110, 8, 0, 0, { 0 } },
    { Instruction_Out,                    0b11101111, 8, Flag_W | 0, 0, { 0 } },
    { Instruction_AddAccumulator,         0b00000100, 8, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_AddAccumulator,         0b00000101, 8, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_AdcAccumulator,         0b00010100, 8, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_AdcAccumulator,         0b00010101, 8, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_Inc,                    0b11111110, 8, 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI } },
    { Instruction_Inc,                    0b11111111, 8, Flag_W | 0, 0, { MOD, B(3, 0b00), RM, DISP_LO, DISP_HI } },
    { Instruction_CmpAccumulator,         0b00111100, 8, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_CmpAccumulator,         0b00111101, 8, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_Neg,                    0b11110110, 8, 0, 0, { MOD, B(3, 0b11), RM, DISP_LO, DISP_HI } },
    { Instruction_Neg,                    0b11110111, 8, Flag_W | 0, 0, { MOD, B(3, 0b11), RM, DISP_LO, DISP_HI } },
    { Instruction_SubAccumulator,         0b00101100, 8, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_SubAccumulator,         0b00101101, 8, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_SbbAccumulator,         0b00011100, 8, 0, 0, { DATA, DATA_IF_W } },
    { Instruction_SbbAccumulator,         0b00011101, 8, Flag_W | 0, 0, { DATA, DATA_IF_W } },
    { Instruction_Dec,                    0b11111110, 8, 0, 0, { MOD, B(3, 0b01), RM, DISP_LO, DISP_HI } },
    { Instruction_Dec,                    0b11111111, 8, Flag_W | 0, 0, { MOD, B(3, 0b01), RM, DISP_LO, DISP_HI } },
    { Instruction_Mul,                    0b11110110, 8, 0, 0, { MOD, B(3, 0b100), RM, DISP_LO, DISP_HI } },
    { Instruction_Mul,                    0b11110111, 8, Flag_W | 0, 0, { MOD, B(3, 0b100), RM, DISP_LO, DISP_HI } },
    { Instruction_Imul,                   0b11110110, 8, 0, 0, { MOD, B(3, 0b101), RM, DISP_LO, DISP_HI } },
    { Instruction_Imul,                   0b11110111, 8, Flag_W | 0, 0, { MOD, B(3, 0b101), RM, DISP_LO, DISP_HI } },
    { Instruction_Div,                    0b11110110, 8, 0, 0, { MOD, B(3, 0b110), RM, DISP_LO, DISP_HI } },
    { Instruction_Div,                    0b11110111, 8, Flag_W | 0, 0, { MOD, B(3, 0b110), RM, DISP_LO, DISP_HI } },
    { Instruction_Idiv,                   0b11110110, 8, 0, 0, { MOD, B(3, 0b111), RM, DISP_LO, DISP_HI } },
    { Instruction_Idiv,                   0b11110111, 8, Flag_W | 0, 0, { MOD, B(3, 0b111), RM, DISP_LO, DISP_HI } },
    { Instruction_Rep,                    0b11110010, 8, 0, 0, { 0 } },
    { Instruction_Rep,                    0b11110011, 8, Flag_Z, 0, { 0 } },
    { Instruction_MovRegisterSegment,     0b10001100, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Xlat,                   0b11010111, 8, 0, 0, { 0 } },
    { Instruction_Lea,                    0b10001101, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Lds,                    0b11000101, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Les,                    0b11000100, 8, 0, 0, { MOD, REG, RM, DISP_LO, DISP_HI } },
    { Instruction_Lahf,                   0b10011111, 8, 0, 0, { 0 } },
    { Instruction_Sahf,                   0b10011110, 8, 0, 0, { 0 } },
    { Instruction_Pushf,                  0b10011100, 8, 0, 0, { 0 } },
    { Instruction_Popf,                   0b10011101, 8, 0, 0, { 0 } },
    { Instruction_Loop,                   0b11100010, 8, 0, 0, { IP_INC8 } },
    { Instruction_Loopz,                  0b11100001, 8, 0, 0, { IP_INC8 } },
    { Instruction_Loopnz,                 0b11100000, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jcxz,                   0b11100011, 8, 0, 0, { IP_INC8 } },
    { Instruction_Int,                    0b11001101, 8, 0, 0, { DATA } },
    { Instruction_Int3,                   0b11001100, 8, 0, 0, { 0 } },
    { Instruction_Into,                   0b11001110, 8, 0, 0, { 0 } },
    { Instruction_Iret,                   0b11001111, 8, 0, 0, { 0 } },
    { Instruction_Clc,                    0b11111000, 8, 0, 0, { 0 } },
    { Instruction_Cmc,                    0b11110101, 8, 0, 0, { 0 } },
    { Instruction_Stc,                    0b11111001, 8, 0, 0, { 0 } },
    { Instruction_Cld,                    0b11111100, 8, 0, 0, { 0 } },
    { Instruction_Std,                    0b11111101, 8, 0, 0, { 0 } },
    { Instruction_Cli,                    0b11111010, 8, 0, 0, { 0 } },
    { Instruction_Sti,                    0b11111011, 8, 0, 0, { 0 } },
    { Instruction_Hlt,                    0b11110100, 8, 0, 0, { 0 } },
    { Instruction_Wait,                   0b10011011, 8, 0, 0, { 0 } },
    { Instruction_Lock,                   0b11110000, 8, 0, 0, { 0 } },
    { Instruction_CallDirectWithin,       0b11101000, 8, 0, 0, { IP_INC_LO, IP_INC_HI } },
    { Instruction_CallIndirect,           0b11111111, 8, 0, 0, { MOD, B(3, 0b011), RM, DISP_LO, DISP_HI } },
    { Instruction_Call,                   0b11111111, 8, 0, 0, { MOD, B(3, 0b010), RM, DISP_LO, DISP_HI } },
    { Instruction_CallDirectIntersegment, 0b10011010, 8, 0, 0, { IP_INC_LO, IP_INC_HI, CS_LO, CS_HI } },
    { Instruction_Jmp,                    0b11111111, 8, 0, 0, { MOD, B(3, 0b100), RM, DISP_LO, DISP_HI } },
    { Instruction_JmpIndirect,            0b11111111, 8, 0, 0, { MOD, B(3, 0b101), RM, DISP_LO, DISP_HI } },
    { Instruction_JmpDirectIntersegment,  0b11101010, 8, 0, 0, { IP_INC_LO, IP_INC_HI, CS_LO, CS_HI } },
    { Instruction_JmpDirectWithin,        0b11101001, 8, 0, 0, { IP_INC_LO, IP_INC_HI } },
    { Instruction_Ret,                    0b11000010, 8, 0, 0, { DATA_LO, DATA_HI } },
    { Instruction_Ret,                    0b11000011, 8, 0, 0, { 0 } },
    { Instruction_Retf,                   0b11001011, 8, 0, 0, { 0 } },
    { Instruction_RetfIntersegment,       0b11001010, 8, 0, 0, { DATA_LO, DATA_HI } },
    { Instruction_Aaa,                    0b00110111, 8, 0, 0, { 0 } },
    { Instruction_Daa,                    0b00100111, 8, 0, 0, { 0 } },
    { Instruction_Aas,                    0b00111111, 8, 0, 0, { 0 } },
    { Instruction_Das,                    0b00101111, 8, 0, 0, { 0 } },
    { Instruction_Aam,                    0b11010100, 8, 0, 0, { DATA } },
    { Instruction_Aad,                    0b11010101, 8, 0, 0, { DATA } },
    { Instruction_Cbw,                    0b10011000, 8, 0, 0, { 0 } },
    { Instruction_Cwd,                    0b10011001, 8, 0, 0, { 0 } },
    { Instruction_Je,                     0b01110100, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jl,                     0b01111100, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jle,                    0b01111110, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jb,                     0b01110010, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jbe,                    0b01110110, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jp,                     0b01111010, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jo,                     0b01110000, 8, 0, 0, { IP_INC8 } },
    { Instruction_Js,                     0b01111000, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jne,                    0b01110101, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jnl,                    0b01111101, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jg,                     0b01111111, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jnb,                    0b01110011, 8, 0, 0, { IP_INC8 } },
    { Instruction_Ja,                     0b01110111, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jnp,                    0b01111011, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jno,                    0b01110001, 8, 0, 0, { IP_INC8 } },
    { Instruction_Jns,                    0b01111001, 8, 0, 0, { IP_INC8 } },
    
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
        for(u32 TableIndex = ArrayCount(GlobalInstructionTable);
            TableIndex >= 0;
            --TableIndex)
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
            Result.Type = TestEntry.Type;
            Result.OpCode = TestEntry.OpCode;
            Result.OpCodeSize = TestEntry.OpCodeSize;
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
    b32 IsWord = ((Instruction.Flags & Flag_W) || 
                  (Instruction.Type == Instruction_Lea) || 
                  (Instruction.Type == Instruction_Lds) ||
                  (Instruction.Type == Instruction_Les));
    string Size = (IsWord) ? String("word") : String("byte");
    
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
            string SubOp = InstructionToString(SubInstruction.Type);
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
                    else if((Instruction.Type == Instruction_Shl) ||
                            (Instruction.Type == Instruction_Shr) ||
                            (Instruction.Type == Instruction_Sar) ||
                            (Instruction.Type == Instruction_Rol) ||
                            (Instruction.Type == Instruction_Ror) ||
                            (Instruction.Type == Instruction_Rcl) ||
                            (Instruction.Type == Instruction_Rcr))
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
                        else if((Instruction.Type == Instruction_Shl) ||
                                (Instruction.Type == Instruction_Shr) ||
                                (Instruction.Type == Instruction_Sar) ||
                                (Instruction.Type == Instruction_Rol) ||
                                (Instruction.Type == Instruction_Ror) ||
                                (Instruction.Type == Instruction_Rcl) ||
                                (Instruction.Type == Instruction_Rcr))
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
                        else if((Instruction.Type == Instruction_Jmp))
                        {
                            AppendFormatString(&State, "%S [%S]", Op, Src);
                        }
                        else if((Instruction.Type == Instruction_JmpIndirect))
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
                else if((Instruction.Type == Instruction_Shl) ||
                        (Instruction.Type == Instruction_Shr) ||
                        (Instruction.Type == Instruction_Sar) ||
                        (Instruction.Type == Instruction_Rol) ||
                        (Instruction.Type == Instruction_Ror) ||
                        (Instruction.Type == Instruction_Rcl) ||
                        (Instruction.Type == Instruction_Rcr))
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
                else if((Instruction.Type == Instruction_AndImmediate) ||
                        (Instruction.Type == Instruction_OrImmediate) ||
                        (Instruction.Type == Instruction_TestImmediate) ||
                        (Instruction.Type == Instruction_SbbImmediate) ||
                        (Instruction.Type == Instruction_XorImmediate))
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
                else if(Instruction.Type == Instruction_Call)
                {
                    AppendFormatString(&State, "%S %S", Op, Src);
                }
                else if((Instruction.Type == Instruction_MovRegisterSegment))
                {
                    Dest = SegmentRegisterToString(Instruction.Bits[Encoding_REG]);
                    AppendFormatString(&State, "%S %S, %S", Op, Src, Dest);
                }
                else if((Instruction.Type == Instruction_CallIndirect))
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
