internal void
RunGetBitsTests(memory_arena *TransientArena, void *Data)
{
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

internal void
RunUnpackTests(memory_arena *TransientArena, void *Data)
{
    {
        u16 Value = 0x1234;
        u8 High = UnpackU16High(Value);
        u8 Low = UnpackU16Low(Value);
        AssertEqualU32(0x12, High);
        AssertEqualU32(0x34, Low);
    }
    
    {
        u16 Value = 0xABCD;
        u8 High = UnpackU16High(Value);
        u8 Low = UnpackU16Low(Value);
        AssertEqualU32(0xAB, High);
        AssertEqualU32(0xCD, Low);
    }
}

internal void
RunFormatStringHexTests(memory_arena *TransientArena, void *Data)
{
    
    {
        AssertEqualString(String("before 0xA after"), 
                          FormatString(TransientArena, "before %X after", 10));
    }
    
    {
        AssertEqualString(String("before 0xa after"), 
                          FormatString(TransientArena, "before %x after", 10));
    }
    
    {
        AssertEqualString(String("before 0x18F after"), 
                          FormatString(TransientArena, "before %X after", 399));
    }
    
    {
        AssertEqualString(String("before 0xABCD after"), 
                          FormatString(TransientArena, "before %X after", 0xABCD));
    }
    
    {
        AssertEqualString(String("before 0xabcd010 after"), 
                          FormatString(TransientArena, "before %x after", 0xABCD010));
    }
    
    {
        AssertEqualString(String("before 0x1234ABCD after"), 
                          FormatString(TransientArena, "before %X after", 0x1234ABCD));
    }
    
    {
        AssertEqualString(String("before <     0x18f> after"), 
                          FormatString(TransientArena, "before <%8x> after", 0x18f));
    }
    
    {
        AssertEqualString(String("before <0x0000018f> after"), 
                          FormatString(TransientArena, "before <%08x> after", 0x18f));
    }
    
}


internal void
RunStringsAreEqualTests(memory_arena *TransientArena, void *Data)
{
    AssertEqualString(String("Foo"), String("Foo"));
    AssertNotEqualString(String("Bar"), String("Foo"));
    AssertEqualString(String("Foo bar Bas"), String("Foo bar Bas"));
    AssertNotEqualString(String("Foo bar Bas"), String("Bas bar Foo"));
    AssertNotEqualString(String("Foo bar Bas"), String(""));
    AssertNotEqualString(String(""), String("Bas bar Foo"));
}

internal void
RunStringBeginsWithTests(memory_arena *TransientArena, void *Data)
{
    string HayStack = String("Content-Length: 256");
    AssertTrue(StringBeginsWith(String("Content-Length"), HayStack));
    AssertFalse(StringBeginsWith(String("256"), HayStack));
}

internal void
RunStringContainsTests(memory_arena *TransientArena, void *Data)
{
    AssertTrue(StringContains(String("foo"), String("Before foo after")));
    AssertFalse(StringContains(String("foo"), String("Before bar after")));
}

internal void
RunFormatStringSignedDecimalIntegerTests(memory_arena *TransientArena, void *Data)
{
    {
        string A = String("before 42 after");
        string B = FormatString(TransientArena, "before %i after", 42);
        AssertEqualString(A, B);
    }
    {
        string A = String("aaa 111 bbb 222 ccc 333 ddd");
        string B = FormatString(TransientArena, "aaa %d bbb %d ccc %d ddd", 111, 222, 333);
        AssertEqualString(A, B);
    }
    {
        string A = String("aaa 111 bbb 222 ccc 333 ddd");
        string B = FormatString(TransientArena, "aaa %d bbb %d ccc %d ddd", 444, 555, 666);
        AssertNotEqualString(A, B);
    }
    {
        string A = String("before 2147483647 after");
        string B = FormatString(TransientArena, "before %d after", S32Max);
        AssertEqualString(A, B);
    }
    {
        string A = String("before -2147483648 after");
        string B = FormatString(TransientArena, "before %d after", S32Min);
        AssertEqualString(A, B);
    }
    {
        string A = String("before 0 after");
        string B = FormatString(TransientArena, "before %u after", 0);
        AssertEqualString(A, B);
    }
    {
        string A = String("before  0 after");
        string B = FormatString(TransientArena, "before %2u after", 0);
        AssertEqualString(A, B);
    }
}

internal void
RunFormatStringUnsignedDecimalIntegerTests(memory_arena *TransientArena, void *Data)
{
    {
        string A = String("before 4294967295 after");
        string B = FormatString(TransientArena, "before %u after", U32Max);
        AssertEqualString(A, B);
    }
    {
        string A = String("before 0 after");
        string B = FormatString(TransientArena, "before %u after", U32Min);
        AssertEqualString(A, B);
    }
    {
        string A = String("before    42 after");
        string B = FormatString(TransientArena, "before %5u after", 42);
        AssertEqualString(A, B);
    }
    {
        string A = String("[  262144]");
        string B = FormatString(TransientArena, "[%8d]", 262144);
        AssertEqualString(A, B);
    }
    {
        string A = String("[       0]");
        string B = FormatString(TransientArena, "[%8d]", 0);
        AssertEqualString(A, B);
    }
    {
        string A = String("[ -262144]");
        string B = FormatString(TransientArena, "[%8d]", -262144);
        AssertEqualString(A, B);
    }
    {
        string A = String("[   123]");
        string B = FormatString(TransientArena, "[%6d]", 123);
        AssertEqualString(A, B);
    }
    {
        string A = String("[  -123]");
        string B = FormatString(TransientArena, "[%6d]", -123);
        AssertEqualString(A, B);
    }
}

internal void
RunFormatStringDecimalFloatingPoint(memory_arena *TransientArena, void *Data)
{
    {
        string A = String("before 3.141592 after");
        string B = FormatString(TransientArena, "before %f after", Pi32);
        AssertEqualString(A, B);
    }
    {
        string A = String("before 6.283185 after");
        string B = FormatString(TransientArena, "before %f after", Tau32);
        AssertEqualString(A, B);
    }
    {
        string A = String("before 3.141 after");
        string B = FormatString(TransientArena, "before %.3f after", Pi32);
        AssertEqualString(A, B);
    }
    {
        string A = String("before   3.141 after");
        string B = FormatString(TransientArena, "before %3.3f after", Pi32);
        AssertEqualString(A, B);
    }
    {
        string A = String("before 3.141 after");
        string B = FormatString(TransientArena, "before %.*f after", 3, Pi32);
        AssertEqualString(A, B);
    }
    // NOTE(kstandbridge): Intentionally checking string length 
    // as precision is not accurate with currently implementation 
    {
        string A = String("before 6.28318530717958647692 after");
        string B = FormatString(TransientArena, "before %.20f after", Tau32);
        AssertEqualU64(A.Size, B.Size);
    }
    {
        string A = String("before 3.14159265359 after");
        string B = FormatString(TransientArena, "before %.11f after", Pi32);
        AssertEqualU64(A.Size, B.Size);
    }
    
    {
        string A = String("[  262144.00]");
        string B = FormatString(TransientArena, "[%8.2f]", 262144.0f);
        AssertEqualString(A, B);
    }
    {
        string A = String("[       0.00]");
        string B = FormatString(TransientArena, "[%8.2f]", 0.0f);
        AssertEqualString(A, B);
    }

    {
        string A = String("[ -262144.00]");
        string B = FormatString(TransientArena, "[%8.2f]", -262144.0f);
        AssertEqualString(A, B);
    }

    {
        string A = String("[   123.00]");
        string B = FormatString(TransientArena, "[%6.2f]", 123.0f);
        AssertEqualString(A, B);
    }

    {
        string A = String("[  -123.00]");
        string B = FormatString(TransientArena, "[%6.2f]", -123.0f);
        AssertEqualString(A, B);
    }
}

internal void
RunFormatStringStringOfCharactersTests(memory_arena *TransientArena, void *Data)
{
    {
        string A = String("before bar after");
        string B = FormatString(TransientArena, "before %s after", "bar");
        AssertEqualString(A, B);
    }
    {
        string A = String("11 11 11 foo 22 22 22 bar 33 33 33 bas 44 44 44");
        string B = FormatString(TransientArena, "11 11 11 %s 22 22 22 %s 33 33 33 %s 44 44 44", "foo", "bar", "bas");
        AssertEqualString(A, B);
    }
    {
        string A = String("11 11 11 foo 22 22 22 bar 33 33 33 bas 44 44 44");
        string B = FormatString(TransientArena, "11 11 11 %s 22 22 22 %s 33 33 33 %s 44 44 44", "bas", "bar", "foo");
        AssertNotEqualString(A, B);
    }
    {
        string A = String("before foo after");
        string B = FormatString(TransientArena, "before %.3s after", "foobar");
        AssertEqualString(A, B);
    }
    {
        string A = String("before foo after");
        string B = FormatString(TransientArena, "before %.*s after", 3, "foobar");
        AssertEqualString(A, B);
    }
}

internal void
RunFormatStringStringTypeTests(memory_arena *TransientArena, void *Data)
{
    {
        string A = String("before bar after");
        string B = FormatString(TransientArena, "before %S after", String("bar"));
        AssertEqualString(A, B);
    }
    {
        string A = String("foo 1234 bar 5678 bas");
        string B = FormatString(TransientArena, "foo %S bar %S bas", String("1234"), String("5678"));
        AssertEqualString(A, B);
    }
    {
        string A = String("before foo after");
        string B = FormatString(TransientArena, "before %S after", String("bar"));
        AssertNotEqualString(A, B);
    }
    {
        string A = String("before foo after");
        string B = FormatString(TransientArena, "before %.3S after", String("foobar"));
        AssertEqualString(A, B);
    }
    
}

internal void
RunFormatStringPercentTests(memory_arena *TransientArena, void *Data)
{
    {
        string A = String("before % after");
        string B = FormatString(TransientArena, "before %% after");
        AssertEqualString(A, B);
    }
    {
        string A = String("before  after");
        string B = FormatString(TransientArena, "before %% after");
        AssertNotEqualString(A, B);
    }
}

internal void
RunFormatStringDateTests(memory_arena *TransientArena, void *Data)
{
    {
        date_time Date;
        Date.Year = 2020;
        Date.Month = 9;
        Date.Day = 29;
        Date.Hour = 7;
        Date.Minute = 45;
        Date.Second = 13;
        Date.Milliseconds = 0;
        string A = String("2020-09-29 07:45:13:000");
        string B = FormatString(TransientArena,
                                "%04d-%02d-%02d %02d:%02d:%02d:%03d",
                                Date.Year, Date.Month, Date.Day, Date.Hour, Date.Minute, Date.Second, Date.Milliseconds);
        AssertEqualString(A, B)
    }
    {
        date_time Date;
        Date.Year = 2020;
        Date.Month = 9;
        Date.Day = 29;
        Date.Hour = 7;
        Date.Minute = 45;
        Date.Second = 00;
        Date.Milliseconds = 123;
        string A = String("2020-09-29 07:45:00:123");
        string B = FormatString(TransientArena,
                                "%04d-%02d-%02d %02d:%02d:%02d:%03d",
                                Date.Year, Date.Month, Date.Day, Date.Hour, Date.Minute, Date.Second, Date.Milliseconds);
        AssertEqualString(A, B)
    }
    {
        date_time Date;
        Date.Year = 2020;
        Date.Month = 9;
        Date.Day = 29;
        Date.Hour = 7;
        Date.Minute = 45;
        Date.Second = 7;
        Date.Milliseconds = 4;
        string A = String("2020-09-29 07:45:07:004");
        string B = FormatString(TransientArena,
                                "%04d-%02d-%02d %02d:%02d:%02d:%03d",
                                Date.Year, Date.Month, Date.Day, Date.Hour, Date.Minute, Date.Second, Date.Milliseconds);
        AssertEqualString(A, B)
    }
    {
        date_time Date;
        Date.Year = 2020;
        Date.Month = 9;
        Date.Day = 29;
        Date.Hour = 7;
        Date.Minute = 45;
        Date.Second = 42;
        Date.Milliseconds = 42;
        string A = String("2020-09-29 07:45:42:042");
        string B = FormatString(TransientArena,
                                "%04d-%02d-%02d %02d:%02d:%02d:%03d",
                                Date.Year, Date.Month, Date.Day, Date.Hour, Date.Minute, Date.Second, Date.Milliseconds);
        AssertEqualString(A, B)
    }
}

internal void
RunFormatStringBinaryTests(memory_arena *TransientArena, void *Data)
{
    {
        string A = String("before 0b10001001 after");
        string B = FormatString(TransientArena, "before %b after", 0b10001001);
        AssertEqualString(A, B);
    }
    {
        string A = String("before 0b11110000 after");
        string B = FormatString(TransientArena, "before %b after", 0b11110000);
        AssertEqualString(A, B);
    }
    {
        string A = String("before 0b00001111 after");
        string B = FormatString(TransientArena, "before %b after", 0b00001111);
        AssertEqualString(A, B);
    }
}

internal void
RunFormatStringWithoutArenaTests(memory_arena *TransientArena, void *Data)
{
    {
        u8 Buffer[256];
        umm BufferSize = sizeof(Buffer);
        AssertEqualU32(256, BufferSize);
        
        format_string_state State = BeginFormatString();
        AppendFormatString(&State, "before %d after", 42);
        string Actual = EndFormatStringToBuffer(&State, Buffer, BufferSize);
        
        AssertEqualString(String("before 42 after"), Actual);
    }
    {
        u8 Buffer[256];
        umm BufferSize = sizeof(Buffer);
        
        string Actual = FormatStringToBuffer(Buffer, BufferSize, "before %d after", 42);
        AssertEqualString(String("before 42 after"), Actual);
    }
}

internal void
RunV2Tests(memory_arena *TransientArena, void *Data)
{
    {
        v2 A = V2(2.3f, 4.5f);
        AssertTrue(A.X == 2.3f);
        AssertTrue(A.Y == 4.5f);
    }
    {
        v2 A = V2Set1(3.14f);
        AssertTrue(A.X == 3.14f);
        AssertTrue(A.Y == 3.14f);
    }
    {
        v2 C = V2Add(V2Set1(3.0f), V2Set1(2.0f));
        AssertTrue(C.X == 5.0f);
        AssertTrue(C.Y == 5.0f);
    }
    {
        v2 C = V2Subtract(V2Set1(3.0f), V2Set1(2.0f));
        AssertTrue(C.X == 1.0f);
        AssertTrue(C.Y == 1.0f);
    }
    {
        v2 C = V2Multiply(V2Set1(3.0f), V2Set1(2.0f));
        AssertTrue(C.X == 6.0f);
        AssertTrue(C.Y == 6.0f);
    }
}

internal void
RunUpperCamelCaseTests(memory_arena *TransientArena, void *Data)
{
    {
        string Expected = String("UpperCamelCase");
        string Actual = FormatString(TransientArena, "upper_camel_case");
        ToUpperCamelCase(&Actual);
        AssertEqualString(Expected, Actual);
    }
}

typedef struct debug_clock_entry
{
    f64 Sum;
} debug_clock_entry;

// TODO(kstandbridge): Preprocessor code generation?

internal void
DebugClockEntryRadixSort(u32 Count, debug_clock_entry *First, debug_clock_entry *Temp, sort_type SortType)
{
    debug_clock_entry *Source = First;
    debug_clock_entry *Dest = Temp;
    for(u32 ByteIndex = 0;
        ByteIndex < 32;
        ByteIndex += 8)
    {
        u32 SortKeyOffsets[256] = {0};
        
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            u32 RadixValue;
            if(SortType == Sort_Descending)
            {
                RadixValue = F32ToRadixValue((f32)-Source[Index].Sum);
            }
            else
            {
                Assert(SortType == Sort_Ascending);
                RadixValue = F32ToRadixValue((f32)Source[Index].Sum);
            }
            u32 RadixPiece = (RadixValue >> ByteIndex) & 0xFF;
            ++SortKeyOffsets[RadixPiece];
        }
        
        u32 Total = 0;
        for(u32 SortKeyIndex = 0;
            SortKeyIndex < ArrayCount(SortKeyOffsets);
            ++SortKeyIndex)
        {
            u32 OffsetCount = SortKeyOffsets[SortKeyIndex];
            SortKeyOffsets[SortKeyIndex] = Total;
            Total += OffsetCount;
        }
        
        
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            u32 RadixValue;
            if(SortType == Sort_Descending)
            {
                RadixValue = F32ToRadixValue((f32)-Source[Index].Sum);
            }
            else
            {
                Assert(SortType == Sort_Ascending);
                RadixValue = F32ToRadixValue((f32)Source[Index].Sum);
            }
            u32 RadixPiece = (RadixValue >> ByteIndex) & 0xFF;
            Dest[SortKeyOffsets[RadixPiece]++] = Source[Index];
        }
        
        debug_clock_entry *SwapTemp = Dest;
        Dest = Source;
        Source = SwapTemp;
    }
}

