#define KENGINE_WIN32
#define KENGINE_TEST
#define KENGINE_IMPLEMENTATION
#include "kengine.h"


#include "sim8086.h"
#include "sim8086.c"

inline void
RunInstructionTableTests()
{
    for(s32 TableIndex = 0;
        TableIndex < ArrayCount(GlobalInstructionTable);
        ++TableIndex)
    {
        instruction_table_entry TestEntry = GlobalInstructionTable[TableIndex];
        AssertEqualBits(TableIndex, TestEntry.OpCode);
    }
}

inline void
RunDisassembleTests()
{
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
    }
    
    {
        // NOTE(kstandbridge): pop ds
        u8 Buffer[] = { 0b00011111 };
        simulator_context Context = 
        {
            .InstructionStream = Buffer,
            .InstructionStreamAt = 0,
            .InstructionStreamSize = sizeof(Buffer)
        };
        instruction Instruction = GetNextInstruction(&Context);
        AssertEqualU32(Instruction_PopSegmentRegister, Instruction.Type);
    }
    
}

inline void
RunDisassembleToAssemblyTests(memory_arena *Arena)
{
    
    {
        u8 Stream[] = { 0b10001001, 0b11011110 };
        AssertEqualString(String("mov si, bx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001000, 0b11000110 };
        AssertEqualString(String("mov dh, al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10110001, 0b00001100 };
        AssertEqualString(String("mov cl, 12"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10110101, 0b11110100 };
        AssertEqualString(String("mov ch, -12"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10111001, 0b00001100, 0b00000000 };
        AssertEqualString(String("mov cx, 12"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10111001, 0b11110100, 0b11111111 };
        AssertEqualString(String("mov cx, -12"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10111010, 0b01101100, 0b00001111 };
        AssertEqualString(String("mov dx, 3948"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10111010, 0b10010100, 0b11110000 };
        AssertEqualString(String("mov dx, -3948"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001010, 0b00000000 };
        AssertEqualString(String("mov al, [bx + si]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001011, 0b00011011 };
        AssertEqualString(String("mov bx, [bp + di]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001011, 0b01010110, 0b00000000 };
        AssertEqualString(String("mov dx, [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001010, 0b01100000, 0b00000100 };
        AssertEqualString(String("mov ah, [bx + si + 4]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001010, 0b10000000, 0b10000111, 0b00010011 };
        AssertEqualString(String("mov al, [bx + si + 4999]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001001, 0b00001001 };
        AssertEqualString(String("mov [bx + di], cx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001000, 0b00001010 };
        AssertEqualString(String("mov [bp + si], cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001000, 0b01101110, 0b00000000 };
        AssertEqualString(String("mov [bp], ch"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001011, 0b01000001, 0b11011011 };
        AssertEqualString(String("mov ax, [bx + di - 37]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001001, 0b10001100, 0b11010100, 0b11111110 };
        AssertEqualString(String("mov [si - 300], cx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001011, 0b01010111, 0b11100000 };
        AssertEqualString(String("mov dx, [bx - 32]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000110, 0b00000011, 0b00000111 };
        AssertEqualString(String("mov [bp + di], byte 7"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000111, 0b10000101, 0b10000101, 0b00000011, 0b01011011, 0b00000001 };
        AssertEqualString(String("mov [di + 901], word 347"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001011, 0b00101110, 0b00000101, 0b00000000 };
        AssertEqualString(String("mov bp, [5]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001011, 0b00011110, 0b10000010, 0b00001101 };
        AssertEqualString(String("mov bx, [3458]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10100001, 0b11111011, 0b00001001 };
        AssertEqualString(String("mov ax, [2555]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10100001, 0b00010000, 0b00000000 };
        AssertEqualString(String("mov ax, [16]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10100011, 0b11111010, 0b00001001 };
        AssertEqualString(String("mov [2554], ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10100011, 0b00001111, 0b00000000 };
        AssertEqualString(String("mov [15], ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b00110010 };
        AssertEqualString(String("push word [bp + si]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b00110110, 0b10111000, 0b00001011 };
        AssertEqualString(String("push word [3000]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b01110001, 0b11100010 };
        AssertEqualString(String("push word [bx + di - 30]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01010001 };
        AssertEqualString(String("push cx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01010000 };
        AssertEqualString(String("push ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01010010 };
        AssertEqualString(String("push dx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00001110 };
        AssertEqualString(String("push cs"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001111, 0b00000010 };
        AssertEqualString(String("pop word [bp + si]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001111, 0b00000110, 0b00000011, 0b00000000 };
        AssertEqualString(String("pop word [3]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001111, 0b10000001, 0b01001000, 0b11110100 };
        AssertEqualString(String("pop word [bx + di - 3000]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01011100 };
        AssertEqualString(String("pop sp"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01011111 };
        AssertEqualString(String("pop di"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01011110 };
        AssertEqualString(String("pop si"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00011111 };
        AssertEqualString(String("pop ds"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000111, 0b10000110, 0b00011000, 0b11111100 };
        AssertEqualString(String("xchg ax, [bp - 1000]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000111, 0b01101111, 0b00110010 };
        AssertEqualString(String("xchg [bx + 50], bp"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10010000 };
        AssertEqualString(String("xchg ax, ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10010010 };
        AssertEqualString(String("xchg ax, dx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10010100 };
        AssertEqualString(String("xchg ax, sp"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10010110 };
        AssertEqualString(String("xchg ax, si"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10010111 };
        AssertEqualString(String("xchg ax, di"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000111, 0b11001010 };
        AssertEqualString(String("xchg cx, dx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000111, 0b11110001 };
        AssertEqualString(String("xchg si, cx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000110, 0b11001100 };
        AssertEqualString(String("xchg cl, ah"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11100100, 0b11001000 };
        AssertEqualString(String("in al, 200"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11101100 };
        AssertEqualString(String("in al, dx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11101101 };
        AssertEqualString(String("in ax, dx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11100111, 0b00101100 };
        AssertEqualString(String("out 44, ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11101110 };
        AssertEqualString(String("out dx, al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010111 };
        AssertEqualString(String("xlat"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001101, 0b10000001, 0b10001100, 0b00000101 };
        AssertEqualString(String("lea ax, [bx + di + 1420]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001101, 0b01011110, 0b11001110 };
        AssertEqualString(String("lea bx, [bp - 50]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001101, 0b10100110, 0b00010101, 0b11111100 };
        AssertEqualString(String("lea sp, [bp - 1003]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001101, 0b01111000, 0b11111001 };
        AssertEqualString(String("lea di, [bx + si - 7]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000101, 0b10000001, 0b10001100, 0b00000101 };
        AssertEqualString(String("lds ax, [bx + di + 1420]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000101, 0b01011110, 0b11001110 };
        AssertEqualString(String("lds bx, [bp - 50]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000101, 0b10100110, 0b00010101, 0b11111100 };
        AssertEqualString(String("lds sp, [bp - 1003]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000101, 0b01111000, 0b11111001 };
        AssertEqualString(String("lds di, [bx + si - 7]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000100, 0b10000001, 0b10001100, 0b00000101 };
        AssertEqualString(String("les ax, [bx + di + 1420]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000100, 0b01011110, 0b11001110 };
        AssertEqualString(String("les bx, [bp - 50]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000100, 0b10100110, 0b00010101, 0b11111100 };
        AssertEqualString(String("les sp, [bp - 1003]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000100, 0b01111000, 0b11111001 };
        AssertEqualString(String("les di, [bx + si - 7]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10011111 };
        AssertEqualString(String("lahf"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10011110 };
        AssertEqualString(String("sahf"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10011100 };
        AssertEqualString(String("pushf"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10011101 };
        AssertEqualString(String("popf"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00000011, 0b01001110, 0b00000000 };
        AssertEqualString(String("add cx, [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00000011, 0b00010000 };
        AssertEqualString(String("add dx, [bx + si]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00000000, 0b10100011, 0b10001000, 0b00010011 };
        AssertEqualString(String("add [bp + di + 5000], ah"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00000000, 0b00000111 };
        AssertEqualString(String("add [bx], al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000001, 0b11000100, 0b10001000, 0b00000001 };
        AssertEqualString(String("add sp, 392"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000011, 0b11000110, 0b00000101 };
        AssertEqualString(String("add si, 5"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00000101, 0b11101000, 0b00000011 };
        AssertEqualString(String("add ax, 1000"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000000, 0b11000100, 0b00011110 };
        AssertEqualString(String("add ah, 30"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00000100, 0b00001001 };
        AssertEqualString(String("add al, 9"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00000001, 0b11011001 };
        AssertEqualString(String("add cx, bx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00000000, 0b11000101 };
        AssertEqualString(String("add ch, al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00010011, 0b01001110, 0b00000000 };
        AssertEqualString(String("adc cx, [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00010011, 0b00010000 };
        AssertEqualString(String("adc dx, [bx + si]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00010000, 0b10100011, 0b10001000, 0b00010011 };
        AssertEqualString(String("adc [bp + di + 5000], ah"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00010000, 0b00000111 };
        AssertEqualString(String("adc [bx], al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000001, 0b11010100, 0b10001000, 0b00000001 };
        AssertEqualString(String("adc sp, 392"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000011, 0b11010110, 0b00000101 };
        AssertEqualString(String("adc si, 5"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00010101, 0b11101000, 0b00000011 };
        AssertEqualString(String("adc ax, 1000"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000000, 0b11010100, 0b00011110 };
        AssertEqualString(String("adc ah, 30"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00010100, 0b00001001 };
        AssertEqualString(String("adc al, 9"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00010001, 0b11011001 };
        AssertEqualString(String("adc cx, bx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00010000, 0b11000101 };
        AssertEqualString(String("adc ch, al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01000000 };
        AssertEqualString(String("inc ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01000001 };
        AssertEqualString(String("inc cx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111110, 0b11000110 };
        AssertEqualString(String("inc dh"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111110, 0b11000000 };
        AssertEqualString(String("inc al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111110, 0b11000100 };
        AssertEqualString(String("inc ah"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01000100 };
        AssertEqualString(String("inc sp"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01000111 };
        AssertEqualString(String("inc di"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111110, 0b10000110, 0b11101010, 0b00000011 };
        AssertEqualString(String("inc byte [bp + 1002]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b01000111, 0b00100111 };
        AssertEqualString(String("inc word [bx + 39]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111110, 0b01000000, 0b00000101 };
        AssertEqualString(String("inc byte [bx + si + 5]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b10000011, 0b11000100, 0b11011000 };
        AssertEqualString(String("inc word [bp + di - 10044]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b00000110, 0b10000101, 0b00100100 };
        AssertEqualString(String("inc word [9349]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111110, 0b01000110, 0b00000000 };
        AssertEqualString(String("inc byte [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110111 };
        AssertEqualString(String("aaa"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100111 };
        AssertEqualString(String("daa"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00101011, 0b01001110, 0b00000000 };
        AssertEqualString(String("sub cx, [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00101011, 0b00010000 };
        AssertEqualString(String("sub dx, [bx + si]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00101000, 0b10100011, 0b10001000, 0b00010011 };
        AssertEqualString(String("sub [bp + di + 5000], ah"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00101000, 0b00000111 };
        AssertEqualString(String("sub [bx], al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000001, 0b11101100, 0b10001000, 0b00000001 };
        AssertEqualString(String("sub sp, 392"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000011, 0b11101110, 0b00000101 };
        AssertEqualString(String("sub si, 5"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00101101, 0b11101000, 0b00000011 };
        AssertEqualString(String("sub ax, 1000"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000000, 0b11101100, 0b00011110 };
        AssertEqualString(String("sub ah, 30"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00101100, 0b00001001 };
        AssertEqualString(String("sub al, 9"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00101001, 0b11011001 };
        AssertEqualString(String("sub cx, bx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00101000, 0b11000101 };
        AssertEqualString(String("sub ch, al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00011011, 0b01001110, 0b00000000 };
        AssertEqualString(String("sbb cx, [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00011011, 0b00010000 };
        AssertEqualString(String("sbb dx, [bx + si]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00011000, 0b10100011, 0b10001000, 0b00010011 };
        AssertEqualString(String("sbb [bp + di + 5000], ah"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00011000, 0b00000111 };
        AssertEqualString(String("sbb [bx], al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000001, 0b11011100, 0b10001000, 0b00000001 };
        AssertEqualString(String("sbb sp, 392"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000011, 0b11011110, 0b00000101 };
        AssertEqualString(String("sbb si, 5"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00011101, 0b11101000, 0b00000011 };
        AssertEqualString(String("sbb ax, 1000"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000000, 0b11011100, 0b00011110 };
        AssertEqualString(String("sbb ah, 30"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00011100, 0b00001001 };
        AssertEqualString(String("sbb al, 9"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00011001, 0b11011001 };
        AssertEqualString(String("sbb cx, bx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00011000, 0b11000101 };
        AssertEqualString(String("sbb ch, al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01001000 };
        AssertEqualString(String("dec ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01001001 };
        AssertEqualString(String("dec cx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111110, 0b11001110 };
        AssertEqualString(String("dec dh"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111110, 0b11001000 };
        AssertEqualString(String("dec al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111110, 0b11001100 };
        AssertEqualString(String("dec ah"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01001100 };
        AssertEqualString(String("dec sp"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01001111 };
        AssertEqualString(String("dec di"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111110, 0b10001110, 0b11101010, 0b00000011 };
        AssertEqualString(String("dec byte [bp + 1002]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b01001111, 0b00100111 };
        AssertEqualString(String("dec word [bx + 39]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111110, 0b01001000, 0b00000101 };
        AssertEqualString(String("dec byte [bx + si + 5]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b10001011, 0b11000100, 0b11011000 };
        AssertEqualString(String("dec word [bp + di - 10044]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b00001110, 0b10000101, 0b00100100 };
        AssertEqualString(String("dec word [9349]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111110, 0b01001110, 0b00000000 };
        AssertEqualString(String("dec byte [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b11011000 };
        AssertEqualString(String("neg ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b11011001 };
        AssertEqualString(String("neg cx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b11011110 };
        AssertEqualString(String("neg dh"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b11011000 };
        AssertEqualString(String("neg al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b11011100 };
        AssertEqualString(String("neg ah"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b11011100 };
        AssertEqualString(String("neg sp"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b11011111 };
        AssertEqualString(String("neg di"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b10011110, 0b11101010, 0b00000011 };
        AssertEqualString(String("neg byte [bp + 1002]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b01011111, 0b00100111 };
        AssertEqualString(String("neg word [bx + 39]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b01011000, 0b00000101 };
        AssertEqualString(String("neg byte [bx + si + 5]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b10011011, 0b11000100, 0b11011000 };
        AssertEqualString(String("neg word [bp + di - 10044]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b00011110, 0b10000101, 0b00100100 };
        AssertEqualString(String("neg word [9349]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b01011110, 0b00000000 };
        AssertEqualString(String("neg byte [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    {
        u8 Stream[] = { 0b00111001, 0b11001011 };
        string Actual = StreamToAssembly(Arena, Stream, sizeof(Stream));
        AssertEqualString(String("cmp bx, cx"), Actual);
    }
    
    {
        u8 Stream[] = { 0b00111010, 0b10110110, 0b10000110, 0b00000001 };
        AssertEqualString(String("cmp dh, [bp + 390]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00111001, 0b01110110, 0b00000010 };
        AssertEqualString(String("cmp [bp + 2], si"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000000, 0b11111011, 0b00010100 };
        AssertEqualString(String("cmp bl, 20"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000000, 0b00111111, 0b00100010 };
        AssertEqualString(String("cmp byte [bx], 34"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00111101, 0b01100101, 0b01011101 };
        AssertEqualString(String("cmp ax, 23909"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00111111 };
        AssertEqualString(String("aas"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00101111 };
        AssertEqualString(String("das"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b11100000 };
        AssertEqualString(String("mul al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b11100001 };
        AssertEqualString(String("mul cx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b01100110, 0b00000000 };
        AssertEqualString(String("mul word [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b10100001, 0b11110100, 0b00000001 };
        AssertEqualString(String("mul byte [bx + di + 500]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    {
        u8 Stream[] = { 0b11110110, 0b11101101 };
        string Actual = StreamToAssembly(Arena, Stream, sizeof(Stream));
        AssertEqualString(String("imul ch"), Actual);
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b11101010 };
        AssertEqualString(String("imul dx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b00101111 };
        AssertEqualString(String("imul byte [bx]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b00101110, 0b00001011, 0b00100101 };
        AssertEqualString(String("imul word [9483]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010100, 0b11111111 };
        AssertEqualString(String("aam 255"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010100, 0b00001010 };
        AssertEqualString(String("aam"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010100, 0b00001011 };
        AssertEqualString(String("aam 11"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b11110011 };
        AssertEqualString(String("div bl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b11110100 };
        AssertEqualString(String("div sp"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b10110000, 0b10101110, 0b00001011 };
        AssertEqualString(String("div byte [bx + si + 2990]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b10110011, 0b11101000, 0b00000011 };
        AssertEqualString(String("div word [bp + di + 1000]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b11111000 };
        AssertEqualString(String("idiv ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b11111110 };
        AssertEqualString(String("idiv si"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b00111010 };
        AssertEqualString(String("idiv byte [bp + si]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b10111111, 0b11101101, 0b00000001 };
        AssertEqualString(String("idiv word [bx + 493]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010101, 0b00001010 };
        AssertEqualString(String("aad"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010101, 0b00001011 };
        AssertEqualString(String("aad 11"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010101, 0b00000001 };
        AssertEqualString(String("aad 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010101, 0b00101010 };
        AssertEqualString(String("aad 42"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10011000 };
        AssertEqualString(String("cbw"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10011001 };
        AssertEqualString(String("cwd"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b11010100 };
        AssertEqualString(String("not ah"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b11010011 };
        AssertEqualString(String("not bl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b11010100 };
        AssertEqualString(String("not sp"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b11010110 };
        AssertEqualString(String("not si"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110111, 0b01010110, 0b00000000 };
        AssertEqualString(String("not word [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b10010110, 0b10110001, 0b00100110 };
        AssertEqualString(String("not byte [bp + 9905]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010001, 0b11100101 };
        AssertEqualString(String("shl bp, 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010000, 0b11100100 };
        AssertEqualString(String("shl ah, 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010000, 0b11100110 };
        AssertEqualString(String("shl dh, 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010001, 0b01100110, 0b00000101 };
        AssertEqualString(String("shl word [bp + 5], 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010001, 0b10100110, 0b10011001, 0b00011001 };
        AssertEqualString(String("shl word [bp + 6553], 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010001, 0b11101000 };
        AssertEqualString(String("shr ax, 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010001, 0b11111011 };
        AssertEqualString(String("sar bx, 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010001, 0b11000001 };
        AssertEqualString(String("rol cx, 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010000, 0b11001110 };
        AssertEqualString(String("ror dh, 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010001, 0b11010100 };
        AssertEqualString(String("rcl sp, 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010001, 0b11011101 };
        AssertEqualString(String("rcr bp, 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010001, 0b01100110, 0b00000101 };
        AssertEqualString(String("shl word [bp + 5], 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010000, 0b10101000, 0b00111001, 0b11111111 };
        AssertEqualString(String("shr byte [bx + si - 199], 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010000, 0b10111001, 0b11010100, 0b11111110 };
        AssertEqualString(String("sar byte [bx + di - 300], 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010001, 0b01000110, 0b00000000 };
        AssertEqualString(String("rol word [bp], 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010001, 0b00001110, 0b01001010, 0b00010011 };
        AssertEqualString(String("ror word [4938], 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    {
        u8 Stream[] = { 0b11010000, 0b00010110, 0b00000011, 0b00000000 };
        AssertEqualString(String("rcl byte [3], 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010001, 0b00011111 };
        AssertEqualString(String("rcr word [bx], 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010010, 0b11100100 };
        AssertEqualString(String("shl ah, cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010011, 0b11101000 };
        AssertEqualString(String("shr ax, cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010011, 0b11111011 };
        AssertEqualString(String("sar bx, cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010011, 0b11000001 };
        AssertEqualString(String("rol cx, cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010010, 0b11001110 };
        AssertEqualString(String("ror dh, cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010011, 0b11010100 };
        AssertEqualString(String("rcl sp, cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010011, 0b11011101 };
        AssertEqualString(String("rcr bp, cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010011, 0b01100110, 0b00000101 };
        AssertEqualString(String("shl word [bp + 5], cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010011, 0b10101000, 0b00111001, 0b11111111 };
        AssertEqualString(String("shr word [bx + si - 199], cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010010, 0b10111001, 0b11010100, 0b11111110 };
        AssertEqualString(String("sar byte [bx + di - 300], cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    {
        u8 Stream[] = { 0b11010010, 0b01000110, 0b00000000 };
        AssertEqualString(String("rol byte [bp], cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010010, 0b00001110, 0b01001010, 0b00010011 };
        AssertEqualString(String("ror byte [4938], cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010010, 0b00010110, 0b00000011, 0b00000000 };
        AssertEqualString(String("rcl byte [3], cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11010011, 0b00011111 };
        AssertEqualString(String("rcr word [bx], cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100000, 0b11100000 };
        AssertEqualString(String("and al, ah"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100000, 0b11001101 };
        AssertEqualString(String("and ch, cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100001, 0b11110101 };
        AssertEqualString(String("and bp, si"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100001, 0b11100111 };
        AssertEqualString(String("and di, sp"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100100, 0b01011101 };
        AssertEqualString(String("and al, 93"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100101, 0b10101000, 0b01001111 };
        AssertEqualString(String("and ax, 20392"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100000, 0b01101010, 0b00001010 };
        AssertEqualString(String("and [bp + si + 10], ch"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100001, 0b10010001, 0b11101000, 0b00000011 };
        AssertEqualString(String("and [bx + di + 1000], dx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100011, 0b01011110, 0b00000000 };
        AssertEqualString(String("and bx, [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100011, 0b00001110, 0b00100000, 0b00010001 };
        AssertEqualString(String("and cx, [4384]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000000, 0b01100110, 0b11011001, 0b11101111 };
        AssertEqualString(String("and byte [bp - 39], 239"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000001, 0b10100000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        AssertEqualString(String("and word [bx + si - 4332], 10328"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000101, 0b11001011 };
        AssertEqualString(String("test bx, cx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000100, 0b10110110, 0b10000110, 0b00000001 };
        AssertEqualString(String("test dh, [bp + 390]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000101, 0b01110110, 0b00000010 };
        AssertEqualString(String("test [bp + 2], si"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b11000011, 0b00010100 };
        AssertEqualString(String("test bl, 20"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110110, 0b00000111, 0b00100010 };
        AssertEqualString(String("test byte [bx], 34"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10101001, 0b01100101, 0b01011101 };
        AssertEqualString(String("test ax, 23909"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00001000, 0b11100000 };
        AssertEqualString(String("or al, ah"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00001000, 0b11001101 };
        AssertEqualString(String("or ch, cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00001001, 0b11110101 };
        AssertEqualString(String("or bp, si"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00001001, 0b11100111 };
        AssertEqualString(String("or di, sp"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00001100, 0b01011101 };
        AssertEqualString(String("or al, 93"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00001101, 0b10101000, 0b01001111 };
        AssertEqualString(String("or ax, 20392"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00001000, 0b01101010, 0b00001010 };
        AssertEqualString(String("or [bp + si + 10], ch"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00001001, 0b10010001, 0b11101000, 0b00000011 };
        AssertEqualString(String("or [bx + di + 1000], dx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00001011, 0b01011110, 0b00000000 };
        AssertEqualString(String("or bx, [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00001011, 0b00001110, 0b00100000, 0b00010001 };
        AssertEqualString(String("or cx, [4384]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000000, 0b01001110, 0b11011001, 0b11101111 };
        AssertEqualString(String("or byte [bp - 39], 239"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000001, 0b10001000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        AssertEqualString(String("or word [bx + si - 4332], 10328"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110000, 0b11100000 };
        AssertEqualString(String("xor al, ah"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110000, 0b11001101 };
        AssertEqualString(String("xor ch, cl"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110001, 0b11110101 };
        AssertEqualString(String("xor bp, si"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110001, 0b11100111 };
        AssertEqualString(String("xor di, sp"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110100, 0b01011101 };
        AssertEqualString(String("xor al, 93"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110101, 0b10101000, 0b01001111 };
        AssertEqualString(String("xor ax, 20392"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110000, 0b01101010, 0b00001010 };
        AssertEqualString(String("xor [bp + si + 10], ch"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110001, 0b10010001, 0b11101000, 0b00000011 };
        AssertEqualString(String("xor [bx + di + 1000], dx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110011, 0b01011110, 0b00000000 };
        AssertEqualString(String("xor bx, [bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110011, 0b00001110, 0b00100000, 0b00010001 };
        AssertEqualString(String("xor cx, [4384]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000000, 0b01110110, 0b11011001, 0b11101111 };
        AssertEqualString(String("xor byte [bp - 39], 239"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000001, 0b10110000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        AssertEqualString(String("xor word [bx + si - 4332], 10328"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110011, 0b10100100 };
        AssertEqualString(String("rep movsb"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110011, 0b10100110 };
        AssertEqualString(String("rep cmpsb"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110011, 0b10101110 };
        AssertEqualString(String("rep scasb"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110011, 0b10101100 };
        AssertEqualString(String("rep lodsb"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110011, 0b10100101 };
        AssertEqualString(String("rep movsw"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110011, 0b10100111 };
        AssertEqualString(String("rep cmpsw"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110011, 0b10101111 };
        AssertEqualString(String("rep scasw"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110011, 0b10101101 };
        AssertEqualString(String("rep lodsw"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110011, 0b10101010 };
        AssertEqualString(String("rep stosb"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110011, 0b10101011 };
        AssertEqualString(String("rep stosw"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b00010110, 0b00100001, 0b10011001 };
        AssertEqualString(String("call [39201]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b01010110, 0b10011100 };
        AssertEqualString(String("call [bp - 100]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b11010100 };
        AssertEqualString(String("call sp"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b11010000 };
        AssertEqualString(String("call ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b11100000 };
        AssertEqualString(String("jmp ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b11100111 };
        AssertEqualString(String("jmp di"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b00100110, 0b00001100, 0b00000000 };
        AssertEqualString(String("jmp [12]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b00100110, 0b00101011, 0b00010001 };
        AssertEqualString(String("jmp [4395]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000010, 0b11111001, 0b11111111 };
        AssertEqualString(String("ret -7"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000010, 0b11110100, 0b00000001 };
        AssertEqualString(String("ret 500"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000011 };
        AssertEqualString(String("ret"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01110100, 0b11111110 };
        AssertEqualString(String("je label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01111100, 0b11111110 };
        AssertEqualString(String("jl label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01111110, 0b11111110 };
        AssertEqualString(String("jle label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01110010, 0b11111110 };
        AssertEqualString(String("jb label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01110110, 0b11111110 };
        AssertEqualString(String("jbe label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01111010, 0b11111110 };
        AssertEqualString(String("jp label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01110000, 0b11111110 };
        AssertEqualString(String("jo label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01111000, 0b11111110 };
        AssertEqualString(String("js label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01110101, 0b11111110 };
        AssertEqualString(String("jne label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01111101, 0b11111110 };
        AssertEqualString(String("jnl label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01111111, 0b11111110 };
        AssertEqualString(String("jg label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01110011, 0b11111110 };
        AssertEqualString(String("jnb label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01110111, 0b11111110 };
        AssertEqualString(String("ja label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01111011, 0b11111110 };
        AssertEqualString(String("jnp label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01110001, 0b11111110 };
        AssertEqualString(String("jno label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b01111001, 0b11111110 };
        AssertEqualString(String("jns label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11100010, 0b11111110 };
        AssertEqualString(String("loop label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11100001, 0b11111110 };
        AssertEqualString(String("loopz label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11100000, 0b11111110 };
        AssertEqualString(String("loopnz label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11100011, 0b11111110 };
        AssertEqualString(String("jcxz label"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11001101, 0b00001101 };
        AssertEqualString(String("int 13"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11001100 };
        AssertEqualString(String("int3"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11001110 };
        AssertEqualString(String("into"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11001111 };
        AssertEqualString(String("iret"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111000 };
        AssertEqualString(String("clc"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110101 };
        AssertEqualString(String("cmc"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111001 };
        AssertEqualString(String("stc"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111100 };
        AssertEqualString(String("cld"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111101 };
        AssertEqualString(String("std"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111010 };
        AssertEqualString(String("cli"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111011 };
        AssertEqualString(String("sti"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110100 };
        AssertEqualString(String("hlt"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10011011 };
        AssertEqualString(String("wait"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110000, 0b11110110, 0b10010110, 0b10110001, 0b00100110 };
        AssertEqualString(String("lock not byte [bp + 9905]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    {
        u8 Stream[] = { 0b10000110, 0b00000110, 0b01100100, 0b00000000 };
        AssertEqualString(String("xchg [100], al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110000, 0b10000110, 0b00000110, 0b01100100, 0b00000000 };
        AssertEqualString(String("lock xchg [100], al"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00101110, 0b10001010, 0b00000000 };
        AssertEqualString(String("mov al, cs:[bx + si]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00111110, 0b10001011, 0b00011011 };
        AssertEqualString(String("mov bx, ds:[bp + di]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100110, 0b10001011, 0b01010110, 0b00000000 };
        AssertEqualString(String("mov dx, es:[bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110110, 0b10001010, 0b01100000, 0b00000100 };
        AssertEqualString(String("mov ah, ss:[bx + si + 4]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00110110, 0b00100000, 0b01101010, 0b00001010 };
        AssertEqualString(String("and ss:[bp + si + 10], ch"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00111110, 0b00001001, 0b10010001, 0b11101000, 0b00000011 };
        AssertEqualString(String("or ds:[bx + di + 1000], dx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100110, 0b00110011, 0b01011110, 0b00000000 };
        AssertEqualString(String("xor bx, es:[bp]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00100110, 0b00111011, 0b00001110, 0b00100000, 0b00010001 };
        AssertEqualString(String("cmp cx, es:[4384]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00101110, 0b11110110, 0b01000110, 0b11011001, 0b11101111 };
        AssertEqualString(String("test byte cs:[bp - 39], 239"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10000001, 0b10011000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        AssertEqualString(String("sbb word [bx + si - 4332], 10328"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b00101110, 0b10000001, 0b10011000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        AssertEqualString(String("sbb word cs:[bx + si - 4332], 10328"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11110000, 0b00101110, 0b11110110, 0b10010110, 0b10110001, 0b00100110 };
        AssertEqualString(String("lock not byte cs:[bp + 9905]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10011010, 0b11001000, 0b00000001, 0b01111011, 0b00000000 };
        AssertEqualString(String("call 123:456"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11101010, 0b00100010, 0b00000000, 0b00010101, 0b00000011 };
        AssertEqualString(String("jmp 789:34"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001100, 0b01000000, 0b00111011 };
        AssertEqualString(String("mov [bx + si + 59], es"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11101001, 0b00111001, 0b00001010 };
        AssertEqualString(String("jmp 2620"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11101000, 0b00011001, 0b00101110 };
        AssertEqualString(String("call 11804"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11001010, 0b10010100, 0b01000100 };
        AssertEqualString(String("retf 17556"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000010, 0b10011000, 0b01000100 };
        AssertEqualString(String("ret 17560"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11001011 };
        AssertEqualString(String("retf"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11000011 };
        AssertEqualString(String("ret"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b01010010, 0b11000110 };
        AssertEqualString(String("call [bp + si - 58]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b01011010, 0b11000110 };
        AssertEqualString(String("call far [bp + si - 58]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b00100101 };
        AssertEqualString(String("jmp [di]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11111111, 0b00101101 };
        AssertEqualString(String("jmp far [di]"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b11101010, 0b10001000, 0b01110111, 0b01100110, 0b01010101 };
        AssertEqualString(String("jmp 21862:30600"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001001, 0b11011101, 0b10001001, 0b11001110 };
        AssertEqualString(String("mov bp, bx\nmov si, cx"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001110, 0b11010000 };
        AssertEqualString(String("mov ss, ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10110000, 0b00010001 };
        AssertEqualString(String("mov al, 17"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
    
    {
        u8 Stream[] = { 0b10001100, 0b11011101 };
        AssertEqualString(String("mov bp, ds"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
    }
}

inline void
RunImmediateMovTests(memory_arena *Arena)
{
    {
        u8 Stream[] = { 0b10111000, 0b00000001, 0b00000000 };
        AssertEqualString(String("mov ax, 1"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
        simulator_context Context = GetSimulatorContext(Stream, sizeof(Stream));
        AssertEqualU32(0, Context.Registers[RegisterWord_AX]);
        Simulate(&Context);
        AssertEqualU32(1, Context.Registers[RegisterWord_AX]);
    }
    
    {
        u8 Stream[] = { 0b10111011, 0b00000010, 0b00000000 };
        AssertEqualString(String("mov bx, 2"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
        simulator_context Context = GetSimulatorContext(Stream, sizeof(Stream));
        AssertEqualU32(0, Context.Registers[RegisterWord_BX]);
        Simulate(&Context);
        AssertEqualU32(2, Context.Registers[RegisterWord_BX]);
    }
    
    {
        u8 Stream[] = 
        { 
            0b10111000, 0b00000001, 0b00000000, 0b10111011, 0b00000010, 0b00000000, 0b10111001, 0b00000011, 0b00000000, 0b10111010, 0b00000100, 0b00000000, 0b10111100, 0b00000101, 0b00000000, 0b10111101, 0b00000110, 0b00000000, 0b10111110, 0b00000111, 0b00000000, 0b10111111, 0b00001000, 0b00000000 
        };
        AssertEqualString(String("mov ax, 1\nmov bx, 2\nmov cx, 3\nmov dx, 4\nmov sp, 5\nmov bp, 6\nmov si, 7\nmov di, 8"), 
                          StreamToAssembly(Arena, Stream, sizeof(Stream)));
        simulator_context Context = GetSimulatorContext(Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualU32(1, Context.Registers[RegisterWord_AX]);
        AssertEqualU32(2, Context.Registers[RegisterWord_BX]);
        AssertEqualU32(3, Context.Registers[RegisterWord_CX]);
        AssertEqualU32(4, Context.Registers[RegisterWord_DX]);
        AssertEqualU32(5, Context.Registers[RegisterWord_SP]);
        AssertEqualU32(6, Context.Registers[RegisterWord_BP]);
        AssertEqualU32(7, Context.Registers[RegisterWord_SI]);
        AssertEqualU32(8, Context.Registers[RegisterWord_DI]);
    }
}


inline void
RunRegisterMovTests(memory_arena *Arena)
{
    {
        u8 Stream[] = { 0b10111000, 42, 0b00000000, 0b10001001, 0b11000100 };
        AssertEqualString(String("mov ax, 42\nmov sp, ax"), StreamToAssembly(Arena, Stream, sizeof(Stream)));
        simulator_context Context = GetSimulatorContext(Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualU32(42, Context.Registers[RegisterWord_AX]);
        AssertEqualU32(42, Context.Registers[RegisterWord_SP]);
    }
    
    {
        u8 Stream[] = 
        { 
            0b10111000, 0b00000001, 0b00000000, 0b10111011, 0b00000010, 0b00000000, 0b10111001, 0b00000011, 0b00000000, 0b10111010, 0b00000100, 0b00000000, 0b10001001, 0b11000100, 0b10001001, 0b11011101, 0b10001001, 0b11001110, 0b10001001, 0b11010111, 0b10001001, 0b11100010, 0b10001001, 0b11101001, 0b10001001, 0b11110011, 0b10001001, 0b11111000 
        };
        AssertEqualString(String("mov ax, 1\nmov bx, 2\nmov cx, 3\nmov dx, 4\nmov sp, ax\nmov bp, bx\nmov si, cx\nmov di, dx\nmov dx, sp\nmov cx, bp\nmov bx, si\nmov ax, di"), 
                          StreamToAssembly(Arena, Stream, sizeof(Stream)));
        simulator_context Context = GetSimulatorContext(Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualU32(4, Context.Registers[RegisterWord_AX]);
        AssertEqualU32(3, Context.Registers[RegisterWord_BX]);
        AssertEqualU32(2, Context.Registers[RegisterWord_CX]);
        AssertEqualU32(1, Context.Registers[RegisterWord_DX]);
        AssertEqualU32(1, Context.Registers[RegisterWord_SP]);
        AssertEqualU32(2, Context.Registers[RegisterWord_BP]);
        AssertEqualU32(3, Context.Registers[RegisterWord_SI]);
        AssertEqualU32(4, Context.Registers[RegisterWord_DI]);
    }
    
    {
        u8 Stream[] = 
        { 
            0b10111000, 0b00100010, 0b00100010, 0b10111011, 0b01000100, 0b01000100, 0b10111001, 0b01100110, 0b01100110, 0b10111010, 0b10001000, 0b10001000, 0b10001110, 0b11010000, 0b10001110, 0b11011011, 0b10001110, 0b11000001, 0b10110000, 0b00010001, 0b10110111, 0b00110011, 0b10110001, 0b01010101, 0b10110110, 0b01110111, 0b10001000, 0b11011100, 0b10001000, 0b11110001, 0b10001110, 0b11010000, 0b10001110, 0b11011011, 0b10001110, 0b11000001, 0b10001100, 0b11010100, 0b10001100, 0b11011101, 0b10001100, 0b11000110, 0b10001001, 0b11010111
        };
        AssertEqualString(String("mov ax, 8738\nmov bx, 17476\nmov cx, 26214\nmov dx, -30584\nmov ss, ax\nmov ds, bx\nmov es, cx\nmov al, 17\nmov bh, 51\nmov cl, 85\nmov dh, 119\nmov ah, bl\nmov cl, dh\nmov ss, ax\nmov ds, bx\nmov es, cx\nmov sp, ss\nmov bp, ds\nmov si, es\nmov di, dx"), 
                          StreamToAssembly(Arena, Stream, sizeof(Stream)));
        simulator_context Context = GetSimulatorContext(Stream, sizeof(Stream));
        SimulateStep(&Context);
        AssertEqualHex(0x2222, Context.Registers[RegisterWord_AX]);
        SimulateStep(&Context);
        AssertEqualHex(0x4444, Context.Registers[RegisterWord_BX]);
        SimulateStep(&Context);
        AssertEqualHex(0x6666, Context.Registers[RegisterWord_CX]);
        SimulateStep(&Context);
        AssertEqualHex(0x8888, Context.Registers[RegisterWord_DX]);
        SimulateStep(&Context);
        AssertEqualHex(0x2222, Context.SegmentRegisters[SegmentRegister_SS]);
        SimulateStep(&Context);
        AssertEqualHex(0x4444, Context.SegmentRegisters[SegmentRegister_DS]);
        SimulateStep(&Context);
        AssertEqualHex(0x6666, Context.SegmentRegisters[SegmentRegister_ES]);
        SimulateStep(&Context);
        AssertEqualHex(0x2211, Context.Registers[RegisterWord_AX]);
        SimulateStep(&Context);
        AssertEqualHex(0x3344, Context.Registers[RegisterWord_BX]);
        SimulateStep(&Context);
        AssertEqualHex(0x6655, Context.Registers[RegisterWord_CX]);
        SimulateStep(&Context);
        AssertEqualHex(0x7788, Context.Registers[RegisterWord_DX]);
        SimulateStep(&Context);
        AssertEqualHex(0x4411, Context.Registers[RegisterWord_AX]);
        SimulateStep(&Context);
        AssertEqualHex(0x6677, Context.Registers[RegisterWord_CX]);
        SimulateStep(&Context);
        AssertEqualHex(0x4411, Context.SegmentRegisters[SegmentRegister_SS]);
        SimulateStep(&Context);
        AssertEqualHex(0x3344, Context.SegmentRegisters[SegmentRegister_DS]);
        SimulateStep(&Context);
        AssertEqualHex(0x6677, Context.SegmentRegisters[SegmentRegister_ES]);
        SimulateStep(&Context);
        AssertEqualHex(0x4411, Context.Registers[RegisterWord_SP]);
        SimulateStep(&Context);
        AssertEqualHex(0x3344, Context.Registers[RegisterWord_BP]);
        SimulateStep(&Context);
        AssertEqualHex(0x6677, Context.Registers[RegisterWord_SI]);
        SimulateStep(&Context);
        AssertEqualHex(0x7788, Context.Registers[RegisterWord_DI]);
    }
    
    {
        u8 Stream[] = 
        { 
            0b10111000, 0b00100010, 0b00100010, 0b10111011, 0b01000100, 0b01000100, 0b10111001, 0b01100110, 0b01100110, 0b10111010, 0b10001000, 0b10001000, 0b10001110, 0b11010000, 0b10001110, 0b11011011, 0b10001110, 0b11000001, 0b10110000, 0b00010001, 0b10110111, 0b00110011, 0b10110001, 0b01010101, 0b10110110, 0b01110111, 0b10001000, 0b11011100, 0b10001000, 0b11110001, 0b10001110, 0b11010000, 0b10001110, 0b11011011, 0b10001110, 0b11000001, 0b10001100, 0b11010100, 0b10001100, 0b11011101, 0b10001100, 0b11000110, 0b10001001, 0b11010111
        };
        AssertEqualString(String("mov ax, 8738\nmov bx, 17476\nmov cx, 26214\nmov dx, -30584\nmov ss, ax\nmov ds, bx\nmov es, cx\nmov al, 17\nmov bh, 51\nmov cl, 85\nmov dh, 119\nmov ah, bl\nmov cl, dh\nmov ss, ax\nmov ds, bx\nmov es, cx\nmov sp, ss\nmov bp, ds\nmov si, es\nmov di, dx"), 
                          StreamToAssembly(Arena, Stream, sizeof(Stream)));
        simulator_context Context = GetSimulatorContext(Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualU32(17425, Context.Registers[RegisterWord_AX]);
        AssertEqualU32(13124, Context.Registers[RegisterWord_BX]);
        AssertEqualU32(26231, Context.Registers[RegisterWord_CX]);
        AssertEqualU32(30600, Context.Registers[RegisterWord_DX]);
        AssertEqualU32(17425, Context.Registers[RegisterWord_SP]);
        AssertEqualU32(13124, Context.Registers[RegisterWord_BP]);
        AssertEqualU32(26231, Context.Registers[RegisterWord_SI]);
        AssertEqualU32(30600, Context.Registers[RegisterWord_DI]);
        AssertEqualU32(26231, Context.SegmentRegisters[SegmentRegister_ES]);
        AssertEqualU32(17425, Context.SegmentRegisters[SegmentRegister_SS]);
        AssertEqualU32(13124, Context.SegmentRegisters[SegmentRegister_DS]);
    }
}

void inline 
RunAddSubCmpTests(memory_arena *Arena)
{
    
    {
        u8 Stream[] = 
        { 
            0b10111011, 0b00000011, 0b11110000, 0b10111001, 0b00000001, 0b00001111, 0b00101001, 0b11001011, 0b10111100, 0b11100110, 0b00000011, 0b10111101, 0b11100111, 0b00000011, 0b00111001, 0b11100101, 0b10000001, 0b11000101, 0b00000011, 0b00000100, 0b10000001, 0b11101101, 0b11101010, 0b00000111
        };
        AssertEqualString(String("mov bx, -4093\nmov cx, 3841\nsub bx, cx\nmov sp, 998\nmov bp, 999\ncmp bp, sp\nadd bp, 1027\nsub bp, 2026"), 
                          StreamToAssembly(Arena, Stream, sizeof(Stream)));
        simulator_context Context = GetSimulatorContext(Stream, sizeof(Stream));
        SimulateStep(&Context);
        AssertEqualHex(0xf003, Context.Registers[RegisterWord_BX]);
        SimulateStep(&Context);
        AssertEqualHex(0xf01, Context.Registers[RegisterWord_CX]);
        AssertEqualBits(0, Context.Flags);
        SimulateStep(&Context);
        AssertEqualHex(0xe102, Context.Registers[RegisterWord_BX]);
        AssertEqualBits(Flag_SF, Context.Flags);
        SimulateStep(&Context);
        AssertEqualHex(0x3e6, Context.Registers[RegisterWord_SP]);
        SimulateStep(&Context);
        AssertEqualHex(0x3e7, Context.Registers[RegisterWord_BP]);
        SimulateStep(&Context);
        AssertEqualHex(0x3e7, Context.Registers[RegisterWord_BP]);
        AssertEqualHex(0x3e6, Context.Registers[RegisterWord_SP]);
        AssertEqualBits(0, Context.Flags);
        SimulateStep(&Context);
        AssertEqualHex(0x7ea, Context.Registers[RegisterWord_BP]);
        AssertEqualBits(Flag_PF, Context.Flags); // NOTE(kstandbridge): This might be incorrect
        SimulateStep(&Context);
        AssertEqualHex(0, Context.Registers[RegisterWord_BP]);
        AssertEqualBits((Flag_ZF | Flag_PF), Context.Flags);
    }
    
    {
        u8 Stream[] = 
        { 
            0b10111011, 0b00000011, 0b11110000, 0b10111001, 0b00000001, 0b00001111, 0b00101001, 0b11001011, 0b10111100, 0b11100110, 0b00000011, 0b10111101, 0b11100111, 0b00000011, 0b00111001, 0b11100101, 0b10000001, 0b11000101, 0b00000011, 0b00000100, 0b10000001, 0b11101101, 0b11101010, 0b00000111
        };
        AssertEqualString(String("mov bx, -4093\nmov cx, 3841\nsub bx, cx\nmov sp, 998\nmov bp, 999\ncmp bp, sp\nadd bp, 1027\nsub bp, 2026"), 
                          StreamToAssembly(Arena, Stream, sizeof(Stream)));
        simulator_context Context = GetSimulatorContext(Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualU32(57602, Context.Registers[RegisterWord_BX]);
        AssertEqualU32(3841, Context.Registers[RegisterWord_CX]);
        AssertEqualU32(998, Context.Registers[RegisterWord_SP]);
    }
    
}

void
RunAllTests(memory_arena *Arena)
{
    RunInstructionTableTests();
    RunDisassembleTests();
    RunDisassembleToAssemblyTests(Arena);
    RunImmediateMovTests(Arena);
    RunRegisterMovTests(Arena);
    RunAddSubCmpTests(Arena);
    
#if 0
    PlatformConsoleOut("\n");
    string FileData = PlatformReadEntireFile(Arena, String("test"));
    b32 First = true;
    for(umm StreamIndex = 0;
        StreamIndex < FileData.Size;
        ++StreamIndex)
    {
        if(First)
        {
            First = false;
        }
        else
        {
            PlatformConsoleOut(", ");
        }
        u8 Byte = FileData.Data[StreamIndex];
        PlatformConsoleOut("%b", Byte);
    }
    PlatformConsoleOut("\n\n");
    string Assembly = StreamToAssembly(Arena, FileData.Data, FileData.Size);
    PlatformConsoleOut("%S\n\n", Assembly);
#endif
    
}
