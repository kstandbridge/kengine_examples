#define KENGINE_TEST
#define KENGINE_IMPLEMENTATION
#include "kengine.h"


#include "sim8086.h"
#include "sim8086.c"

internal void
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

internal void
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

internal void
RunDisassembleToAssemblyTests(memory_arena *Arena)
{
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001001, 0b11011110 };
        AssertEqualString(String("mov si, bx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001000, 0b11000110 };
        AssertEqualString(String("mov dh, al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10110001, 0b00001100 };
        AssertEqualString(String("mov cl, 12"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10110101, 0b11110100 };
        AssertEqualString(String("mov ch, -12"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10111001, 0b00001100, 0b00000000 };
        AssertEqualString(String("mov cx, 12"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10111001, 0b11110100, 0b11111111 };
        AssertEqualString(String("mov cx, -12"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10111010, 0b01101100, 0b00001111 };
        AssertEqualString(String("mov dx, 3948"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10111010, 0b10010100, 0b11110000 };
        AssertEqualString(String("mov dx, -3948"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001010, 0b00000000 };
        AssertEqualString(String("mov al, byte [bx + si]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001011, 0b00011011 };
        AssertEqualString(String("mov bx, word [bp + di]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001011, 0b01010110, 0b00000000 };
        AssertEqualString(String("mov dx, [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001010, 0b01100000, 0b00000100 };
        AssertEqualString(String("mov ah, [bx + si + 4]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001010, 0b10000000, 0b10000111, 0b00010011 };
        AssertEqualString(String("mov al, [bx + si + 4999]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001001, 0b00001001 };
        AssertEqualString(String("mov word [bx + di], cx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001000, 0b00001010 };
        AssertEqualString(String("mov byte [bp + si], cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001000, 0b01101110, 0b00000000 };
        AssertEqualString(String("mov [bp], ch"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001011, 0b01000001, 0b11011011 };
        AssertEqualString(String("mov ax, [bx + di - 37]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001001, 0b10001100, 0b11010100, 0b11111110 };
        AssertEqualString(String("mov [si - 300], cx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001011, 0b01010111, 0b11100000 };
        AssertEqualString(String("mov dx, [bx - 32]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000110, 0b00000011, 0b00000111 };
        AssertEqualString(String("mov [bp + di], byte 7"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000111, 0b10000101, 0b10000101, 0b00000011, 0b01011011, 0b00000001 };
        AssertEqualString(String("mov word [di + 901], 347"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000111, 0b01000111, 0b00000100, 0b00001010, 0b00000000 };
        AssertEqualString(String("mov word [bx + 4], 10"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000111, 0b00000110, 0b11101000, 0b00000011, 0b00000001, 0b00000000 };
        AssertEqualString(String("mov word [1000], 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001001, 0b00110010 };
        AssertEqualString(String("mov word [bp + si], si"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001011, 0b00101110, 0b00000101, 0b00000000 };
        AssertEqualString(String("mov bp, word [5]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001011, 0b00011110, 0b10000010, 0b00001101 };
        AssertEqualString(String("mov bx, word [3458]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001011, 0b00011110, 0b11101000, 0b00000011 };
        AssertEqualString(String("mov bx, word [1000]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10100001, 0b11111011, 0b00001001 };
        AssertEqualString(String("mov ax, [2555]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10100001, 0b00010000, 0b00000000 };
        AssertEqualString(String("mov ax, [16]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10100011, 0b11111010, 0b00001001 };
        AssertEqualString(String("mov [2554], ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10100011, 0b00001111, 0b00000000 };
        AssertEqualString(String("mov [15], ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b00110010 };
        AssertEqualString(String("push word [bp + si]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b00110110, 0b10111000, 0b00001011 };
        AssertEqualString(String("push word [3000]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b01110001, 0b11100010 };
        AssertEqualString(String("push word [bx + di - 30]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01010001 };
        AssertEqualString(String("push cx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01010000 };
        AssertEqualString(String("push ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01010010 };
        AssertEqualString(String("push dx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00001110 };
        AssertEqualString(String("push cs"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001111, 0b00000010 };
        AssertEqualString(String("pop word [bp + si]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001111, 0b00000110, 0b00000011, 0b00000000 };
        AssertEqualString(String("pop word [3]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001111, 0b10000001, 0b01001000, 0b11110100 };
        AssertEqualString(String("pop word [bx + di - 3000]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01011100 };
        AssertEqualString(String("pop sp"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01011111 };
        AssertEqualString(String("pop di"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01011110 };
        AssertEqualString(String("pop si"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00011111 };
        AssertEqualString(String("pop ds"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000111, 0b10000110, 0b00011000, 0b11111100 };
        AssertEqualString(String("xchg ax, [bp - 1000]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000111, 0b01101111, 0b00110010 };
        AssertEqualString(String("xchg [bx + 50], bp"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10010000 };
        AssertEqualString(String("xchg ax, ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10010010 };
        AssertEqualString(String("xchg ax, dx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10010100 };
        AssertEqualString(String("xchg ax, sp"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10010110 };
        AssertEqualString(String("xchg ax, si"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10010111 };
        AssertEqualString(String("xchg ax, di"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000111, 0b11001010 };
        AssertEqualString(String("xchg cx, dx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000111, 0b11110001 };
        AssertEqualString(String("xchg si, cx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000110, 0b11001100 };
        AssertEqualString(String("xchg cl, ah"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11100100, 0b11001000 };
        AssertEqualString(String("in al, 200"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11101100 };
        AssertEqualString(String("in al, dx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11101101 };
        AssertEqualString(String("in ax, dx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11100111, 0b00101100 };
        AssertEqualString(String("out 44, ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11101110 };
        AssertEqualString(String("out dx, al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010111 };
        AssertEqualString(String("xlat"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001101, 0b10000001, 0b10001100, 0b00000101 };
        AssertEqualString(String("lea ax, [bx + di + 1420]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001101, 0b01011110, 0b11001110 };
        AssertEqualString(String("lea bx, [bp - 50]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001101, 0b10100110, 0b00010101, 0b11111100 };
        AssertEqualString(String("lea sp, [bp - 1003]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001101, 0b01111000, 0b11111001 };
        AssertEqualString(String("lea di, [bx + si - 7]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000101, 0b10000001, 0b10001100, 0b00000101 };
        AssertEqualString(String("lds ax, [bx + di + 1420]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000101, 0b01011110, 0b11001110 };
        AssertEqualString(String("lds bx, [bp - 50]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000101, 0b10100110, 0b00010101, 0b11111100 };
        AssertEqualString(String("lds sp, [bp - 1003]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000101, 0b01111000, 0b11111001 };
        AssertEqualString(String("lds di, [bx + si - 7]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000100, 0b10000001, 0b10001100, 0b00000101 };
        AssertEqualString(String("les ax, [bx + di + 1420]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000100, 0b01011110, 0b11001110 };
        AssertEqualString(String("les bx, [bp - 50]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000100, 0b10100110, 0b00010101, 0b11111100 };
        AssertEqualString(String("les sp, [bp - 1003]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000100, 0b01111000, 0b11111001 };
        AssertEqualString(String("les di, [bx + si - 7]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10011111 };
        AssertEqualString(String("lahf"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10011110 };
        AssertEqualString(String("sahf"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10011100 };
        AssertEqualString(String("pushf"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10011101 };
        AssertEqualString(String("popf"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00000011, 0b01001110, 0b00000000 };
        AssertEqualString(String("add cx, [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00000011, 0b00010000 };
        AssertEqualString(String("add dx, word [bx + si]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00000000, 0b10100011, 0b10001000, 0b00010011 };
        AssertEqualString(String("add [bp + di + 5000], ah"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00000000, 0b00000111 };
        AssertEqualString(String("add byte [bx], al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000001, 0b11000100, 0b10001000, 0b00000001 };
        AssertEqualString(String("add sp, 392"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000011, 0b11000110, 0b00000101 };
        AssertEqualString(String("add si, 5"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00000101, 0b11101000, 0b00000011 };
        AssertEqualString(String("add ax, 1000"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000000, 0b11000100, 0b00011110 };
        AssertEqualString(String("add ah, 30"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00000100, 0b00001001 };
        AssertEqualString(String("add al, 9"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00000001, 0b11011001 };
        AssertEqualString(String("add cx, bx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00000000, 0b11000101 };
        AssertEqualString(String("add ch, al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00010011, 0b01001110, 0b00000000 };
        AssertEqualString(String("adc cx, [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00010011, 0b00010000 };
        AssertEqualString(String("adc dx, [bx + si]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00010000, 0b10100011, 0b10001000, 0b00010011 };
        AssertEqualString(String("adc [bp + di + 5000], ah"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00010000, 0b00000111 };
        AssertEqualString(String("adc [bx], al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000001, 0b11010100, 0b10001000, 0b00000001 };
        AssertEqualString(String("adc sp, 392"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000011, 0b11010110, 0b00000101 };
        AssertEqualString(String("adc si, 5"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00010101, 0b11101000, 0b00000011 };
        AssertEqualString(String("adc ax, 1000"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000000, 0b11010100, 0b00011110 };
        AssertEqualString(String("adc ah, 30"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00010100, 0b00001001 };
        AssertEqualString(String("adc al, 9"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00010001, 0b11011001 };
        AssertEqualString(String("adc cx, bx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00010000, 0b11000101 };
        AssertEqualString(String("adc ch, al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01000000 };
        AssertEqualString(String("inc ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01000001 };
        AssertEqualString(String("inc cx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111110, 0b11000110 };
        AssertEqualString(String("inc dh"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111110, 0b11000000 };
        AssertEqualString(String("inc al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111110, 0b11000100 };
        AssertEqualString(String("inc ah"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01000100 };
        AssertEqualString(String("inc sp"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01000111 };
        AssertEqualString(String("inc di"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111110, 0b10000110, 0b11101010, 0b00000011 };
        AssertEqualString(String("inc byte [bp + 1002]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b01000111, 0b00100111 };
        AssertEqualString(String("inc word [bx + 39]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111110, 0b01000000, 0b00000101 };
        AssertEqualString(String("inc byte [bx + si + 5]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b10000011, 0b11000100, 0b11011000 };
        AssertEqualString(String("inc word [bp + di - 10044]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b00000110, 0b10000101, 0b00100100 };
        AssertEqualString(String("inc word [9349]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111110, 0b01000110, 0b00000000 };
        AssertEqualString(String("inc byte [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110111 };
        AssertEqualString(String("aaa"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100111 };
        AssertEqualString(String("daa"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00101011, 0b01001110, 0b00000000 };
        AssertEqualString(String("sub cx, [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00101011, 0b00010000 };
        AssertEqualString(String("sub dx, [bx + si]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00101000, 0b10100011, 0b10001000, 0b00010011 };
        AssertEqualString(String("sub [bp + di + 5000], ah"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00101000, 0b00000111 };
        AssertEqualString(String("sub [bx], al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000001, 0b11101100, 0b10001000, 0b00000001 };
        AssertEqualString(String("sub sp, 392"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000011, 0b11101110, 0b00000101 };
        AssertEqualString(String("sub si, 5"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00101101, 0b11101000, 0b00000011 };
        AssertEqualString(String("sub ax, 1000"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000000, 0b11101100, 0b00011110 };
        AssertEqualString(String("sub ah, 30"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00101100, 0b00001001 };
        AssertEqualString(String("sub al, 9"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00101001, 0b11011001 };
        AssertEqualString(String("sub cx, bx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00101000, 0b11000101 };
        AssertEqualString(String("sub ch, al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00011011, 0b01001110, 0b00000000 };
        AssertEqualString(String("sbb cx, [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00011011, 0b00010000 };
        AssertEqualString(String("sbb dx, [bx + si]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00011000, 0b10100011, 0b10001000, 0b00010011 };
        AssertEqualString(String("sbb [bp + di + 5000], ah"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00011000, 0b00000111 };
        AssertEqualString(String("sbb [bx], al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000001, 0b11011100, 0b10001000, 0b00000001 };
        AssertEqualString(String("sbb sp, 392"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000011, 0b11011110, 0b00000101 };
        AssertEqualString(String("sbb si, 5"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00011101, 0b11101000, 0b00000011 };
        AssertEqualString(String("sbb ax, 1000"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000000, 0b11011100, 0b00011110 };
        AssertEqualString(String("sbb ah, 30"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00011100, 0b00001001 };
        AssertEqualString(String("sbb al, 9"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00011001, 0b11011001 };
        AssertEqualString(String("sbb cx, bx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00011000, 0b11000101 };
        AssertEqualString(String("sbb ch, al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01001000 };
        AssertEqualString(String("dec ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01001001 };
        AssertEqualString(String("dec cx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111110, 0b11001110 };
        AssertEqualString(String("dec dh"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111110, 0b11001000 };
        AssertEqualString(String("dec al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111110, 0b11001100 };
        AssertEqualString(String("dec ah"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01001100 };
        AssertEqualString(String("dec sp"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01001111 };
        AssertEqualString(String("dec di"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111110, 0b10001110, 0b11101010, 0b00000011 };
        AssertEqualString(String("dec byte [bp + 1002]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b01001111, 0b00100111 };
        AssertEqualString(String("dec word [bx + 39]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111110, 0b01001000, 0b00000101 };
        AssertEqualString(String("dec byte [bx + si + 5]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b10001011, 0b11000100, 0b11011000 };
        AssertEqualString(String("dec word [bp + di - 10044]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b00001110, 0b10000101, 0b00100100 };
        AssertEqualString(String("dec word [9349]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111110, 0b01001110, 0b00000000 };
        AssertEqualString(String("dec byte [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b11011000 };
        AssertEqualString(String("neg ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b11011001 };
        AssertEqualString(String("neg cx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b11011110 };
        AssertEqualString(String("neg dh"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b11011000 };
        AssertEqualString(String("neg al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b11011100 };
        AssertEqualString(String("neg ah"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b11011100 };
        AssertEqualString(String("neg sp"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b11011111 };
        AssertEqualString(String("neg di"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b10011110, 0b11101010, 0b00000011 };
        AssertEqualString(String("neg byte [bp + 1002]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b01011111, 0b00100111 };
        AssertEqualString(String("neg word [bx + 39]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b01011000, 0b00000101 };
        AssertEqualString(String("neg byte [bx + si + 5]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b10011011, 0b11000100, 0b11011000 };
        AssertEqualString(String("neg word [bp + di - 10044]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b00011110, 0b10000101, 0b00100100 };
        AssertEqualString(String("neg word [9349]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b01011110, 0b00000000 };
        AssertEqualString(String("neg byte [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00111001, 0b11001011 };
        AssertEqualString(String("cmp bx, cx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00111010, 0b10110110, 0b10000110, 0b00000001 };
        AssertEqualString(String("cmp dh, [bp + 390]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00111001, 0b01110110, 0b00000010 };
        AssertEqualString(String("cmp [bp + 2], si"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000000, 0b11111011, 0b00010100 };
        AssertEqualString(String("cmp bl, 20"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000000, 0b00111111, 0b00100010 };
        AssertEqualString(String("cmp byte [bx], 34"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00111101, 0b01100101, 0b01011101 };
        AssertEqualString(String("cmp ax, 23909"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00111111 };
        AssertEqualString(String("aas"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00101111 };
        AssertEqualString(String("das"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b11100000 };
        AssertEqualString(String("mul al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b11100001 };
        AssertEqualString(String("mul cx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b01100110, 0b00000000 };
        AssertEqualString(String("mul word [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b10100001, 0b11110100, 0b00000001 };
        AssertEqualString(String("mul byte [bx + di + 500]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);u8 Stream[] = { 0b11110110, 0b11101101 };
        AssertEqualString(String("imul ch"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b11101010 };
        AssertEqualString(String("imul dx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b00101111 };
        AssertEqualString(String("imul byte [bx]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b00101110, 0b00001011, 0b00100101 };
        AssertEqualString(String("imul word [9483]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010100, 0b11111111 };
        AssertEqualString(String("aam 255"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010100, 0b00001010 };
        AssertEqualString(String("aam"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010100, 0b00001011 };
        AssertEqualString(String("aam 11"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b11110011 };
        AssertEqualString(String("div bl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b11110100 };
        AssertEqualString(String("div sp"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b10110000, 0b10101110, 0b00001011 };
        AssertEqualString(String("div byte [bx + si + 2990]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b10110011, 0b11101000, 0b00000011 };
        AssertEqualString(String("div word [bp + di + 1000]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b11111000 };
        AssertEqualString(String("idiv ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b11111110 };
        AssertEqualString(String("idiv si"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b00111010 };
        AssertEqualString(String("idiv byte [bp + si]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b10111111, 0b11101101, 0b00000001 };
        AssertEqualString(String("idiv word [bx + 493]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010101, 0b00001010 };
        AssertEqualString(String("aad"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010101, 0b00001011 };
        AssertEqualString(String("aad 11"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010101, 0b00000001 };
        AssertEqualString(String("aad 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010101, 0b00101010 };
        AssertEqualString(String("aad 42"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10011000 };
        AssertEqualString(String("cbw"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10011001 };
        AssertEqualString(String("cwd"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b11010100 };
        AssertEqualString(String("not ah"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b11010011 };
        AssertEqualString(String("not bl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b11010100 };
        AssertEqualString(String("not sp"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b11010110 };
        AssertEqualString(String("not si"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110111, 0b01010110, 0b00000000 };
        AssertEqualString(String("not word [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b10010110, 0b10110001, 0b00100110 };
        AssertEqualString(String("not byte [bp + 9905]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010001, 0b11100101 };
        AssertEqualString(String("shl bp, 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010000, 0b11100100 };
        AssertEqualString(String("shl ah, 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010000, 0b11100110 };
        AssertEqualString(String("shl dh, 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010001, 0b01100110, 0b00000101 };
        AssertEqualString(String("shl word [bp + 5], 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010001, 0b10100110, 0b10011001, 0b00011001 };
        AssertEqualString(String("shl word [bp + 6553], 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010001, 0b11101000 };
        AssertEqualString(String("shr ax, 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010001, 0b11111011 };
        AssertEqualString(String("sar bx, 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010001, 0b11000001 };
        AssertEqualString(String("rol cx, 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010000, 0b11001110 };
        AssertEqualString(String("ror dh, 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010001, 0b11010100 };
        AssertEqualString(String("rcl sp, 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010001, 0b11011101 };
        AssertEqualString(String("rcr bp, 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010001, 0b01100110, 0b00000101 };
        AssertEqualString(String("shl word [bp + 5], 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010000, 0b10101000, 0b00111001, 0b11111111 };
        AssertEqualString(String("shr byte [bx + si - 199], 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010000, 0b10111001, 0b11010100, 0b11111110 };
        AssertEqualString(String("sar byte [bx + di - 300], 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010001, 0b01000110, 0b00000000 };
        AssertEqualString(String("rol word [bp], 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010001, 0b00001110, 0b01001010, 0b00010011 };
        AssertEqualString(String("ror word [4938], 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010000, 0b00010110, 0b00000011, 0b00000000 };
        AssertEqualString(String("rcl byte [3], 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010001, 0b00011111 };
        AssertEqualString(String("rcr word [bx], 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010010, 0b11100100 };
        AssertEqualString(String("shl ah, cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010011, 0b11101000 };
        AssertEqualString(String("shr ax, cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010011, 0b11111011 };
        AssertEqualString(String("sar bx, cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010011, 0b11000001 };
        AssertEqualString(String("rol cx, cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010010, 0b11001110 };
        AssertEqualString(String("ror dh, cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010011, 0b11010100 };
        AssertEqualString(String("rcl sp, cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010011, 0b11011101 };
        AssertEqualString(String("rcr bp, cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010011, 0b01100110, 0b00000101 };
        AssertEqualString(String("shl word [bp + 5], cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010011, 0b10101000, 0b00111001, 0b11111111 };
        AssertEqualString(String("shr word [bx + si - 199], cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010010, 0b10111001, 0b11010100, 0b11111110 };
        AssertEqualString(String("sar byte [bx + di - 300], cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010010, 0b01000110, 0b00000000 };
        AssertEqualString(String("rol byte [bp], cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010010, 0b00001110, 0b01001010, 0b00010011 };
        AssertEqualString(String("ror byte [4938], cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010010, 0b00010110, 0b00000011, 0b00000000 };
        AssertEqualString(String("rcl byte [3], cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11010011, 0b00011111 };
        AssertEqualString(String("rcr word [bx], cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100000, 0b11100000 };
        AssertEqualString(String("and al, ah"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100000, 0b11001101 };
        AssertEqualString(String("and ch, cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100001, 0b11110101 };
        AssertEqualString(String("and bp, si"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100001, 0b11100111 };
        AssertEqualString(String("and di, sp"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100100, 0b01011101 };
        AssertEqualString(String("and al, 93"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100101, 0b10101000, 0b01001111 };
        AssertEqualString(String("and ax, 20392"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100000, 0b01101010, 0b00001010 };
        AssertEqualString(String("and [bp + si + 10], ch"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100001, 0b10010001, 0b11101000, 0b00000011 };
        AssertEqualString(String("and [bx + di + 1000], dx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100011, 0b01011110, 0b00000000 };
        AssertEqualString(String("and bx, [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100011, 0b00001110, 0b00100000, 0b00010001 };
        AssertEqualString(String("and cx, [4384]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000000, 0b01100110, 0b11011001, 0b11101111 };
        AssertEqualString(String("and byte [bp - 39], 239"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000001, 0b10100000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        AssertEqualString(String("and word [bx + si - 4332], 10328"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000101, 0b11001011 };
        AssertEqualString(String("test bx, cx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000100, 0b10110110, 0b10000110, 0b00000001 };
        AssertEqualString(String("test dh, [bp + 390]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000101, 0b01110110, 0b00000010 };
        AssertEqualString(String("test [bp + 2], si"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b11000011, 0b00010100 };
        AssertEqualString(String("test bl, 20"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110110, 0b00000111, 0b00100010 };
        AssertEqualString(String("test byte [bx], 34"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10101001, 0b01100101, 0b01011101 };
        AssertEqualString(String("test ax, 23909"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00001000, 0b11100000 };
        AssertEqualString(String("or al, ah"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00001000, 0b11001101 };
        AssertEqualString(String("or ch, cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00001001, 0b11110101 };
        AssertEqualString(String("or bp, si"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00001001, 0b11100111 };
        AssertEqualString(String("or di, sp"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00001100, 0b01011101 };
        AssertEqualString(String("or al, 93"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00001101, 0b10101000, 0b01001111 };
        AssertEqualString(String("or ax, 20392"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00001000, 0b01101010, 0b00001010 };
        AssertEqualString(String("or [bp + si + 10], ch"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00001001, 0b10010001, 0b11101000, 0b00000011 };
        AssertEqualString(String("or [bx + di + 1000], dx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00001011, 0b01011110, 0b00000000 };
        AssertEqualString(String("or bx, [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00001011, 0b00001110, 0b00100000, 0b00010001 };
        AssertEqualString(String("or cx, [4384]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000000, 0b01001110, 0b11011001, 0b11101111 };
        AssertEqualString(String("or byte [bp - 39], 239"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000001, 0b10001000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        AssertEqualString(String("or word [bx + si - 4332], 10328"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110000, 0b11100000 };
        AssertEqualString(String("xor al, ah"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110000, 0b11001101 };
        AssertEqualString(String("xor ch, cl"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110001, 0b11110101 };
        AssertEqualString(String("xor bp, si"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110001, 0b11100111 };
        AssertEqualString(String("xor di, sp"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110100, 0b01011101 };
        AssertEqualString(String("xor al, 93"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110101, 0b10101000, 0b01001111 };
        AssertEqualString(String("xor ax, 20392"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110000, 0b01101010, 0b00001010 };
        AssertEqualString(String("xor [bp + si + 10], ch"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110001, 0b10010001, 0b11101000, 0b00000011 };
        AssertEqualString(String("xor [bx + di + 1000], dx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110011, 0b01011110, 0b00000000 };
        AssertEqualString(String("xor bx, [bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110011, 0b00001110, 0b00100000, 0b00010001 };
        AssertEqualString(String("xor cx, [4384]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000000, 0b01110110, 0b11011001, 0b11101111 };
        AssertEqualString(String("xor byte [bp - 39], 239"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000001, 0b10110000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        AssertEqualString(String("xor word [bx + si - 4332], 10328"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110011, 0b10100100 };
        AssertEqualString(String("rep movsb"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110011, 0b10100110 };
        AssertEqualString(String("rep cmpsb"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110011, 0b10101110 };
        AssertEqualString(String("rep scasb"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110011, 0b10101100 };
        AssertEqualString(String("rep lodsb"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110011, 0b10100101 };
        AssertEqualString(String("rep movsw"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110011, 0b10100111 };
        AssertEqualString(String("rep cmpsw"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110011, 0b10101111 };
        AssertEqualString(String("rep scasw"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110011, 0b10101101 };
        AssertEqualString(String("rep lodsw"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110011, 0b10101010 };
        AssertEqualString(String("rep stosb"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110011, 0b10101011 };
        AssertEqualString(String("rep stosw"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b00010110, 0b00100001, 0b10011001 };
        AssertEqualString(String("call [39201]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b01010110, 0b10011100 };
        AssertEqualString(String("call [bp - 100]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b11010100 };
        AssertEqualString(String("call sp"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b11010000 };
        AssertEqualString(String("call ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b11100000 };
        AssertEqualString(String("jmp ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b11100111 };
        AssertEqualString(String("jmp di"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b00100110, 0b00001100, 0b00000000 };
        AssertEqualString(String("jmp [12]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b00100110, 0b00101011, 0b00010001 };
        AssertEqualString(String("jmp [4395]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000010, 0b11111001, 0b11111111 };
        AssertEqualString(String("ret -7"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000010, 0b11110100, 0b00000001 };
        AssertEqualString(String("ret 500"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000011 };
        AssertEqualString(String("ret"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01110100, 0b11111110 };
        AssertEqualString(String("je $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01111100, 0b11111110 };
        AssertEqualString(String("jl $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01111110, 0b11111110 };
        AssertEqualString(String("jle $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01110010, 0b11111110 };
        AssertEqualString(String("jb $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01110110, 0b11111110 };
        AssertEqualString(String("jbe $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01111010, 0b11111110 };
        AssertEqualString(String("jp $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01110000, 0b11111110 };
        AssertEqualString(String("jo $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01111000, 0b11111110 };
        AssertEqualString(String("js $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01110101, 0b11111110 };
        AssertEqualString(String("jne $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01111101, 0b11111110 };
        AssertEqualString(String("jnl $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01111111, 0b11111110 };
        AssertEqualString(String("jg $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01110011, 0b11111110 };
        AssertEqualString(String("jnb $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01110111, 0b11111110 };
        AssertEqualString(String("ja $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01111011, 0b11111110 };
        AssertEqualString(String("jnp $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01110001, 0b11111110 };
        AssertEqualString(String("jno $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01111001, 0b11111110 };
        AssertEqualString(String("jns $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11100010, 0b11111110 };
        AssertEqualString(String("loop $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11100001, 0b11111110 };
        AssertEqualString(String("loopz $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11100000, 0b11111110 };
        AssertEqualString(String("loopnz $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11100011, 0b11111110 };
        AssertEqualString(String("jcxz $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11001101, 0b00001101 };
        AssertEqualString(String("int 13"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11001100 };
        AssertEqualString(String("int3"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11001110 };
        AssertEqualString(String("into"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11001111 };
        AssertEqualString(String("iret"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111000 };
        AssertEqualString(String("clc"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110101 };
        AssertEqualString(String("cmc"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111001 };
        AssertEqualString(String("stc"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111100 };
        AssertEqualString(String("cld"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111101 };
        AssertEqualString(String("std"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111010 };
        AssertEqualString(String("cli"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111011 };
        AssertEqualString(String("sti"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110100 };
        AssertEqualString(String("hlt"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10011011 };
        AssertEqualString(String("wait"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110000, 0b11110110, 0b10010110, 0b10110001, 0b00100110 };
        AssertEqualString(String("lock not byte [bp + 9905]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    {
        
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000110, 0b00000110, 0b01100100, 0b00000000 };
        AssertEqualString(String("xchg [100], al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110000, 0b10000110, 0b00000110, 0b01100100, 0b00000000 };
        AssertEqualString(String("lock xchg [100], al"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00101110, 0b10001010, 0b00000000 };
        AssertEqualString(String("mov al, byte cs:[bx + si]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00111110, 0b10001011, 0b00011011 };
        AssertEqualString(String("mov bx, word ds:[bp + di]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100110, 0b10001011, 0b01010110, 0b00000000 };
        AssertEqualString(String("mov dx, es:[bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110110, 0b10001010, 0b01100000, 0b00000100 };
        AssertEqualString(String("mov ah, ss:[bx + si + 4]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00110110, 0b00100000, 0b01101010, 0b00001010 };
        AssertEqualString(String("and ss:[bp + si + 10], ch"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00111110, 0b00001001, 0b10010001, 0b11101000, 0b00000011 };
        AssertEqualString(String("or ds:[bx + di + 1000], dx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100110, 0b00110011, 0b01011110, 0b00000000 };
        AssertEqualString(String("xor bx, es:[bp]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00100110, 0b00111011, 0b00001110, 0b00100000, 0b00010001 };
        AssertEqualString(String("cmp cx, es:[4384]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00101110, 0b11110110, 0b01000110, 0b11011001, 0b11101111 };
        AssertEqualString(String("test byte cs:[bp - 39], 239"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000001, 0b10011000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        AssertEqualString(String("sbb word [bx + si - 4332], 10328"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00101110, 0b10000001, 0b10011000, 0b00010100, 0b11101111, 0b01011000, 0b00101000 };
        AssertEqualString(String("sbb word cs:[bx + si - 4332], 10328"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11110000, 0b00101110, 0b11110110, 0b10010110, 0b10110001, 0b00100110 };
        AssertEqualString(String("lock not byte cs:[bp + 9905]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10011010, 0b11001000, 0b00000001, 0b01111011, 0b00000000 };
        AssertEqualString(String("call 123:456"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11101010, 0b00100010, 0b00000000, 0b00010101, 0b00000011 };
        AssertEqualString(String("jmp 789:34"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001100, 0b01000000, 0b00111011 };
        AssertEqualString(String("mov [bx + si + 59], es"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11101001, 0b00111001, 0b00001010 };
        AssertEqualString(String("jmp 2620"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11101000, 0b00011001, 0b00101110 };
        AssertEqualString(String("call 11804"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11001010, 0b10010100, 0b01000100 };
        AssertEqualString(String("retf 17556"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000010, 0b10011000, 0b01000100 };
        AssertEqualString(String("ret 17560"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11001011 };
        AssertEqualString(String("retf"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11000011 };
        AssertEqualString(String("ret"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b01010010, 0b11000110 };
        AssertEqualString(String("call [bp + si - 58]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b01011010, 0b11000110 };
        AssertEqualString(String("call far [bp + si - 58]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b00100101 };
        AssertEqualString(String("jmp [di]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11111111, 0b00101101 };
        AssertEqualString(String("jmp far [di]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b11101010, 0b10001000, 0b01110111, 0b01100110, 0b01010101 };
        AssertEqualString(String("jmp 21862:30600"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001001, 0b11011101, 0b10001001, 0b11001110 };
        AssertEqualString(String("mov bp, bx\nmov si, cx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001110, 0b11010000 };
        AssertEqualString(String("mov ss, ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10110000, 0b00010001 };
        AssertEqualString(String("mov al, 17"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10001100, 0b11011101 };
        AssertEqualString(String("mov bp, ds"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000001, 0b11000011, 0b01000000, 0b10011100 };
        AssertEqualString(String("add bx, 40000"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b00000011, 0b00011010 };
        AssertEqualString(String("add bx, word [bp + si]"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000011, 0b11000001, 0b10100110 };
        AssertEqualString(String("add cx, -90"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10000011, 0b11000001, 0b10100110, 0b10111100, 0b01100011, 0b00000000 };
        AssertEqualString(String("add cx, -90\nmov sp, 99"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10111001, 0b11001000, 0b00000000 };
        AssertEqualString(String("mov cx, -56"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10111001, 0b11001000, 0b00000000, 0b10001001, 0b11001011 };
        AssertEqualString(String("mov cx, -56\nmov bx, cx"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b01110101, 0b11111110 };
        AssertEqualString(String("jne $0"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush);
    }
    
}


internal void
RunImmediateMovTests(memory_arena *Arena)
{
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10111000, 0b00000001, 0b00000000 };
        AssertEqualString(String("mov ax, 1"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        AssertEqualU32(0, Context.Registers[RegisterWord_AX]);
        Simulate(&Context);
        AssertEqualU32(1, Context.Registers[RegisterWord_AX]);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10111011, 0b00000010, 0b00000000 };
        AssertEqualString(String("mov bx, 2"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        AssertEqualU32(0, Context.Registers[RegisterWord_BX]);
        Simulate(&Context);
        AssertEqualU32(2, Context.Registers[RegisterWord_BX]);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111000, 0b00000001, 0b00000000, 0b10111011, 0b00000010, 0b00000000, 0b10111001, 0b00000011, 0b00000000, 0b10111010, 0b00000100, 0b00000000, 0b10111100, 0b00000101, 0b00000000, 0b10111101, 0b00000110, 0b00000000, 0b10111110, 0b00000111, 0b00000000, 0b10111111, 0b00001000, 0b00000000 
        };
        AssertEqualString(String("mov ax, 1\nmov bx, 2\nmov cx, 3\nmov dx, 4\nmov sp, 5\nmov bp, 6\nmov si, 7\nmov di, 8"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualU32(1, Context.Registers[RegisterWord_AX]);
        AssertEqualU32(2, Context.Registers[RegisterWord_BX]);
        AssertEqualU32(3, Context.Registers[RegisterWord_CX]);
        AssertEqualU32(4, Context.Registers[RegisterWord_DX]);
        AssertEqualU32(5, Context.Registers[RegisterWord_SP]);
        AssertEqualU32(6, Context.Registers[RegisterWord_BP]);
        AssertEqualU32(7, Context.Registers[RegisterWord_SI]);
        AssertEqualU32(8, Context.Registers[RegisterWord_DI]);
        EndTemporaryMemory(MemoryFlush);
    }
}


internal void
RunRegisterMovTests(memory_arena *Arena)
{
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = { 0b10111000, 42, 0b00000000, 0b10001001, 0b11000100 };
        AssertEqualString(String("mov ax, 42\nmov sp, ax"), StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualU32(42, Context.Registers[RegisterWord_AX]);
        AssertEqualU32(42, Context.Registers[RegisterWord_SP]);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111000, 0b00000001, 0b00000000, 0b10111011, 0b00000010, 0b00000000, 0b10111001, 0b00000011, 0b00000000, 0b10111010, 0b00000100, 0b00000000, 0b10001001, 0b11000100, 0b10001001, 0b11011101, 0b10001001, 0b11001110, 0b10001001, 0b11010111, 0b10001001, 0b11100010, 0b10001001, 0b11101001, 0b10001001, 0b11110011, 0b10001001, 0b11111000 
        };
        AssertEqualString(String("mov ax, 1\nmov bx, 2\nmov cx, 3\nmov dx, 4\nmov sp, ax\nmov bp, bx\nmov si, cx\nmov di, dx\nmov dx, sp\nmov cx, bp\nmov bx, si\nmov ax, di"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualU32(4, Context.Registers[RegisterWord_AX]);
        AssertEqualU32(3, Context.Registers[RegisterWord_BX]);
        AssertEqualU32(2, Context.Registers[RegisterWord_CX]);
        AssertEqualU32(1, Context.Registers[RegisterWord_DX]);
        AssertEqualU32(1, Context.Registers[RegisterWord_SP]);
        AssertEqualU32(2, Context.Registers[RegisterWord_BP]);
        AssertEqualU32(3, Context.Registers[RegisterWord_SI]);
        AssertEqualU32(4, Context.Registers[RegisterWord_DI]);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111000, 0b00100010, 0b00100010, 0b10111011, 0b01000100, 0b01000100, 0b10111001, 0b01100110, 0b01100110, 0b10111010, 0b10001000, 0b10001000, 0b10001110, 0b11010000, 0b10001110, 0b11011011, 0b10001110, 0b11000001, 0b10110000, 0b00010001, 0b10110111, 0b00110011, 0b10110001, 0b01010101, 0b10110110, 0b01110111, 0b10001000, 0b11011100, 0b10001000, 0b11110001, 0b10001110, 0b11010000, 0b10001110, 0b11011011, 0b10001110, 0b11000001, 0b10001100, 0b11010100, 0b10001100, 0b11011101, 0b10001100, 0b11000110, 0b10001001, 0b11010111
        };
        AssertEqualString(String("mov ax, 8738\nmov bx, 17476\nmov cx, 26214\nmov dx, -30584\nmov ss, ax\nmov ds, bx\nmov es, cx\nmov al, 17\nmov bh, 51\nmov cl, 85\nmov dh, 119\nmov ah, bl\nmov cl, dh\nmov ss, ax\nmov ds, bx\nmov es, cx\nmov sp, ss\nmov bp, ds\nmov si, es\nmov di, dx"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
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
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111000, 0b00100010, 0b00100010, 0b10111011, 0b01000100, 0b01000100, 0b10111001, 0b01100110, 0b01100110, 0b10111010, 0b10001000, 0b10001000, 0b10001110, 0b11010000, 0b10001110, 0b11011011, 0b10001110, 0b11000001, 0b10110000, 0b00010001, 0b10110111, 0b00110011, 0b10110001, 0b01010101, 0b10110110, 0b01110111, 0b10001000, 0b11011100, 0b10001000, 0b11110001, 0b10001110, 0b11010000, 0b10001110, 0b11011011, 0b10001110, 0b11000001, 0b10001100, 0b11010100, 0b10001100, 0b11011101, 0b10001100, 0b11000110, 0b10001001, 0b11010111
        };
        AssertEqualString(String("mov ax, 8738\nmov bx, 17476\nmov cx, 26214\nmov dx, -30584\nmov ss, ax\nmov ds, bx\nmov es, cx\nmov al, 17\nmov bh, 51\nmov cl, 85\nmov dh, 119\nmov ah, bl\nmov cl, dh\nmov ss, ax\nmov ds, bx\nmov es, cx\nmov sp, ss\nmov bp, ds\nmov si, es\nmov di, dx"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
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
        EndTemporaryMemory(MemoryFlush);
    }
}

internal void
RunAddSubCmpTests(memory_arena *Arena)
{
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111011, 0b00000011, 0b11110000, 0b10111001, 0b00000001, 0b00001111, 0b00101001, 0b11001011, 0b10111100, 0b11100110, 0b00000011, 0b10111101, 0b11100111, 0b00000011, 0b00111001, 0b11100101, 0b10000001, 0b11000101, 0b00000011, 0b00000100, 0b10000001, 0b11101101, 0b11101010, 0b00000111
        };
        AssertEqualString(String("mov bx, -4093\nmov cx, 3841\nsub bx, cx\nmov sp, 998\nmov bp, 999\ncmp bp, sp\nadd bp, 1027\nsub bp, 2026"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        SimulateStep(&Context); // mov bx, -4093
        AssertEqualHex(0xf003, Context.Registers[RegisterWord_BX]);
        SimulateStep(&Context); // mov cx, 3841
        AssertEqualHex(0xf01, Context.Registers[RegisterWord_CX]);
        AssertEqualBits16(0, Context.Flags);
        SimulateStep(&Context); // sub bx, cx
        AssertEqualHex(0xe102, Context.Registers[RegisterWord_BX]);
        AssertEqualBits16(Flag_SF, Context.Flags);
        SimulateStep(&Context);
        AssertEqualHex(0x3e6, Context.Registers[RegisterWord_SP]);
        SimulateStep(&Context);
        AssertEqualHex(0x3e7, Context.Registers[RegisterWord_BP]);
        SimulateStep(&Context);
        AssertEqualBits16(0, Context.Flags);
        SimulateStep(&Context);
        AssertEqualHex(0x7ea, Context.Registers[RegisterWord_BP]);
        AssertEqualBits16(0, Context.Flags);
        SimulateStep(&Context);
        AssertEqualHex(0, Context.Registers[RegisterWord_BP]);
        AssertEqualBits16((Flag_ZF | Flag_PF), Context.Flags);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111011, 0b00000011, 0b11110000, 0b10111001, 0b00000001, 0b00001111, 0b00101001, 0b11001011, 0b10111100, 0b11100110, 0b00000011, 0b10111101, 0b11100111, 0b00000011, 0b00111001, 0b11100101, 0b10000001, 0b11000101, 0b00000011, 0b00000100, 0b10000001, 0b11101101, 0b11101010, 0b00000111
        };
        AssertEqualString(String("mov bx, -4093\nmov cx, 3841\nsub bx, cx\nmov sp, 998\nmov bp, 999\ncmp bp, sp\nadd bp, 1027\nsub bp, 2026"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualU32(57602, Context.Registers[RegisterWord_BX]);
        AssertEqualU32(3841, Context.Registers[RegisterWord_CX]);
        AssertEqualU32(998, Context.Registers[RegisterWord_SP]);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10000001, 0b11000011, 0b00110000, 0b01110101, 0b10000001, 0b11000011, 0b00010000, 0b00100111, 0b10000001, 0b11101011, 0b10001000, 0b00010011, 0b10000001, 0b11101011, 0b10001000, 0b00010011, 0b10111011, 0b00000001, 0b00000000, 0b10111001, 0b01100100, 0b00000000, 0b00000001, 0b11001011, 0b10111010, 0b00001010, 0b00000000, 0b00101001, 0b11010001, 0b10000001, 0b11000011, 0b01000000, 0b10011100, 0b10000011, 0b11000001, 0b10100110, 0b10111100, 0b01100011, 0b00000000, 0b10111101, 0b01100010, 0b00000000, 0b00111001, 0b11100101
        };
        AssertEqualString(String("add bx, 30000\nadd bx, 10000\nsub bx, 5000\nsub bx, 5000\nmov bx, 1\nmov cx, 100\nadd bx, cx\nmov dx, 10\nsub cx, dx\nadd bx, 40000\nadd cx, -90\nmov sp, 99\nmov bp, 98\ncmp bp, sp"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        AssertEqualBits(0, Context.Flags);
        SimulateStep(&Context); // add bx, 30000
        AssertEqualHex(0x7530, Context.Registers[RegisterWord_BX]);
        AssertEqualBits(Flag_PF, Context.Flags);
        SimulateStep(&Context); // add bx, 10000
        AssertEqualHex(0x9c40, Context.Registers[RegisterWord_BX]);
        AssertEqualBits16((Flag_SF|Flag_OF), Context.Flags);
        SimulateStep(&Context); // sub bx, 5000
        AssertEqualHex(0x88b8, Context.Registers[RegisterWord_BX]);
        AssertEqualBits16((Flag_PF|Flag_AF|Flag_SF), Context.Flags);
        SimulateStep(&Context); // sub bx, 5000
        AssertEqualHex(0x7530, Context.Registers[RegisterWord_BX]);
        AssertEqualBits16((Flag_PF|Flag_OF), Context.Flags);
        SimulateStep(&Context); // mov bx, 1
        AssertEqualHex(0x1, Context.Registers[RegisterWord_BX]);
        SimulateStep(&Context); // mov cx, 100
        AssertEqualHex(0x64, Context.Registers[RegisterWord_CX]);
        SimulateStep(&Context); // add bx, cx
        AssertEqualHex(0x65, Context.Registers[RegisterWord_BX]);
        AssertEqualBits16((Flag_PF), Context.Flags);
        SimulateStep(&Context); // mov dx, 10
        AssertEqualHex(0xa, Context.Registers[RegisterWord_DX]);
        SimulateStep(&Context);// sub cx, dx
        AssertEqualHex(0x5a, Context.Registers[RegisterWord_CX]);
        AssertEqualBits16((Flag_PF|Flag_AF), Context.Flags);
        SimulateStep(&Context); // add bx, 40000
        AssertEqualHex(0x9ca5, Context.Registers[RegisterWord_BX]);
        AssertEqualBits16((Flag_PF|Flag_SF|Flag_OF), Context.Flags); // NOTE(kstandbridge): Not sure if overflow is correct here
        SimulateStep(&Context); // add cx, -90
        AssertEqualHex(0, Context.Registers[RegisterWord_CX]);
        AssertEqualBits16((Flag_CF|Flag_PF|Flag_AF|Flag_ZF), Context.Flags);
        SimulateStep(&Context); // mov sp, 99
        AssertEqualHex(0x63, Context.Registers[RegisterWord_SP]);
        SimulateStep(&Context); // mov bp, 98
        AssertEqualHex(0x62, Context.Registers[RegisterWord_BP]);
        SimulateStep(&Context); // cmp bp, sp
        AssertEqualBits16((Flag_CF|Flag_PF|Flag_AF|Flag_SF), Context.Flags);
        
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10000001, 0b11000011, 0b00110000, 0b01110101, 0b10000001, 0b11000011, 0b00010000, 0b00100111, 0b10000001, 0b11101011, 0b10001000, 0b00010011, 0b10000001, 0b11101011, 0b10001000, 0b00010011, 0b10111011, 0b00000001, 0b00000000, 0b10111001, 0b01100100, 0b00000000, 0b00000001, 0b11001011, 0b10111010, 0b00001010, 0b00000000, 0b00101001, 0b11010001, 0b10000001, 0b11000011, 0b01000000, 0b10011100, 0b10000011, 0b11000001, 0b10100110, 0b10111100, 0b01100011, 0b00000000, 0b10111101, 0b01100010, 0b00000000, 0b00111001, 0b11100101
        };
        AssertEqualString(String("add bx, 30000\nadd bx, 10000\nsub bx, 5000\nsub bx, 5000\nmov bx, 1\nmov cx, 100\nadd bx, cx\nmov dx, 10\nsub cx, dx\nadd bx, 40000\nadd cx, -90\nmov sp, 99\nmov bp, 98\ncmp bp, sp"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualU32(40101, Context.Registers[RegisterWord_BX]);
        AssertEqualU32(10, Context.Registers[RegisterWord_DX]);
        AssertEqualU32(99, Context.Registers[RegisterWord_SP]);
        AssertEqualU32(98, Context.Registers[RegisterWord_BP]);
        EndTemporaryMemory(MemoryFlush);
    }
    
}

internal void 
RunIPRegisterTests(memory_arena *Arena)
{
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111001,  0b11001000,  0b00000000,  0b10001001,  0b11001011,  0b10000001,  0b11000001,  0b11101000,  0b00000011,  0b10111011,  0b11010000,  0b00000111,  0b00101001,  0b11011001
        };
        AssertEqualString(String("mov cx, -56\nmov bx, cx\nadd cx, 1000\nmov bx, 2000\nsub cx, bx"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        string Output = SimulateStep(&Context);
        AssertEqualString(String("mov cx, -56 ; cx:0x0->0xc8 ip:0x0->0x3"), Output);
        Output = SimulateStep(&Context);
        AssertEqualString(String("mov bx, cx ; bx:0x0->0xc8 ip:0x3->0x5"), Output);
        Output = SimulateStep(&Context);
        AssertEqualString(String("add cx, 1000 ; cx:0xc8->0x4b0 ip:0x5->0x9 flags:->A"), Output);
        Output = SimulateStep(&Context);
        AssertEqualString(String("mov bx, 2000 ; bx:0xc8->0x7d0 ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context);
        AssertEqualString(String("sub cx, bx ; cx:0x4b0->0xfce0 ip:0xc->0xe flags:A->CS"), Output);
        AssertEqualHex(0xfce0, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0xe, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_CF|Flag_SF), Context.Flags);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111001,  0b11001000,  0b00000000,  0b10001001,  0b11001011,  0b10000001,  0b11000001,  0b11101000,  0b00000011,  0b10111011,  0b11010000,  0b00000111,  0b00101001,  0b11011001
        };
        AssertEqualString(String("mov cx, -56\nmov bx, cx\nadd cx, 1000\nmov bx, 2000\nsub cx, bx"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualU32(0x7d0, Context.Registers[RegisterWord_BX]);
        AssertEqualU32(0xfce0, Context.Registers[RegisterWord_CX]);
        AssertEqualU32(0xe, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_CF|Flag_SF), Context.Flags);
        EndTemporaryMemory(MemoryFlush);
    }
}

internal void 
RunIPConditionalJumpTests(memory_arena *Arena)
{
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111001,  0b00000011,  0b00000000,  0b10111011,  0b11101000,  0b00000011,  0b10000011,  0b11000011,  0b00001010,  0b10000011,  0b11101001,  0b00000001,  0b01110101,  0b11111000
        };
        AssertEqualString(String("mov cx, 3\nmov bx, 1000\nadd bx, 10\nsub cx, 1\njne $-6"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        SimulateStep(&Context); // mov cx, 3
        AssertEqualHex(0x3, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x3, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // mov bx, 1000
        AssertEqualHex(0x3e8, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x6, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // add bx, 10
        AssertEqualHex(0x3f2, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_AF), Context.Flags);
        SimulateStep(&Context); // sub cx, 16
        AssertEqualHex(0x2, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0xc, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // jne $-6
        AssertEqualHex(0x6, Context.InstructionStreamAt);
        
        SimulateStep(&Context); // add bx, 10
        AssertEqualHex(0x3fc, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF), Context.Flags);
        SimulateStep(&Context); // sub cx, 16
        AssertEqualHex(0x1, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0xc, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // jne $-6
        AssertEqualHex(0x6, Context.InstructionStreamAt);
        
        SimulateStep(&Context); // add bx, 10
        AssertEqualHex(0x406, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF|Flag_AF), Context.Flags);
        SimulateStep(&Context); // sub cx, 16
        AssertEqualHex(0x0, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0xc, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF|Flag_ZF), Context.Flags);
        SimulateStep(&Context); // jne $-6
        AssertEqualHex(0xe, Context.InstructionStreamAt);
        
        AssertEqualHex(0x406, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0xe, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF|Flag_ZF), Context.Flags);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111001,  0b00000011,  0b00000000,  0b10111011,  0b11101000,  0b00000011,  0b10000011,  0b11000011,  0b00001010,  0b10000011,  0b11101001,  0b00000001,  0b01110101,  0b11111000
        };
        AssertEqualString(String("mov cx, 3\nmov bx, 1000\nadd bx, 10\nsub cx, 1\njne $-6"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualHex(0x406, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0xe, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF|Flag_ZF), Context.Flags);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111000, 0b00001010, 0b00000000, 0b10111011, 0b00001010, 0b00000000, 0b10111001, 0b00001010, 0b00000000, 0b00111001, 0b11001011, 0b01110100, 0b00000101, 0b10000011, 0b11000000, 0b00000001, 0b01111010, 0b00000101, 0b10000011, 0b11101011, 0b00000101, 0b01110010, 0b00000011, 0b10000011, 0b11101001, 0b00000010, 0b11100000, 0b11101101
        };
        AssertEqualString(String("mov ax, 10\nmov bx, 10\nmov cx, 10\ncmp bx, cx\nje $7\nadd ax, 1\njp $7\nsub bx, 5\njb $5\nsub cx, 2\nloopnz $-17"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        SimulateStep(&Context); // mov ax, 10
        AssertEqualHex(0xa, Context.Registers[RegisterWord_AX]);
        AssertEqualHex(0x3, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // mov bx, 10
        AssertEqualHex(0xa, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x6, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // mov cx, 10
        AssertEqualHex(0xa, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // cmp bx, cx
        AssertEqualHex(0xb, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF|Flag_ZF), Context.Flags);
        SimulateStep(&Context); // je $7
        AssertEqualHex(0x12, Context.InstructionStreamAt);
        SimulateStep(&Context); // sub bx, 5
        AssertEqualHex(0x5, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x15, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF), Context.Flags);
        SimulateStep(&Context); // jb $+5
        AssertEqualHex(0x17, Context.InstructionStreamAt);
        SimulateStep(&Context); // sub cx, 2
        AssertEqualHex(0x8, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x1a, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // loopnz $-17
        AssertEqualHex(0x7, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        SimulateStep(&Context); // cmp bx, cx
        AssertEqualHex(0xb, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_CF|Flag_AF|Flag_SF), Context.Flags);
        SimulateStep(&Context); // jb $+7
        AssertEqualHex(0xd, Context.InstructionStreamAt);
        SimulateStep(&Context); // add ax, 1
        AssertEqualHex(0xb, Context.Registers[RegisterWord_AX]);
        AssertEqualHex(0x10, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // jb $+7
        AssertEqualHex(0x12, Context.InstructionStreamAt);
        SimulateStep(&Context); // sub bx, 5
        AssertEqualHex(0x0, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x15, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF|Flag_ZF), Context.Flags);
        SimulateStep(&Context); // jb $+5
        AssertEqualHex(0x17, Context.InstructionStreamAt);
        SimulateStep(&Context); // sub cx, 2
        AssertEqualHex(0x5, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x1a, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF), Context.Flags);
        SimulateStep(&Context); // loopnz $-17
        AssertEqualHex(0x4, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        SimulateStep(&Context); // cmp bx, cx
        AssertEqualHex(0xb, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_CF|Flag_PF|Flag_AF|Flag_SF), Context.Flags);
        SimulateStep(&Context); // jb $+7
        AssertEqualHex(0xd, Context.InstructionStreamAt);
        SimulateStep(&Context); // add ax, 1
        AssertEqualHex(0xc, Context.Registers[RegisterWord_AX]);
        AssertEqualHex(0x10, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF), Context.Flags);
        SimulateStep(&Context); // jb $+7
        AssertEqualHex(0x17, Context.InstructionStreamAt);
        SimulateStep(&Context); // sub cx, 2
        AssertEqualHex(0x2, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x1a, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // loopnz $-17
        AssertEqualHex(0x1, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        SimulateStep(&Context); // cmp bx, cx
        AssertEqualHex(0xb, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_CF|Flag_PF|Flag_AF|Flag_SF), Context.Flags);
        SimulateStep(&Context); // je $+7
        AssertEqualHex(0xd, Context.InstructionStreamAt);
        SimulateStep(&Context); // add ax, 1
        AssertEqualHex(0xd, Context.Registers[RegisterWord_AX]);
        AssertEqualHex(0x10, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // je $+7
        AssertEqualHex(0x12, Context.InstructionStreamAt);
        SimulateStep(&Context); // sub bx, 5
        AssertEqualHex(0xfffb, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x15, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_CF|Flag_AF|Flag_SF), Context.Flags);
        SimulateStep(&Context); // je $+5
        AssertEqualHex(0x1a, Context.InstructionStreamAt);
        SimulateStep(&Context); // loopnz $-17
        AssertEqualHex(0x0, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x1c, Context.InstructionStreamAt);
        
        AssertEqualU32(0xd, Context.Registers[RegisterWord_AX]);
        AssertEqualU32(0xfffb, Context.Registers[RegisterWord_BX]);
        AssertEqualU32(0x1c, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_CF|Flag_AF|Flag_SF), Context.Flags);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111000, 0b00001010, 0b00000000, 0b10111011, 0b00001010, 0b00000000, 0b10111001, 0b00001010, 0b00000000, 0b00111001, 0b11001011, 0b01110100, 0b00000101, 0b10000011, 0b11000000, 0b00000001, 0b01111010, 0b00000101, 0b10000011, 0b11101011, 0b00000101, 0b01110010, 0b00000011, 0b10000011, 0b11101001, 0b00000010, 0b11100000, 0b11101101
        };
        AssertEqualString(String("mov ax, 10\nmov bx, 10\nmov cx, 10\ncmp bx, cx\nje $7\nadd ax, 1\njp $7\nsub bx, 5\njb $5\nsub cx, 2\nloopnz $-17"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualHex(0xd, Context.Registers[RegisterWord_AX]);
        AssertEqualHex(0xfffb, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x1c, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_CF|Flag_AF|Flag_SF), Context.Flags);
        EndTemporaryMemory(MemoryFlush);
    }
}

internal void
RunMemoryMovTests(memory_arena *Arena)
{
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b11000111, 0b00000110, 0b11101000, 0b00000011, 0b00000001, 0b00000000, 0b11000111, 0b00000110, 0b11101010, 0b00000011, 0b00000010, 0b00000000, 0b11000111, 0b00000110, 0b11101100, 0b00000011, 0b00000011, 0b00000000, 0b11000111, 0b00000110, 0b11101110, 0b00000011, 0b00000100, 0b00000000, 0b10111011, 0b11101000, 0b00000011, 0b11000111, 0b01000111, 0b00000100, 0b00001010, 0b00000000, 0b10001011, 0b00011110, 0b11101000, 0b00000011, 0b10001011, 0b00001110, 0b11101010, 0b00000011, 0b10001011, 0b00010110, 0b11101100, 0b00000011, 0b10001011, 0b00101110, 0b11101110, 0b00000011
        };
        AssertEqualString(String("mov word [1000], 1\nmov word [1002], 2\nmov word [1004], 3\nmov word [1006], 4\nmov bx, 1000\nmov word [bx+4], 10\nmov bx, word [1000]\nmov cx, word [1002]\nmov dx, word [1004]\nmov bp, word [1006]"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        SimulateStep(&Context); // mov word [1000], 1
        AssertEqualHex(0x6, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov word [1002], 2
        AssertEqualHex(0xc, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov word [1004], 3
        AssertEqualHex(0x12, Context.InstructionStreamAt);
        AssertEqualHex(0x3, Context.Memory[1004]);
        SimulateStep(&Context); // mov word [1006], 4
        AssertEqualHex(0x18, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov bx, 1000
        AssertEqualHex(0x1b, Context.InstructionStreamAt);
        AssertEqualHex(0x3e8, Context.Registers[RegisterWord_BX]);
        SimulateStep(&Context); // mov word [bx + 4], 10
        AssertEqualHex(0x20, Context.InstructionStreamAt);
        AssertEqualHex(0xa, Context.Memory[1004]);
        SimulateStep(&Context); // mov bx, word [1000]
        AssertEqualHex(0x1, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x24, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov cx, word [1002]
        AssertEqualHex(0x2, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x28, Context.InstructionStreamAt);
        AssertEqualHex(0x0, Context.Registers[RegisterWord_DX]);
        SimulateStep(&Context); // mov dx, word [1004]
        AssertEqualHex(0xa, Context.Registers[RegisterWord_DX]);
        AssertEqualHex(0x2c, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov bp, word [1006]
        AssertEqualHex(0x4, Context.Registers[RegisterWord_BP]);
        AssertEqualHex(0x30, Context.InstructionStreamAt);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b11000111, 0b00000110, 0b11101000, 0b00000011, 0b00000001, 0b00000000, 0b11000111, 0b00000110, 0b11101010, 0b00000011, 0b00000010, 0b00000000, 0b11000111, 0b00000110, 0b11101100, 0b00000011, 0b00000011, 0b00000000, 0b11000111, 0b00000110, 0b11101110, 0b00000011, 0b00000100, 0b00000000, 0b10111011, 0b11101000, 0b00000011, 0b11000111, 0b01000111, 0b00000100, 0b00001010, 0b00000000, 0b10001011, 0b00011110, 0b11101000, 0b00000011, 0b10001011, 0b00001110, 0b11101010, 0b00000011, 0b10001011, 0b00010110, 0b11101100, 0b00000011, 0b10001011, 0b00101110, 0b11101110, 0b00000011
        };
        AssertEqualString(String("mov word [1000], 1\nmov word [1002], 2\nmov word [1004], 3\nmov word [1006], 4\nmov bx, 1000\nmov word [bx+4], 10\nmov bx, word [1000]\nmov cx, word [1002]\nmov dx, word [1004]\nmov bp, word [1006]"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualHex(0x1, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x2, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0xa, Context.Registers[RegisterWord_DX]);
        AssertEqualHex(0x4, Context.Registers[RegisterWord_BP]);
        AssertEqualHex(0x30, Context.InstructionStreamAt);
        EndTemporaryMemory(MemoryFlush);
    }
}

internal void
RunMemoryAddLoopTests(memory_arena *Arena)
{
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111010, 0b00000110, 0b00000000, 0b10111101, 0b11101000, 0b00000011, 0b10111110, 0b00000000, 0b00000000, 0b10001001, 0b00110010, 0b10000011, 0b11000110, 0b00000010, 0b00111001, 0b11010110, 0b01110101, 0b11110111, 0b10111011, 0b00000000, 0b00000000, 0b10111110, 0b00000000, 0b00000000, 0b10001011, 0b00001010, 0b00000001, 0b11001011, 0b10000011, 0b11000110, 0b00000010, 0b00111001, 0b11010110, 0b01110101, 0b11110101
        };
        AssertEqualString(String("mov dx, 6\nmov bp, 1000\nmov si, 0\nmov word [bp + si], si\nadd si, 2\ncmp si, dx\njne $-7\nmov bx, 0\nmov si, 0\nmov cx, word [bp + si]\nadd bx, cx\nadd si, 2\ncmp si, dx\njne $-9"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        SimulateStep(&Context); // mov dx, 6;
        AssertEqualHex(0x6, Context.Registers[RegisterWord_DX]);
        AssertEqualHex(0x3, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov bp, 1000
        AssertEqualHex(0x3e8, Context.Registers[RegisterWord_BP]);
        AssertEqualHex(0x6, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov si, 0
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov word [bp+si], si
        AssertEqualHex(0xb, Context.InstructionStreamAt);
        SimulateStep(&Context); // add si, 2
        AssertEqualHex(0x2, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0xe, Context.InstructionStreamAt);
        SimulateStep(&Context); // cmp si, dx
        AssertEqualBits16((Flag_CF|Flag_PF|Flag_AF|Flag_SF), Context.Flags);
        AssertEqualHex(0x10, Context.InstructionStreamAt);
        SimulateStep(&Context); // jne $-7
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov word [bp+si], si
        AssertEqualHex(0x2, Context.Memory[1000 + 2]);
        AssertEqualHex(0xb, Context.InstructionStreamAt);
        SimulateStep(&Context); // add si, 2
        AssertEqualHex(0x4, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0xe, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // cmp si, dx
        AssertEqualHex(0x10, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_CF|Flag_AF|Flag_SF), Context.Flags);
        SimulateStep(&Context); // jne $-7
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov word [bp+si], si
        AssertEqualHex(0x4, Context.Memory[1000 + 4]);
        AssertEqualHex(0xb, Context.InstructionStreamAt);
        SimulateStep(&Context); // add si, 2
        AssertEqualHex(0x6, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0xe, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF), Context.Flags);
        SimulateStep(&Context); // cmp si, dx
        AssertEqualHex(0x10, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF|Flag_ZF), Context.Flags);
        SimulateStep(&Context); // jne $-7
        AssertEqualHex(0x12, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov bx, 0
        AssertEqualHex(0x15, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov si, 0
        AssertEqualHex(0x0, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0x18, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov cx, [bp+si]
        AssertEqualHex(0x4, Context.Memory[1000 + 4]);
        AssertEqualHex(0x1a, Context.InstructionStreamAt);
        SimulateStep(&Context); // add bx, cx
        AssertEqualHex(0x1c, Context.InstructionStreamAt);
        SimulateStep(&Context); // add si, 2
        AssertEqualHex(0x1f, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        AssertEqualHex(0x2, Context.Registers[RegisterWord_SI]);
        SimulateStep(&Context); // cmp si, dx
        AssertEqualBits16((Flag_CF|Flag_PF|Flag_AF|Flag_SF), Context.Flags);
        AssertEqualHex(0x21, Context.InstructionStreamAt);
        SimulateStep(&Context); // jne $-9
        AssertEqualHex(0x18, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov cx, [bp+si]
        AssertEqualHex(0x2, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x1a, Context.InstructionStreamAt);
        SimulateStep(&Context); // add bx, cx
        AssertEqualHex(0x2, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x1C, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // add si, 2
        AssertEqualHex(0x4, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0x1f, Context.InstructionStreamAt);
        SimulateStep(&Context); // cmp si, dx
        AssertEqualBits16((Flag_CF|Flag_AF|Flag_SF), Context.Flags);
        AssertEqualHex(0x21, Context.InstructionStreamAt);
        SimulateStep(&Context); // jne $-9
        AssertEqualHex(0x18, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov cx, [bp+si]
        AssertEqualHex(0x4, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x1a, Context.InstructionStreamAt);
        SimulateStep(&Context); // add bx, cx
        AssertEqualHex(0x6, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x1C, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF), Context.Flags);
        SimulateStep(&Context); // add si, 2
        AssertEqualHex(0x6, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0x1f, Context.InstructionStreamAt);
        SimulateStep(&Context); // cmp si, dx
        AssertEqualHex(0x21, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF|Flag_ZF), Context.Flags);
        SimulateStep(&Context); // jne $-9
        AssertEqualHex(0x23, Context.InstructionStreamAt);
        
        AssertEqualHex(0x6, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0x4, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x6, Context.Registers[RegisterWord_DX]);
        AssertEqualHex(0x3e8, Context.Registers[RegisterWord_BP]);
        AssertEqualHex(0x6, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0x23, Context.InstructionStreamAt);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111010, 0b00000110, 0b00000000, 0b10111101, 0b11101000, 0b00000011, 0b10111110, 0b00000000, 0b00000000, 0b10001001, 0b00110010, 0b10000011, 0b11000110, 0b00000010, 0b00111001, 0b11010110, 0b01110101, 0b11110111, 0b10111011, 0b00000000, 0b00000000, 0b10111110, 0b00000000, 0b00000000, 0b10001011, 0b00001010, 0b00000001, 0b11001011, 0b10000011, 0b11000110, 0b00000010, 0b00111001, 0b11010110, 0b01110101, 0b11110101
        };
        AssertEqualString(String("mov dx, 6\nmov bp, 1000\nmov si, 0\nmov word [bp + si], si\nadd si, 2\ncmp si, dx\njne $-7\nmov bx, 0\nmov si, 0\nmov cx, word [bp + si]\nadd bx, cx\nadd si, 2\ncmp si, dx\njne $-9"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualHex(0x6, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0x4, Context.Registers[RegisterWord_CX]);
        AssertEqualHex(0x6, Context.Registers[RegisterWord_DX]);
        AssertEqualHex(0x3e8, Context.Registers[RegisterWord_BP]);
        AssertEqualHex(0x6, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0x23, Context.InstructionStreamAt);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111010, 0b00000110, 0b00000000, 0b10111101, 0b11101000, 0b00000011, 0b10111110, 0b00000000, 0b00000000, 0b10001001, 0b00110010, 0b10000011, 0b11000110, 0b00000010, 0b00111001, 0b11010110, 0b01110101, 0b11110111, 0b10111011, 0b00000000, 0b00000000, 0b10001001, 0b11010110, 0b10000011, 0b11101101, 0b00000010, 0b00000011, 0b00011010, 0b10000011, 0b11101110, 0b00000010, 0b01110101, 0b11111001
        };
        AssertEqualString(String("mov dx, 6\nmov bp, 1000\nmov si, 0\nmov word [bp + si], si\nadd si, 2\ncmp si, dx\njne $-7\nmov bx, 0\nmov si, dx\nsub bp, 2\nadd bx, word [bp + si]\nsub si, 2\njne $-5"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        SimulateStep(&Context); // mov dx, 6
        AssertEqualHex(0x6, Context.Registers[RegisterWord_DX]);
        AssertEqualHex(0x3, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov bp, 1000
        AssertEqualHex(0x3e8, Context.Registers[RegisterWord_BP]);
        AssertEqualHex(0x6, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov si, 0
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov word [bp+si], si
        AssertEqualHex(0x0, Context.Memory[1000 + 0]);
        AssertEqualHex(0xb, Context.InstructionStreamAt);
        SimulateStep(&Context); // add si, 2
        AssertEqualHex(0x2, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0xe, Context.InstructionStreamAt);
        SimulateStep(&Context); // cmp si, dx
        AssertEqualBits16((Flag_CF|Flag_PF|Flag_AF|Flag_SF), Context.Flags);
        AssertEqualHex(0x10, Context.InstructionStreamAt);
        SimulateStep(&Context); // jne $-7
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov word [bp+si], si
        AssertEqualHex(0x2, Context.Memory[1000 + 2]);
        AssertEqualHex(0xb, Context.InstructionStreamAt);
        SimulateStep(&Context); // add si, 2
        AssertEqualHex(0x4, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0xe, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // cmp si, dx
        AssertEqualHex(0x10, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_CF|Flag_AF|Flag_SF), Context.Flags);
        SimulateStep(&Context); // jne $-7
        AssertEqualHex(0x9, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov word [bp+si], si
        AssertEqualHex(0x4, Context.Memory[1000 + 4]);
        AssertEqualHex(0xb, Context.InstructionStreamAt);
        SimulateStep(&Context); // add si, 2
        AssertEqualHex(0x6, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0xe, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF), Context.Flags);
        SimulateStep(&Context); // cmp si, dx
        AssertEqualHex(0x10, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF|Flag_ZF), Context.Flags);
        SimulateStep(&Context); // jne $-7
        AssertEqualHex(0x12, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov bx, 0
        AssertEqualHex(0x15, Context.InstructionStreamAt);
        SimulateStep(&Context); // mov si, dx
        AssertEqualHex(0x6, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0x6, Context.Registers[RegisterWord_DX]);
        AssertEqualHex(0x17, Context.InstructionStreamAt);
        SimulateStep(&Context); // sub bp, 2
        AssertEqualHex(0x3e6, Context.Registers[RegisterWord_BP]);
        AssertEqualHex(0x1a, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        AssertEqualHex(0x4, Context.Memory[0x3e6 + 6]); // BP + SI
        SimulateStep(&Context); // add bx, [bp+si]
        AssertEqualHex(0x4, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x1c, Context.InstructionStreamAt);
        SimulateStep(&Context); // sub si, 2
        AssertEqualHex(0x4, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0x1f, Context.InstructionStreamAt);
        SimulateStep(&Context); // jne $-5
        AssertEqualHex(0x1a, Context.InstructionStreamAt);
        SimulateStep(&Context); // add bx, [bp+si]
        AssertEqualHex(0x6, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x1c, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF), Context.Flags);
        SimulateStep(&Context); // sub si, 2
        AssertEqualHex(0x2, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0x1f, Context.InstructionStreamAt);
        AssertEqualBits16((0), Context.Flags);
        SimulateStep(&Context); // jne $-5
        AssertEqualHex(0x1a, Context.InstructionStreamAt);
        SimulateStep(&Context); // add bx, [bp+si]
        AssertEqualHex(0x1c, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF), Context.Flags);
        SimulateStep(&Context); // sub si, 2
        AssertEqualHex(0x0, Context.Registers[RegisterWord_SI]);
        AssertEqualHex(0x1f, Context.InstructionStreamAt);
        AssertEqualBits16((Flag_PF|Flag_ZF), Context.Flags);
        SimulateStep(&Context); // jne $-5
        AssertEqualHex(0x21, Context.InstructionStreamAt);
        AssertEqualHex(0x6, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x6, Context.Registers[RegisterWord_DX]);
        AssertEqualHex(0x3e6, Context.Registers[RegisterWord_BP]);
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111010, 0b00000110, 0b00000000, 0b10111101, 0b11101000, 0b00000011, 0b10111110, 0b00000000, 0b00000000, 0b10001001, 0b00110010, 0b10000011, 0b11000110, 0b00000010, 0b00111001, 0b11010110, 0b01110101, 0b11110111, 0b10111011, 0b00000000, 0b00000000, 0b10001001, 0b11010110, 0b10000011, 0b11101101, 0b00000010, 0b00000011, 0b00011010, 0b10000011, 0b11101110, 0b00000010, 0b01110101, 0b11111001
        };
        AssertEqualString(String("mov dx, 6\nmov bp, 1000\nmov si, 0\nmov word [bp + si], si\nadd si, 2\ncmp si, dx\njne $-7\nmov bx, 0\nmov si, dx\nsub bp, 2\nadd bx, word [bp + si]\nsub si, 2\njne $-5"), 
                          StreamToAssembly(MemoryFlush.Arena, Stream, sizeof(Stream)));
        EndTemporaryMemory(MemoryFlush); MemoryFlush = BeginTemporaryMemory(Arena);
        simulator_context Context = GetSimulatorContext(MemoryFlush.Arena, Stream, sizeof(Stream));
        Simulate(&Context);
        AssertEqualHex(0x6, Context.Registers[RegisterWord_BX]);
        AssertEqualHex(0x6, Context.Registers[RegisterWord_DX]);
        AssertEqualHex(0x3e6, Context.Registers[RegisterWord_BP]);
        AssertEqualHex(0x21, Context.InstructionStreamAt);
        EndTemporaryMemory(MemoryFlush);
    }
    
    
}

internal void
RunSimulateTests(memory_arena *Arena)
{
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111010, 0b00000110, 0b00000000, 0b10111101, 0b11101000, 0b00000011, 0b10111110, 0b00000000, 0b00000000, 0b10001001, 0b00110010, 0b10000011, 0b11000110, 0b00000010, 0b00111001, 0b11010110, 0b01110101, 0b11110111, 0b10111011, 0b00000000, 0b00000000, 0b10001001, 0b11010110, 0b10000011, 0b11101101, 0b00000010, 0b00000011, 0b00011010, 0b10000011, 0b11101110, 0b00000010, 0b01110101, 0b11111001
        };
        
        simulator_context Context = GetSimulatorContext(Arena, Stream, sizeof(Stream));
        
        string Output;
        Output = SimulateStep(&Context); AssertEqualString(String("mov dx, 6 ; dx:0x0->0x6 ip:0x0->0x3"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov bp, 1000 ; bp:0x0->0x3e8 ip:0x3->0x6"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov si, 0 ; ip:0x6->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp + si], si ; ip:0x9->0xb"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add si, 2 ; si:0x0->0x2 ip:0xb->0xe"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("cmp si, dx ; ip:0xe->0x10 flags:->CPAS"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-7 ; ip:0x10->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp + si], si ; ip:0x9->0xb"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add si, 2 ; si:0x2->0x4 ip:0xb->0xe flags:CPAS->"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("cmp si, dx ; ip:0xe->0x10 flags:->CAS"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-7 ; ip:0x10->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp + si], si ; ip:0x9->0xb"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add si, 2 ; si:0x4->0x6 ip:0xb->0xe flags:CAS->P"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("cmp si, dx ; ip:0xe->0x10 flags:P->PZ"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-7 ; ip:0x10->0x12"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov bx, 0 ; ip:0x12->0x15"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov si, dx ; ip:0x15->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("sub bp, 2 ; bp:0x3e8->0x3e6 ip:0x17->0x1a flags:PZ->"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bx, word [bp + si] ; bx:0x0->0x4 ip:0x1a->0x1c"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("sub si, 2 ; si:0x6->0x4 ip:0x1c->0x1f"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-5 ; ip:0x1f->0x1a"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bx, word [bp + si] ; bx:0x4->0x6 ip:0x1a->0x1c flags:->P"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("sub si, 2 ; si:0x4->0x2 ip:0x1c->0x1f flags:P->"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-5 ; ip:0x1f->0x1a"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bx, word [bp + si] ; ip:0x1a->0x1c flags:->P"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("sub si, 2 ; si:0x2->0x0 ip:0x1c->0x1f flags:P->PZ"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-5 ; ip:0x1f->0x21"), Output);
        
        Output = GetRegisterDetails(&Context);
        AssertEqualString(String("\t\tdx: 0x0006 (6)\n\t\tbx: 0x0006 (6)\n\t\tbp: 0x03e6 (998)\n\t\tip: 0x0021 (33)\n\tflags: PZ"), Output);
        
        EndTemporaryMemory(MemoryFlush);
    }
}

internal void
RunDrawRectangleTests(memory_arena *Arena)
{
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111101, 0b00000000, 0b00000001, 0b10111010, 0b00000000, 0b00000000, 0b10111001, 0b00000000, 0b00000000, 0b10001001, 0b01001110, 0b00000000, 0b10001001, 0b01010110, 0b00000010, 0b11000110, 0b01000110, 0b00000011, 0b11111111, 0b10000011, 0b11000101, 0b00000100, 0b10000011, 0b11000001, 0b00000001, 0b10000011, 0b11111001, 0b01000000, 0b01110101, 0b11101011, 0b10000011, 0b11000010, 0b00000001, 0b10000011, 0b11111010, 0b01000000, 0b01110101, 0b11100000
        };
        
        simulator_context Context = GetSimulatorContext(Arena, Stream, sizeof(Stream));
        string Output;
        Output = SimulateStep(&Context); AssertEqualString(String("mov bp, 256 ; bp:0x0->0x100 ip:0x0->0x3"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov dx, 0 ; ip:0x3->0x6"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov cx, 0 ; ip:0x6->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp], cx ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp+2], dx ; ip:0xc->0xf"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0xf->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x100->0x104 ip:0x13->0x16"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add cx, 1 ; cx:0x0->0x1 ip:0x16->0x19"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("cmp cx, 64 ; ip:0x19->0x1c flags:->CS"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-19 ; ip:0x1c->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp], cx ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp+2], dx ; ip:0xc->0xf"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0xf->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x104->0x108 ip:0x13->0x16 flags:CS->"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add cx, 1 ; cx:0x1->0x2 ip:0x16->0x19"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("cmp cx, 64 ; ip:0x19->0x1c flags:->CS"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-19 ; ip:0x1c->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp], cx ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp+2], dx ; ip:0xc->0xf"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0xf->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x108->0x10c ip:0x13->0x16 flags:CS->P"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add cx, 1 ; cx:0x2->0x3 ip:0x16->0x19"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("cmp cx, 64 ; ip:0x19->0x1c flags:P->CPS"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-19 ; ip:0x1c->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp], cx ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp+2], dx ; ip:0xc->0xf"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0xf->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x10c->0x110 ip:0x13->0x16 flags:CPS->A"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add cx, 1 ; cx:0x3->0x4 ip:0x16->0x19 flags:A->"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("cmp cx, 64 ; ip:0x19->0x1c flags:->CS"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-19 ; ip:0x1c->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp], cx ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp+2], dx ; ip:0xc->0xf"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0xf->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x110->0x114 ip:0x13->0x16 flags:CS->P"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add cx, 1 ; cx:0x4->0x5 ip:0x16->0x19"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("cmp cx, 64 ; ip:0x19->0x1c flags:P->CPS"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-19 ; ip:0x1c->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp], cx ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp+2], dx ; ip:0xc->0xf"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0xf->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x114->0x118 ip:0x13->0x16 flags:CPS->P"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add cx, 1 ; cx:0x5->0x6 ip:0x16->0x19"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("cmp cx, 64 ; ip:0x19->0x1c flags:P->CPS"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-19 ; ip:0x1c->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp], cx ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp+2], dx ; ip:0xc->0xf"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0xf->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x118->0x11c ip:0x13->0x16 flags:CPS->"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add cx, 1 ; cx:0x6->0x7 ip:0x16->0x19"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("cmp cx, 64 ; ip:0x19->0x1c flags:->CS"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-19 ; ip:0x1c->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp], cx ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp+2], dx ; ip:0xc->0xf"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0xf->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x11c->0x120 ip:0x13->0x16 flags:CS->A"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add cx, 1 ; cx:0x7->0x8 ip:0x16->0x19 flags:A->"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("cmp cx, 64 ; ip:0x19->0x1c flags:->CS"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("jne $-19 ; ip:0x1c->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp], cx ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bp+2], dx ; ip:0xc->0xf"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0xf->0x13"), Output);
        // NOTE(kstandbridge): This goes on for 28k lines
        EndTemporaryMemory(MemoryFlush);
    }
    
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111101, 0b00000000, 0b00000001, 0b10111010, 0b01000000, 0b00000000, 0b10111001, 0b01000000, 0b00000000, 0b10001000, 0b01001110, 0b00000000, 0b11000110, 0b01000110, 0b00000001, 0b00000000, 0b10001000, 0b01010110, 0b00000010, 0b11000110, 0b01000110, 0b00000011, 0b11111111, 0b10000011, 0b11000101, 0b00000100, 0b11100010, 0b11101101, 0b10000011, 0b11101010, 0b00000001, 0b01110101, 0b11100101, 0b10111101, 0b00000100, 0b00000010, 0b10001001, 0b11101011, 0b10111001, 0b00111110, 0b00000000, 0b11000110, 0b01000110, 0b00000001, 0b11111111, 0b11000110, 0b10000110, 0b00000001, 0b00111101, 0b11111111, 0b11000110, 0b01000111, 0b00000001, 0b11111111, 0b11000110, 0b10000111, 0b11110101, 0b00000000, 0b11111111, 0b10000011, 0b11000101, 0b00000100, 0b10000001, 0b11000011, 0b00000000, 0b00000001, 0b11100010, 0b11100101
        };
        
        simulator_context Context = GetSimulatorContext(Arena, Stream, sizeof(Stream));
        string Output;
        Output = SimulateStep(&Context); AssertEqualString(String("mov bp, 256 ; bp:0x0->0x100 ip:0x0->0x3"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov dx, 64 ; dx:0x0->0x40 ip:0x3->0x6"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov cx, 64 ; cx:0x0->0x40 ip:0x6->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x100->0x104 ip:0x17->0x1a"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x40->0x3f ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x104->0x108 ip:0x17->0x1a"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x3f->0x3e ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x108->0x10c ip:0x17->0x1a flags:->P"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x3e->0x3d ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x10c->0x110 ip:0x17->0x1a flags:P->A"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x3d->0x3c ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x110->0x114 ip:0x17->0x1a flags:A->P"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x3c->0x3b ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x114->0x118 ip:0x17->0x1a"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x3b->0x3a ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x118->0x11c ip:0x17->0x1a flags:P->"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x3a->0x39 ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x11c->0x120 ip:0x17->0x1a flags:->A"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x39->0x38 ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x120->0x124 ip:0x17->0x1a flags:A->P"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x38->0x37 ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x124->0x128 ip:0x17->0x1a"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x37->0x36 ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x128->0x12c ip:0x17->0x1a flags:P->"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x36->0x35 ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x12c->0x130 ip:0x17->0x1a flags:->PA"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x35->0x34 ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x130->0x134 ip:0x17->0x1a flags:PA->"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x34->0x33 ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x134->0x138 ip:0x17->0x1a"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x33->0x32 ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x138->0x13c ip:0x17->0x1a flags:->P"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x32->0x31 ip:0x1a->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp], cl ; ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+1], 0 ; ip:0xc->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+2], dl ; ip:0x10->0x13"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov byte [bp+3], 255 ; ip:0x13->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add bp, 4 ; bp:0x13c->0x140 ip:0x17->0x1a flags:P->A"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("loop $-17 ; cx:0x31->0x30 ip:0x1a->0x9"), Output);
        // NOTE(kstandbridge): This goes on for 25k lines
        EndTemporaryMemory(MemoryFlush);
    }
}

internal void
RunEstimateCyclesTests(memory_arena *Arena)
{
    // NOTE(kstandbridge): 8086
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111011, 0b11101000, 0b00000011, 0b10111101, 0b11010000, 0b00000111, 0b10111110, 0b10111000, 0b00001011, 0b10111111, 0b10100000, 0b00001111, 0b10001001, 0b11011001, 0b10111010, 0b00001100, 0b00000000, 0b10001011, 0b00010110, 0b11101000, 0b00000011, 0b10001011, 0b00001111, 0b10001011, 0b01001110, 0b00000000, 0b10001001, 0b00001100, 0b10001001, 0b00001101, 0b10001011, 0b10001111, 0b11101000, 0b00000011, 0b10001011, 0b10001110, 0b11101000, 0b00000011, 0b10001001, 0b10001100, 0b11101000, 0b00000011, 0b10001001, 0b10001101, 0b11101000, 0b00000011, 0b00000001, 0b11010001, 0b00000001, 0b10001101, 0b11101000, 0b00000011, 0b10000011, 0b11000010, 0b00110010
        };
        
        simulator_context Context = GetSimulatorContext(Arena, Stream, sizeof(Stream));
        Context.DisplayClocks = true;
        
        string Output;
        Output = SimulateStep(&Context); AssertEqualString(String("mov bx, 1000 ; Clocks: +4 = 4 | bx:0x0->0x3e8 ip:0x0->0x3"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov bp, 2000 ; Clocks: +4 = 8 | bp:0x0->0x7d0 ip:0x3->0x6"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov si, 3000 ; Clocks: +4 = 12 | si:0x0->0xbb8 ip:0x6->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov di, 4000 ; Clocks: +4 = 16 | di:0x0->0xfa0 ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov cx, bx ; Clocks: +2 = 18 | cx:0x0->0x3e8 ip:0xc->0xe"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov dx, 12 ; Clocks: +4 = 22 | dx:0x0->0xc ip:0xe->0x11"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov dx, word [1000] ; Clocks: +14 = 36 (8 + 6ea) | dx:0xc->0x0 ip:0x11->0x15"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov cx, word [bx] ; Clocks: +13 = 49 (8 + 5ea) | cx:0x3e8->0x0 ip:0x15->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word cx, [bp] ; Clocks: +13 = 62 (8 + 5ea) | ip:0x17->0x1a"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [si], cx ; Clocks: +14 = 76 (9 + 5ea) | ip:0x1a->0x1c"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [di], cx ; Clocks: +14 = 90 (9 + 5ea) | ip:0x1c->0x1e"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word cx, [bx+1000] ; Clocks: +17 = 107 (8 + 9ea) | ip:0x1e->0x22"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word cx, [bp+1000] ; Clocks: +17 = 124 (8 + 9ea) | ip:0x22->0x26"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [si+1000], cx ; Clocks: +18 = 142 (9 + 9ea) | ip:0x26->0x2a"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [di+1000], cx ; Clocks: +18 = 160 (9 + 9ea) | ip:0x2a->0x2e"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add cx, dx ; Clocks: +3 = 163 | ip:0x2e->0x30 flags:->PZ"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add word [di+1000], cx ; Clocks: +25 = 188 (16 + 9ea) | ip:0x30->0x34"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add dx, 50 ; Clocks: +4 = 192 | dx:0x0->0x32 ip:0x34->0x37 flags:PZ->"), Output);
        
        EndTemporaryMemory(MemoryFlush);
    }
    
    // NOTE(kstandbridge): 8088
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111011, 0b11101000, 0b00000011, 0b10111101, 0b11010000, 0b00000111, 0b10111110, 0b10111000, 0b00001011, 0b10111111, 0b10100000, 0b00001111, 0b10001001, 0b11011001, 0b10111010, 0b00001100, 0b00000000, 0b10001011, 0b00010110, 0b11101000, 0b00000011, 0b10001011, 0b00001111, 0b10001011, 0b01001110, 0b00000000, 0b10001001, 0b00001100, 0b10001001, 0b00001101, 0b10001011, 0b10001111, 0b11101000, 0b00000011, 0b10001011, 0b10001110, 0b11101000, 0b00000011, 0b10001001, 0b10001100, 0b11101000, 0b00000011, 0b10001001, 0b10001101, 0b11101000, 0b00000011, 0b00000001, 0b11010001, 0b00000001, 0b10001101, 0b11101000, 0b00000011, 0b10000011, 0b11000010, 0b00110010
        };
        
        simulator_context Context = GetSimulatorContext(Arena, Stream, sizeof(Stream));
        Context.DisplayClocks = true;
        Context.Is8088 = true;
        
        string Output;
        Output = SimulateStep(&Context); AssertEqualString(String("mov bx, 1000 ; Clocks: +4 = 4 | bx:0x0->0x3e8 ip:0x0->0x3"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov bp, 2000 ; Clocks: +4 = 8 | bp:0x0->0x7d0 ip:0x3->0x6"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov si, 3000 ; Clocks: +4 = 12 | si:0x0->0xbb8 ip:0x6->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov di, 4000 ; Clocks: +4 = 16 | di:0x0->0xfa0 ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov cx, bx ; Clocks: +2 = 18 | cx:0x0->0x3e8 ip:0xc->0xe"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov dx, 12 ; Clocks: +4 = 22 | dx:0x0->0xc ip:0xe->0x11"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov dx, word [1000] ; Clocks: +18 = 40 (8 + 6ea + 4p) | dx:0xc->0x0 ip:0x11->0x15"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov cx, word [bx] ; Clocks: +17 = 57 (8 + 5ea + 4p) | cx:0x3e8->0x0 ip:0x15->0x17"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word cx, [bp] ; Clocks: +17 = 74 (8 + 5ea + 4p) | ip:0x17->0x1a"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [si], cx ; Clocks: +18 = 92 (9 + 5ea + 4p) | ip:0x1a->0x1c"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [di], cx ; Clocks: +18 = 110 (9 + 5ea + 4p) | ip:0x1c->0x1e"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word cx, [bx+1000] ; Clocks: +21 = 131 (8 + 9ea + 4p) | ip:0x1e->0x22"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word cx, [bp+1000] ; Clocks: +21 = 152 (8 + 9ea + 4p) | ip:0x22->0x26"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [si+1000], cx ; Clocks: +22 = 174 (9 + 9ea + 4p) | ip:0x26->0x2a"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [di+1000], cx ; Clocks: +22 = 196 (9 + 9ea + 4p) | ip:0x2a->0x2e"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add cx, dx ; Clocks: +3 = 199 | ip:0x2e->0x30 flags:->PZ"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add word [di+1000], cx ; Clocks: +33 = 232 (16 + 9ea + 8p) | ip:0x30->0x34"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add dx, 50 ; Clocks: +4 = 236 | dx:0x0->0x32 ip:0x34->0x37 flags:PZ->"), Output);
        
        EndTemporaryMemory(MemoryFlush);
    }
    
    // NOTE(kstandbridge): 8086
    {
        temporary_memory MemoryFlush = BeginTemporaryMemory(Arena);
        u8 Stream[] = 
        { 
            0b10111011, 0b11101000, 0b00000011, 0b10111101, 0b11010000, 0b00000111, 0b10111110, 0b10111000, 0b00001011, 0b10111111, 0b10100000, 0b00001111, 0b10001011, 0b00001011, 0b10001001, 0b00001000, 0b10001011, 0b00001010, 0b10001001, 0b00001001, 0b10001011, 0b10001011, 0b11101000, 0b00000011, 0b10001001, 0b10001000, 0b11101000, 0b00000011, 0b10001011, 0b10001010, 0b11101000, 0b00000011, 0b10001001, 0b10001001, 0b11101000, 0b00000011, 0b00000011, 0b10010010, 0b11101000, 0b00000011, 0b10000011, 0b00000010, 0b01001100, 0b00000011, 0b10010010, 0b11101001, 0b00000011, 0b00000001, 0b10010101, 0b11100111, 0b00000011, 0b10000011, 0b00000010, 0b01001011
        };
        
        simulator_context Context = GetSimulatorContext(Arena, Stream, sizeof(Stream));
        Context.DisplayClocks = true;
        
        string Output;
        Output = SimulateStep(&Context); AssertEqualString(String("mov bx, 1000 ; Clocks: +4 = 4 | bx:0x0->0x3e8 ip:0x0->0x3"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov bp, 2000 ; Clocks: +4 = 8 | bp:0x0->0x7d0 ip:0x3->0x6"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov si, 3000 ; Clocks: +4 = 12 | si:0x0->0xbb8 ip:0x6->0x9"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov di, 4000 ; Clocks: +4 = 16 | di:0x0->0xfa0 ip:0x9->0xc"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov cx, word [bp + di] ; Clocks: +15 = 31 (8 + 7ea) | ip:0xc->0xe"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bx + si], cx ; Clocks: +16 = 47 (9 + 7ea) | ip:0xe->0x10"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov cx, word [bp + si] ; Clocks: +16 = 63 (8 + 8ea) | ip:0x10->0x12"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bx + di], cx ; Clocks: +17 = 80 (9 + 8ea) | ip:0x12->0x14"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word cx, [bp + di+1000] ; Clocks: +19 = 99 (8 + 11ea) | ip:0x14->0x18"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bx + si+1000], cx ; Clocks: +20 = 119 (9 + 11ea) | ip:0x18->0x1c"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word cx, [bp + si+1000] ; Clocks: +20 = 139 (8 + 12ea) | ip:0x1c->0x20"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("mov word [bx + di+1000], cx ; Clocks: +21 = 160 (9 + 12ea) | ip:0x20->0x24"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add word dx, [bp + si+1000] ; Clocks: +21 = 181 (9 + 12ea) | ip:0x24->0x28 flags:->PZ"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add word [bp + si], 76 ; Clocks: +25 = 206 (17 + 8ea) | ip:0x28->0x2b flags:PZ->"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add word dx, [bp + si+1001] ; Clocks: +25 = 231 (9 + 12ea + 4p) | ip:0x2b->0x2f flags:->PZ"), Output);
#if 0
        Output = SimulateStep(&Context); AssertEqualString(String("add word [di+999], dx ; Clocks: +33 = 264 (16 + 9ea + 8p) | ip:0x2f->0x33 flags:PZ->P"), Output);
        Output = SimulateStep(&Context); AssertEqualString(String("add word [bp + si], 75 ; Clocks: +25 = 289 (17 + 8ea) | ip:0x33->0x36 flags:P->A"), Output);
#endif
        EndTemporaryMemory(MemoryFlush);
    }
}

void
RunAllTests(memory_arena *Arena)
{
    RunInstructionTableTests();
    RunDisassembleTests();
#if 0
    RunDisassembleToAssemblyTests(Arena);
#endif
    RunImmediateMovTests(Arena);
    RunRegisterMovTests(Arena);
    RunAddSubCmpTests(Arena);
    RunIPRegisterTests(Arena);
    RunIPConditionalJumpTests(Arena);
    RunMemoryMovTests(Arena);
    RunMemoryAddLoopTests(Arena);
    RunSimulateTests(Arena);
    RunDrawRectangleTests(Arena);
    RunEstimateCyclesTests(Arena);
    
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