internal void
RunRadixSortTests(memory_arena *TransientArena, void *Data)
{
    // TODO(kstandbridge): Seed random number, perhaps PlatformGetTimestamp?
    random_state RandomState = 
    {
        .Value = 1234u
    };
            
    u32 Count = RandomU32(&RandomState) % 1000;
    debug_clock_entry *Entries = PushArray(TransientArena, Count, debug_clock_entry);
    for(u32 Index = 0;
        Index < Count;
        ++Index)
    {
        debug_clock_entry *Entry = Entries + Index;
        Entry->Sum = RandomU32(&RandomState);
    }
    
    debug_clock_entry *TempEntries = PushArray(TransientArena, Count, debug_clock_entry);
    DebugClockEntryRadixSort(Count, Entries, TempEntries, Sort_Ascending);
    
    u32 LastSum = U32Min;
    for(u32 Index = 0;
        Index < Count;
        ++Index)
    {
        debug_clock_entry *Entry = Entries + Index;
        AssertTrue(Entry->Sum >= LastSum);
        LastSum = (u32)Entry->Sum;
    }
    
    DebugClockEntryRadixSort(Count, Entries, TempEntries, Sort_Descending);
    LastSum = U32Max;
    for(u32 Index = 0;
        Index < Count;
        ++Index)
    {
        debug_clock_entry *Entry = Entries + Index;
        AssertTrue(Entry->Sum <= LastSum);
        LastSum = (u32)Entry->Sum;
    }
}

