#define KENGINE_WIN32
#define KENGINE_PREPROCESSOR
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

inline void
GenerateCtor(c_struct Struct)
{
    PlatformConsoleOut("\n#define %S(", Struct.Name);
    
    b32 First = true;
    for(c_member *Member = Struct.Members;
        Member;
        Member = Member->Next)
    {
        if(!First)
        {
            PlatformConsoleOut(", ");
        }
        PlatformConsoleOut("%S", Member->Name);
        First = false;
    }
    
    PlatformConsoleOut(") (%S){", Struct.Type);
    First = true;
    for(c_member *Member = Struct.Members;
        Member;
        Member = Member->Next)
    {
        if(!First)
        {
            PlatformConsoleOut(", ");
        }
        PlatformConsoleOut("(%S)(%S)", Member->TypeName, Member->Name);
        First = false;
    }
    PlatformConsoleOut("}");
}

inline void
GenerateSet1(c_struct Struct)
{
    PlatformConsoleOut("\n#define %SSet1(Value) (%S){", Struct.Name, Struct.Type);
    
    b32 First = true;
    for(c_member *Member = Struct.Members;
        Member;
        Member = Member->Next)
    {
        if(!First)
        {
            PlatformConsoleOut(", ");
        }
        PlatformConsoleOut("(%S)(Value)", Member->TypeName);
        First = false;
    }
    PlatformConsoleOut("}");
}

inline void
GenerateMath_(c_struct Struct, string Name, string Op)
{
    PlatformConsoleOut("\n#define %S%S(A, B) (%S){", Struct.Name, Name, Struct.Type);
    
    b32 First = true;
    for(c_member *Member = Struct.Members;
        Member;
        Member = Member->Next)
    {
        if(!First)
        {
            PlatformConsoleOut(", ");
        }
        PlatformConsoleOut("(A.%S %S B.%S)", Member->Name, Op, Member->Name);
        First = false;
    }
    PlatformConsoleOut("}");
}


inline void
GenerateMath(c_struct Struct)
{
    GenerateMath_(Struct, String("Add"), String("+"));
    GenerateMath_(Struct, String("Subtract"), String("-"));
    GenerateMath_(Struct, String("Multiply"), String("*"));
}

void
GenerateCodeFor(c_struct Struct, string_list *Options)
{
    for(string_list *Option = Options;
        Option;
        Option = Option->Next)
    {
        if(StringsAreEqual(String("ctor"), Option->Entry))
        {
            GenerateCtor(Struct);
        }
        else if(StringsAreEqual(String("set1"), Option->Entry))
        {
            GenerateSet1(Struct);
        }
        else if(StringsAreEqual(String("math"), Option->Entry))
        {
            GenerateMath(Struct);
        }
        else if(StringsAreEqual(String("ToJson"), Option->Entry))
        {
            PlatformConsoleOut("\ninline string\n");
            PlatformConsoleOut("%SToJson(memory_arena *Arena, %S A)\n", Struct.Name, Struct.Type);
            PlatformConsoleOut("{\n");
            PlatformConsoleOut("\tformat_string_state StringState = BeginFormatString();\n\n");
            PlatformConsoleOut("\tAppendFormatString(&StringState, \"{\\n\");\n");
            b32 First = true;
            for(c_member *Member = Struct.Members;
                Member;
                Member = Member->Next)
            {
                if(!First)
                {
                    PlatformConsoleOut("\tAppendFormatString(&StringState, \",\\n\");\n");
                }
                PlatformConsoleOut("\tAppendFormatString(&StringState, \"\\t%S\": ", Member->Name);
                
                switch(Member->Type)
                {
                    case C_U32:
                    {
                        PlatformConsoleOut("\"%%u\", A.%S%S);\n", Member->Name);
                    } break;
                    
                    case C_String:
                    {
                        PlatformConsoleOut("\"%%S\", A.%S%S);\n", Member->Name);
                    } break;
                    
                    default:
                    {
                        PlatformConsoleOut("\n#error Unsupported type \"%S\" for member \"%S\"\n", GetCTypeName(Member->Type), Member->Name);
                    } break;
                }
                
                First = false;
            }
            PlatformConsoleOut("\tAppendFormatString(&StringState, \"\\n}\");\n\n");
            PlatformConsoleOut("\tstring Result = EndFormatString(&StringState, Arena);\n");
            PlatformConsoleOut("\treturn Result;\n");
            
            PlatformConsoleOut("}\n");
            
        }
        else
        {
            PlatformConsoleOut("\n// Skipping unsupported option \"%S\" for type \"%S\"", Option->Entry, Struct.Type);
        }
    }
    
    PlatformConsoleOut("\n");
}