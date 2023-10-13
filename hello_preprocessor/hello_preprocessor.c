#define _GNU_SOURCE

#define KENGINE_PREPROCESSOR
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

void
GenerateCodeFor(memory_arena *Arena, c_struct Struct, string_list *Options)
{
    for(string_list *Option = Options;
        Option;
        Option = Option->Next)
    {
        if(StringsAreEqual(String("ToJson"), Option->Entry))
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
                PlatformConsoleOut("\tAppendFormatString(&StringState, \t\"%S\": ", Member->Name);
                
                switch(Member->Type)
                {
                    case C_U32:
                    {
                        PlatformConsoleOut("\"%%u\", A.%S);\n", Member->Name);
                    } break;
                    
                    case C_String:
                    {
                        PlatformConsoleOut("\"%%S\", A.%S);\n", Member->Name);
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