internal void
RunParseFromStringTests(memory_arena *TransientArena, void *Data)
{
    {
        s32 First = 0;
        s32 Second = 0;
        ParseFromString(String("42-24"), "%d-%d", &First, &Second);
        AssertEqualU32(42, First);
        AssertEqualU32(24, Second);
    }
    {
        u64 First = 0;
        s32 Second = 0;
        ParseFromString(String("8527741705-24"), "%ld-%d", &First, &Second);
        AssertEqualU64(8527741705, First);
        AssertEqualU32(24, Second);
    }
    {
        string Input = String("27-Nov-2020 17:52  2.13 MB");
        char *Format = "%d-%S-%d %d:%d";
        s32 Day = 0;
        string Month;
        Month.Size = 0;
        Month.Data = 0;
        s32 Year = 0;
        s32 Hour = 0;
        s32 Minute = 0;
        ParseFromString(Input, Format,
                        &Day, &Month, &Year, &Hour, &Minute);
        AssertEqualU32(27, Day);
        AssertEqualString(String("Nov"), Month);
        AssertEqualU32(2020, Year);
        AssertEqualU32(17, Hour);
        AssertEqualU32(52, Minute);
    }
    
    {
        string Hostname = String("");
        u32 Port = 0;
        string Endpoint = String("");
        ParseFromString(String("http://www.whatever.org:6231/foo/bar/bas/{ignmore}.html"),
                        "http://%S:%u%S{ignore}.html", &Hostname, &Port, &Endpoint);
        AssertEqualString(String("www.whatever.org"), Hostname);
        AssertEqualU32(6231, Port);
        AssertEqualString(String("/foo/bar/bas/"), Endpoint);
    }
}

internal void
RunSha512Tests(memory_arena *TransientArena, void *Data)
{
    u8 Seed[32];
    for(u8 Index = 0;
        Index < sizeof(Seed);
        ++Index)
    {
        Seed[Index] = Index;
    }
    
    u8 Output[64];
    ZeroSize(sizeof(Output), Output);
    Sha512(Seed, sizeof(Seed), Output);
        
    AssertEqualU32(Output[0], 0x3d);
    AssertEqualU32(Output[1], 0x94);
    AssertEqualU32(Output[2], 0xee);
    AssertEqualU32(Output[3], 0xa4);
    AssertEqualU32(Output[4], 0x9c);
    AssertEqualU32(Output[5], 0x58);
    AssertEqualU32(Output[6], 0x0a);
    AssertEqualU32(Output[7], 0xef);
    AssertEqualU32(Output[8], 0x81);
    AssertEqualU32(Output[9], 0x69);
    AssertEqualU32(Output[10], 0x35);
    AssertEqualU32(Output[11], 0x76);
    AssertEqualU32(Output[12], 0x2b);
    AssertEqualU32(Output[13], 0xe0);
    AssertEqualU32(Output[14], 0x49);
    AssertEqualU32(Output[15], 0x55);
    AssertEqualU32(Output[16], 0x9d);
    AssertEqualU32(Output[17], 0x6d);
    AssertEqualU32(Output[18], 0x14);
    AssertEqualU32(Output[19], 0x40);
    AssertEqualU32(Output[20], 0xde);
    AssertEqualU32(Output[21], 0xde);
    AssertEqualU32(Output[22], 0x12);
    AssertEqualU32(Output[23], 0xe6);
    AssertEqualU32(Output[24], 0xa1);
    AssertEqualU32(Output[25], 0x25);
    AssertEqualU32(Output[26], 0xf1);
    AssertEqualU32(Output[27], 0x84);
    AssertEqualU32(Output[28], 0x1f);
    AssertEqualU32(Output[29], 0xff);
    AssertEqualU32(Output[30], 0x8e);
    AssertEqualU32(Output[31], 0x6f);

}

internal void
RunEdDSATests(memory_arena *TransientArena, void *Data)
{
    // TODO(kstandbridge): Seed random number, perhaps PlatformGetTimestamp?
    random_state RandomState = 
    {
        .Value = 1234u
    };
    
    ed25519_private_key PersistentKey;
    
    // NOTE(kstandbridge): Generate a random private key for the test
    {    
        for(u32 Index = 0;
            Index < 64;
            ++Index)
        {
            PersistentKey[Index] = RandomU32(&RandomState) % 256;
        }
    }
    
    ed25519_key_pair KeyPair = Ed25519CreateKeyPair(PersistentKey);
    
    string Message = String("some super secret message");
    ed25519_signature Signature;
    Ed25519Sign(Signature, Message, KeyPair);
    
    b32 SignVerify = Ed25519Verify(Signature, Message, KeyPair.Public);
    AssertTrue(SignVerify);
    
    
    SignVerify = Ed25519Verify(Signature, String("wrong secret message"), KeyPair.Public);
    AssertFalse(SignVerify);
}

typedef enum node_predicate_type
{
    NodePredicate_ByValue,
    NodePredicate_MatchValue,
    NodePredicate_MatchText
} node_predicate_type;

typedef struct node
{
    u32 Value;
    string Text;
    
    struct node *Next;
} node;

// TODO(kstandbridge): Preprocessor code generation

internal u32
GetNodeCount(node *Head)
{
    u32 Result = 0;
    
    while(Head != 0)
    {
        Head = Head->Next;
        ++Result;
    }
    
    return Result;
}

internal node *
GetNodeByIndex(node *Head, s32 Index)
{
    node *Result = Head;
    
    if(Result != 0)
    {
        while(Result && Index)
        {
            Result = Result->Next;
            --Index;
        }
    }
    
    return Result;
}

internal s32
GetIndexOfNode(node *Head, node *Node)
{
    s32 Result = -1;
    
    for(s32 Index = 0;
        Head;
        ++Index, Head = Head->Next)
    {
        if(Head == Node)
        {
            Result = Index;
            break;
        }
    }
    
    return Result;
}

typedef b32 node_predicate(void *Context, node *A, node *B);

internal node *
GetNode(node *Head, node_predicate *Predicate, void *Context, node *Match)
{
    node *Result = 0;
    
    while(Head)
    {
        if(Predicate(Context, Head, Match))
        {
            Result = Head;
            break;
        }
        else
        {
            Head = Head->Next;
        }
    }
    
    return Result;
}

internal node *
GetNodeTail(node *Head)
{
    node *Result = Head;
    
    if(Result != 0)
    {
        while(Result->Next != 0)
        {
            Result = Result->Next;
        }
    }
    
    return Result;
}

internal node *
PushNode(node **HeadRef, memory_arena *Arena)
{
    node *Result = PushStruct(Arena, node);
    
    Result->Next = *HeadRef;
    *HeadRef = Result;
    
    return Result;
}

internal node *
PushbackNode(node **HeadRef, memory_arena *Arena)
{
    node *Result = PushStruct(Arena, node);
    
    Result->Next = 0;
    if(*HeadRef == 0)
    {
        *HeadRef = Result;
    }
    else
    {
        node *Tail = GetNodeTail(*HeadRef);
        Tail->Next = Result;
    }
    
    return Result;
}

internal node *
NodeMergeSort_(node *Front, node *Back, node_predicate *Predicate, void *Context, sort_type SortType)
{
    node *Result = 0;
    if(Front == 0)
    {
        Result = Back;
    }
    else if(Back == 0)
    {
        Result = Front;
    }
    else
    {
        b32 PredicateResult = Predicate(Context, Front, Back);
        if(SortType == Sort_Descending)
        {
            PredicateResult = !PredicateResult;
        }
        else
        {
            Assert(SortType == Sort_Ascending);
        }
        if(PredicateResult)
        {
            Result = Front;
            Result->Next = NodeMergeSort_(Front->Next, Back, Predicate, Context, SortType);
        }
        else
        {
            Result = Back;
            Back->Next = NodeMergeSort_(Front, Back->Next, Predicate, Context, SortType);
        }
    }
    
    return Result;
}

internal void
NodeFrontBackSplit(node *Head, node **FrontRef, node **BackRef)
{
    node *Fast;
    node *Slow;
    Slow = Head;
    Fast = Head->Next;
    
    while(Fast != 0)
    {
        Fast = Fast->Next;
        if(Fast != 0)
        {
            Slow = Slow->Next;
            Fast = Fast->Next;
        }
    }
    
    *FrontRef = Head;
    *BackRef = Slow->Next;
    Slow->Next = 0;
}

internal void
NodeMergeSort(node **HeadRef, node_predicate *Predicate, void *Context, sort_type SortType)
{
    node *Head = *HeadRef;
    
    if((Head!= 0) &&
       (Head->Next != 0))
    {
        node *Front;
        node *Back;
        NodeFrontBackSplit(Head, &Front, &Back);
        
        NodeMergeSort(&Front, Predicate, Context, SortType);
        NodeMergeSort(&Back, Predicate, Context, SortType);
        
        *HeadRef = NodeMergeSort_(Front, Back, Predicate, Context, SortType);
    }
}

internal b32
NodePredicate(void *Context, node *A, node *B)
{
    b32 Result = false;
    
    switch(*(node_predicate_type *)Context)
    {
        case NodePredicate_ByValue:
        {
            Result = (A->Value < B->Value);
        } break;
        
        case NodePredicate_MatchValue:
        {
            Result = (A->Value == B->Value);
        } break;
        
        case NodePredicate_MatchText:
        {
            s32 SortIndex = StringComparisonLowercase(A->Text, B->Text);
            if(SortIndex > 0)
            {
                Result = false;
            }
            else
            {
                Result = true;
            }
        } break;
        
        InvalidDefaultCase;
    }
    return Result;
}

internal void
RunLinkedListMergeSortTests_(memory_arena *Arena)
{
    node *Head = 0;
    
    node *FortyOne = PushbackNode(&Head, Arena);
    FortyOne->Value = 41;
    FortyOne->Text = String("ddaa");
    
    node *Five = PushbackNode(&Head, Arena);
    Five->Value = 5;
    Five->Text = String("ee");
    
    node *Seven = PushbackNode(&Head, Arena);
    Seven->Value = 7;
    Seven->Text = String("gg");
    
    node *TwentyTwo = PushbackNode(&Head, Arena);
    TwentyTwo->Value = 22;
    TwentyTwo->Text = String("bbbb");
    
    node *TwentyEight = PushbackNode(&Head, Arena);
    TwentyEight->Value = 28;
    TwentyEight->Text = String("bbhh");
    
    node *SixtyThree = PushbackNode(&Head, Arena);
    SixtyThree->Value = 63;
    SixtyThree->Text = String("ffcc");
    
    node *Four = PushbackNode(&Head, Arena);
    Four->Value = 4;
    Four->Text = String("dd");
    
    node *Eight = PushbackNode(&Head, Arena);
    Eight->Value = 8;
    Eight->Text = String("hh");
    
    node *Two = PushbackNode(&Head, Arena);
    Two->Value = 2;
    Two->Text = String("bb");
    
    node *Eleven = PushbackNode(&Head, Arena);
    Eleven->Value = 11;
    Eleven->Text = String("aa");
    
    AssertTrue(Head == FortyOne);
    AssertTrue(Head->Next == Five);
    AssertTrue(Head->Next->Next == Seven);
    AssertTrue(Head->Next->Next->Next == TwentyTwo);
    AssertTrue(Head->Next->Next->Next->Next == TwentyEight);
    AssertTrue(Head->Next->Next->Next->Next->Next == SixtyThree);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next == Four);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next == Eight);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next == Two);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next->Next == Eleven);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next->Next->Next == 0);
    
    node_predicate_type PredicateType = NodePredicate_ByValue;
    
    NodeMergeSort(&Head, NodePredicate, &PredicateType, Sort_Ascending);
    
    AssertTrue(Head == Two);
    AssertTrue(Head->Next == Four);
    AssertTrue(Head->Next->Next == Five);
    AssertTrue(Head->Next->Next->Next == Seven);
    AssertTrue(Head->Next->Next->Next->Next == Eight);
    AssertTrue(Head->Next->Next->Next->Next->Next == Eleven);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next == TwentyTwo);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next == TwentyEight);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next == FortyOne);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next->Next == SixtyThree);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next->Next->Next == 0);
    
    NodeMergeSort(&Head, NodePredicate, &PredicateType, Sort_Descending);
    
    AssertTrue(Head == SixtyThree);
    AssertTrue(Head->Next == FortyOne);
    AssertTrue(Head->Next->Next == TwentyEight);
    AssertTrue(Head->Next->Next->Next == TwentyTwo);
    AssertTrue(Head->Next->Next->Next->Next == Eleven);
    AssertTrue(Head->Next->Next->Next->Next->Next == Eight);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next == Seven);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next == Five);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next == Four);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next->Next == Two);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next->Next->Next == 0);
    
    PredicateType = NodePredicate_MatchText;
    
    NodeMergeSort(&Head, NodePredicate, &PredicateType, Sort_Ascending);
    
    AssertTrue(Head == Eleven);
    AssertTrue(Head->Next == TwentyTwo);
    AssertTrue(Head->Next->Next == TwentyEight);
    AssertTrue(Head->Next->Next->Next == Two);
    AssertTrue(Head->Next->Next->Next->Next == FortyOne);
    AssertTrue(Head->Next->Next->Next->Next->Next == Four);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next == Five);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next == SixtyThree);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next == Seven);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next->Next == Eight);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next->Next->Next == 0);
    
    NodeMergeSort(&Head, NodePredicate, &PredicateType, Sort_Descending);
    
    AssertTrue(Head == Eight);
    AssertTrue(Head->Next == Seven);
    AssertTrue(Head->Next->Next == SixtyThree);
    AssertTrue(Head->Next->Next->Next == Five);
    AssertTrue(Head->Next->Next->Next->Next == Four);
    AssertTrue(Head->Next->Next->Next->Next->Next == FortyOne);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next == Two);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next == TwentyEight);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next == TwentyTwo);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next->Next == Eleven);
    AssertTrue(Head->Next->Next->Next->Next->Next->Next->Next->Next->Next->Next == 0);
    
}

internal void
RunLinkedListMergeSortTests(memory_arena *TransientArena, void *Data)
{
    RunLinkedListMergeSortTests_(TransientArena);
}

internal void
RunNodeGetCountTests(memory_arena *TransientArena, void *Data)
{
    node *Head = 0;
    
    node *FortyOne = PushbackNode(&Head, TransientArena);
    FortyOne->Value = 41;
    node *Five = PushbackNode(&Head, TransientArena);
    Five->Value = 5;
    node *Seven = PushbackNode(&Head, TransientArena);
    Seven->Value = 7;
    
    AssertTrue(Head == FortyOne);
    AssertTrue(Head->Next == Five);
    AssertTrue(Head->Next->Next == Seven);
    
    u32 Actual = GetNodeCount(Head);
    AssertEqualU32(3, Actual);
}

internal void
RunGetNodeByIndexTests(memory_arena *TransientArena, void *Data)
{
    node *Head = 0;
    
    node *FortyOne = PushbackNode(&Head, TransientArena);
    FortyOne->Value = 41;
    node *Five = PushbackNode(&Head, TransientArena);
    Five->Value = 5;
    node *Seven = PushbackNode(&Head, TransientArena);
    Seven->Value = 7;
    
    AssertTrue(Head == FortyOne);
    AssertTrue(Head->Next == Five);
    AssertTrue(Head->Next->Next == Seven);
    
    node *Actual = GetNodeByIndex(Head, 0);
    AssertTrue(Actual == Head);
    Actual = GetNodeByIndex(Head, 1);
    AssertTrue(Actual == Five);
    Actual = GetNodeByIndex(Head, 2);
    AssertTrue(Actual == Seven);
    Actual = GetNodeByIndex(Head, 3);
    AssertTrue(Actual == 0);
}

internal void
RunGetIndexOfNodeTests(memory_arena *TransientArena, void *Data)
{
    node *Head = 0;
    
    node *FortyOne = PushbackNode(&Head, TransientArena);
    FortyOne->Value = 41;
    node *Five = PushbackNode(&Head, TransientArena);
    Five->Value = 5;
    node *Seven = PushbackNode(&Head, TransientArena);
    Seven->Value = 7;
    node *DummyNode = PushStruct(TransientArena, node);
    
    AssertTrue(Head == FortyOne);
    AssertTrue(Head->Next == Five);
    AssertTrue(Head->Next->Next == Seven);
    
    s32 Actual = GetIndexOfNode(Head, FortyOne);
    AssertEqualS32(Actual, 0);
    Actual = GetIndexOfNode(Head, Five);
    AssertEqualS32(Actual, 1);
    Actual = GetIndexOfNode(Head, Seven);
    AssertEqualS32(Actual, 2);
    Actual = GetIndexOfNode(Head, DummyNode);
    AssertEqualS32(Actual, -1);
    
}

internal void
RunGetNodeTests(memory_arena *TransientArena, void *Data)
{
    node *Head = 0;
    
    node *FortyOne = PushbackNode(&Head, TransientArena);
    FortyOne->Value = 41;
    node *Five = PushbackNode(&Head, TransientArena);
    Five->Value = 5;
    node *Seven = PushbackNode(&Head, TransientArena);
    Seven->Value = 7;
    node *DummyNode = PushStruct(TransientArena, node);
    AssertTrue(Head == FortyOne);
    AssertTrue(Head->Next == Five);
    AssertTrue(Head->Next->Next == Seven);
    
    node MatchNode = 
    {
        .Value = 41
    };
    
    node_predicate_type PredicateType = NodePredicate_MatchValue;
    node *Actual = GetNode(Head, NodePredicate, &PredicateType, &MatchNode);
    AssertTrue(Actual == FortyOne);
    
    MatchNode.Value = 7;
    Actual = GetNode(Head, NodePredicate, &PredicateType, &MatchNode);
    AssertTrue(Actual == Seven);
    
    Actual = GetNode(Head, NodePredicate, &PredicateType, DummyNode);
    AssertTrue(Actual == 0);
}

internal void
RunParseHtmlTest(memory_arena *TransientArena, void *Data)
{
    string HtmlData = String("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n"
                             "<html>\n"
                             "\t<head>\n"
                             "\t\t<title>The title goes here</title>\n"
                             "\t</head>\n"
                             "\t<body>\n"
                             "\t\t<h1>The header goes here</h1>\n"
                             "\t\t<ul>\n"
                             "\t\t\t<li><a href=\"/Parent/\"> Parent Directory</a></li>\n"
                             "\t\t\t<li><a href=\"foo.img\"> foo image</a></li>\n"
                             "\t\t\t<li><a href=\"bar.txt\"> bar text</a></li>\n"
                             "\t\t\t<li><a href=\"bas.wav\"> bas soundl</a></li>\n"
                             "\t\t</ul>\n"
                             "\t</body>\n"
                             "</html>");
    
    xml_element *DocElement = ParseXmlDocument(TransientArena, HtmlData, String("DummyFile.html"));
    xml_element *HtmlElement = GetXmlElement(DocElement, String("html"));
    AssertTrue(HtmlElement);
    xml_element *BodyElement = GetXmlElement(HtmlElement, String("body"));
    AssertTrue(BodyElement);
    
    xml_element *UlElement = GetXmlElement(BodyElement, String("ul"));
    AssertTrue(UlElement);
    
    xml_element *LiElements = GetXmlElement(UlElement, String("li"));
    AssertTrue(LiElements);
    
    xml_element *LiElement = LiElements;
    xml_element *AElement = GetXmlElement(LiElement, String("a"));
    AssertTrue(AElement);
    xml_attribute *HrefAttribute = GetXmlAttribute(AElement, String("href"));
    AssertTrue(HrefAttribute);
    AssertEqualString(String("/Parent/"), HrefAttribute->Value);
    
    LiElement = LiElement->Next;
    AElement = GetXmlElement(LiElement, String("a"));
    AssertTrue(AElement);
    HrefAttribute = GetXmlAttribute(AElement, String("href"));
    AssertTrue(HrefAttribute);
    AssertEqualString(String("foo.img"), HrefAttribute->Value);
    
    LiElement = LiElement->Next;
    AElement = GetXmlElement(LiElement, String("a"));
    AssertTrue(AElement);
    HrefAttribute = GetXmlAttribute(AElement, String("href"));
    AssertTrue(HrefAttribute);
    AssertEqualString(String("bar.txt"), HrefAttribute->Value);
    
    LiElement = LiElement->Next;
    AElement = GetXmlElement(LiElement, String("a"));
    AssertTrue(AElement);
    HrefAttribute = GetXmlAttribute(AElement, String("href"));
    AssertTrue(HrefAttribute);
    AssertEqualString(String("bas.wav"), HrefAttribute->Value);
}


internal void
RunParseXmlTest(memory_arena *TransientArena, void *Data)
{
    
    {
        string FileName = String("/dev/null");
        string FileData = String("<Config>\n"
                                 "\t<Source>SomeSource</Source>\n"
                                 "\t<!--\n"
                                 "\t\tThis is a comment that should be entirely ignored\n"
                                 "\t-->\n"
                                 "\t<Name>SomeName</Name>\n"
                                 "</Config>");
        xml_element *DocElement = ParseXmlDocument(TransientArena, FileData, FileName);
        xml_element *ConfigElement = GetXmlElement(DocElement, String("Config"));
        AssertTrue(ConfigElement);
        
        xml_element *SourceElement = GetXmlElement(ConfigElement, String("Source"));
        AssertTrue(SourceElement);
        string SourceElementValue = GetXmlElementValue(SourceElement);
        AssertEqualString(String("SomeSource"), SourceElementValue);
        
        xml_element *NameElement = GetXmlElement(ConfigElement, String("Name"));
        AssertTrue(NameElement);
        string NameElementValue = GetXmlElementValue(NameElement);
        AssertEqualString(String("SomeName"), NameElementValue);
    }
    
    {
        string FileName = String("/dev/null");
        string FileData = String("<foo>"
                                 "bar"
                                 "</foo>");
        xml_element *DocElement = ParseXmlDocument(TransientArena, FileData, FileName);
        xml_element *FooElement = GetXmlElement(DocElement, String("foo"));
        AssertTrue(FooElement);
        string Value = GetXmlElementValue(FooElement);
        AssertEqualString(String("bar"), Value);
    }
    
    {
        string FileName = String("/dev/null");
        string FileData = String("<foo abc=\"123\" cba=\"321\">"
                                 "bar"
                                 "</foo>");
        xml_element *DocElement = ParseXmlDocument(TransientArena, FileData, FileName);
        xml_element *FooElement = GetXmlElement(DocElement, String("foo"));
        AssertTrue(FooElement);
        string Value = GetXmlElementValue(FooElement);
        AssertEqualString(String("bar"), Value);
        xml_attribute *CbaAttribute = GetXmlAttribute(FooElement, String("cba"));
        AssertEqualString(String("321"), CbaAttribute->Value);
        xml_attribute *AbcAttribute = GetXmlAttribute(FooElement, String("abc"));
        AssertEqualString(String("123"), AbcAttribute->Value);
    }
    
    {
        string FileName = String("/dev/null");
        string FileData = String("<foo>"
                                 "\t<bar A=\"1\" />"
                                 "\t<bar A=\"2\" />"
                                 "\t<bar A=\"3\" />"
                                 "</foo>");
        xml_element *DocElement = ParseXmlDocument(TransientArena, FileData, FileName);
        xml_element *FooElement = GetXmlElement(DocElement, String("foo"));
        AssertTrue(FooElement);
        
        xml_element *BarElement = GetXmlElement(FooElement, String("bar"));
        AssertTrue(BarElement);
        xml_attribute *AAttribute = GetXmlAttribute(BarElement, String("A"));
        AssertTrue(AAttribute);
        AssertEqualString(String("1"), AAttribute->Value);
        
        BarElement = BarElement->Next;
        AssertTrue(BarElement);
        AAttribute = GetXmlAttribute(BarElement, String("A"));
        AssertTrue(AAttribute);
        AssertEqualString(String("2"), AAttribute->Value);
        
        BarElement = BarElement->Next;
        AssertTrue(BarElement);
        AAttribute = GetXmlAttribute(BarElement, String("A"));
        AssertTrue(AAttribute);
        AssertEqualString(String("3"), AAttribute->Value);
    }
    
    {
        string FileName = String("/dev/null");
        string FileData = String("<foo>"
                                 "<bar>1</bar>"
                                 "<bar>2</bar>"
                                 "<bar>3</bar>"
                                 "</foo>");
        xml_element *DocElement = ParseXmlDocument(TransientArena, FileData, FileName);
        xml_element *FooElement = GetXmlElement(DocElement, String("foo"));
        AssertTrue(FooElement);
        
        xml_element *BarElement = GetXmlElement(FooElement, String("bar"));
        AssertTrue(BarElement);
        string Value = GetXmlElementValue(BarElement);
        AssertEqualString(String("1"), Value);
        
        BarElement = BarElement->Next;
        AssertTrue(BarElement);
        Value = GetXmlElementValue(BarElement);
        AssertEqualString(String("2"), Value);
        
        BarElement = BarElement->Next;
        AssertTrue(BarElement);
        Value = GetXmlElementValue(BarElement);
        AssertEqualString(String("3"), Value);
    }
    
    {
        string Filename = String("/dev/null");
        string FileData = String("<header>"
                                 "<first>"
                                 "<sub>1</sub>"
                                 "<sub>2</sub>"
                                 "</first>"
                                 "<second>"
                                 "<sub>3</sub>"
                                 "</second>");
        xml_element *DocElement = ParseXmlDocument(TransientArena, FileData, Filename);
        xml_element *HeaderElement = GetXmlElement(DocElement, String("header"));
        Assert(HeaderElement);
        
        xml_element *FirstElement = GetXmlElement(HeaderElement, String("first"));
        Assert(FirstElement);
        
        xml_element *SubElement = GetXmlElement(FirstElement, String("sub"));
        Assert(SubElement);
        string Value = GetXmlElementValue(SubElement);
        AssertEqualString(String("1"), Value);
        
        SubElement = SubElement->Next;
        Value = GetXmlElementValue(SubElement);
        AssertEqualString(String("2"), Value);
        
        SubElement = SubElement->Next;
        AssertTrue(SubElement == 0);
    }
    
    {
        string Filename = String("/dev/null");
        string FileData = String("<header>"
                                 "<first />"
                                 "<second>"
                                 "<sub>3</sub>"
                                 "</second>");
        xml_element *DocElement = ParseXmlDocument(TransientArena, FileData, Filename);
        xml_element *HeaderElement = GetXmlElement(DocElement, String("header"));
        Assert(HeaderElement);
        
        xml_element *FirstElement = GetXmlElement(HeaderElement, String("first"));
        Assert(FirstElement);
        
        xml_element *SubElement = GetXmlElement(FirstElement, String("sub"));
        AssertTrue(SubElement == 0);
    }
}

internal void
ParseJsonTests(memory_arena *TransientArena, void *Data)
{
    {
        string Filename = String("/dev/null");
        string FileData = String("{"
                                 "\"foo\": \"bar\""
                                 "}");

        json_element *DocElement = ParseJsonDocument(TransientArena, FileData, Filename);
        AssertEqualU32(JsonElement_Object, DocElement->Type);

        json_element *FooElement = GetJsonElement(DocElement, String("foo"));
        AssertEqualU32(JsonElement_String, FooElement->Type);
         AssertEqualString(String("bar"), FooElement->Value);
    }

    {
        string Filename = String("/dev/null");
        string FileData = String("{"
                                 "\"foo\": 42"
                                 "}");

        json_element *DocElement = ParseJsonDocument(TransientArena, FileData, Filename);
        AssertEqualU32(JsonElement_Object, DocElement->Type);

        json_element *FooElement = GetJsonElement(DocElement, String("foo"));
        AssertEqualU32(JsonElement_Number, FooElement->Type);
        AssertEqualString(String("42"), FooElement->Value);
    }
    
    {
        string Filename = String("/dev/null");
        string FileData = String("{"
                                 "  \"foo\": {"
                                        "\"bar\": \"bas\""
                                 "  }"
                                 "}");

        json_element *DocElement = ParseJsonDocument(TransientArena, FileData, Filename);
        AssertEqualU32(JsonElement_Object, DocElement->Type);

        json_element *FooElement = GetJsonElement(DocElement, String("foo"));
        AssertEqualU32(JsonElement_Object, FooElement->Type);
        
        json_element *BarElement = GetJsonElement(FooElement, String("bar"));
        AssertEqualU32(JsonElement_String, BarElement->Type);
        AssertEqualString(String("bas"), BarElement->Value);
    }
    {
        string Filename = String("/dev/null");
        string FileData = String("{"
                                 "  \"foo\": ["
                                        "{ \"key\": \"first\", \"value\": 1234 },"
                                        "{ \"key\": \"second\", \"value\": 4321 }"
                                 "  ]"
                                 "}");
        json_element *DocElement = ParseJsonDocument(TransientArena, FileData, Filename);
        AssertEqualU32(JsonElement_Object, DocElement->Type);

        json_element *FooElement = GetJsonElement(DocElement, String("foo"));
        AssertEqualU32(JsonElement_Array, FooElement->Type);

        json_element *ArrayElement = FooElement->Children;
        AssertEqualU32(JsonElement_Object, ArrayElement->Type);

        json_element *KeyElement = GetJsonElement(ArrayElement, String("key"));
        AssertEqualU32(JsonElement_String, KeyElement->Type);
        AssertEqualString(String("first"), KeyElement->Value);

        json_element *ValueElement = GetJsonElement(ArrayElement, String("value"));
        AssertEqualU32(JsonElement_Number, ValueElement->Type);
        AssertEqualString(String("1234"), ValueElement->Value);

        ArrayElement = ArrayElement->Next;
        AssertEqualU32(JsonElement_Object, ArrayElement->Type);

        KeyElement = GetJsonElement(ArrayElement, String("key"));
        AssertEqualU32(JsonElement_String, KeyElement->Type);
        AssertEqualString(String("second"), KeyElement->Value);

        ValueElement = GetJsonElement(ArrayElement, String("value"));
        AssertEqualU32(JsonElement_Number, ValueElement->Type);
        AssertEqualString(String("4321"), ValueElement->Value);
    }
    {
        string Filename = String("/dev/null");
        string FileData = String("{"
                                    "\"pairs\": ["
                                        "{ \"x0\": -122.16404366051066, \"y0\": 32.11899076424161, \"x1\": -108.68219357802062, \"y1\": 43.988229505979504 },"
                                    "]"
                                 "}");
        json_element *DocElement = ParseJsonDocument(TransientArena, FileData, Filename);
        AssertEqualU32(JsonElement_Object, DocElement->Type);
    
        json_element *PairsElement = GetJsonElement(DocElement, String("pairs"));
        AssertEqualU32(JsonElement_Array, PairsElement->Type);

        json_element *ArrayElement = PairsElement->Children;
        AssertEqualU32(JsonElement_Object, ArrayElement->Type);

        json_element *X0Element = GetJsonElement(ArrayElement, String("x0"));
        AssertEqualU32(JsonElement_Number, X0Element->Type);
        AssertEqualString(String("-122.16404366051066"), X0Element->Value);

        json_element *Y0Element = GetJsonElement(ArrayElement, String("y0"));
        AssertEqualU32(JsonElement_Number, Y0Element->Type);
        AssertEqualString(String("32.11899076424161"), Y0Element->Value);

        json_element *X1Element = GetJsonElement(ArrayElement, String("x1"));
        AssertEqualU32(JsonElement_Number, X1Element->Type);
        AssertEqualString(String("-108.68219357802062"), X1Element->Value);

        json_element *Y1Element = GetJsonElement(ArrayElement, String("y1"));
        AssertEqualU32(JsonElement_Number, Y1Element->Type);
        AssertEqualString(String("43.988229505979504"), Y1Element->Value);
    }
}

internal void
RunF32FromStringTests(memory_arena *TransientArena, void *Data)
{
    {
        f32 Expected = -3.124f;
        f32 Actual = F32FromString(String("-3.124"));
        AssertEqualF32(Expected, Actual);
    }
    {
        f32 Expected = 3.124f;
        f32 Actual = F32FromString(String("3.124"));
        AssertEqualF32(Expected, Actual);
    }
}
