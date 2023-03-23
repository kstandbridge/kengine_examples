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

inline void
GenerateLinkedList(c_struct Struct)
{
    string Name = Struct.Name;
    string Type = Struct.Type;
    
    PlatformConsoleOut("\ninline u32\nGet%SCount(%S *Head)\n", Name, Type);
    PlatformConsoleOut("{\n");
    PlatformConsoleOut("    u32 Result = 0;\n\n");
    PlatformConsoleOut("    while(Head != 0)\n");
    PlatformConsoleOut("    {\n");
    PlatformConsoleOut("        Head = Head->Next;\n");
    PlatformConsoleOut("        ++Result;\n");
    PlatformConsoleOut("    }\n\n");
    PlatformConsoleOut("    return Result;\n");
    PlatformConsoleOut("}\n");
    
    
    PlatformConsoleOut("\ninline %S *\nGet%SByIndex(%S *Head, s32 Index)\n", Type, Name, Type);
    PlatformConsoleOut("{\n");
    PlatformConsoleOut("        %S *Result = Head;\n\n", Type);
    PlatformConsoleOut("        if(Result != 0)\n");
    PlatformConsoleOut("        {\n");
    PlatformConsoleOut("            while(Result && Index)\n");
    PlatformConsoleOut("            {\n");
    PlatformConsoleOut("                Result = Result->Next;\n");
    PlatformConsoleOut("            --Index;\n");
    PlatformConsoleOut("            }\n");
    PlatformConsoleOut("        }\n\n");
    PlatformConsoleOut("    return Result;\n");
    PlatformConsoleOut("}\n");
    
    PlatformConsoleOut("\ninline s32\nGetIndexOf%S(%S *Head, %S *%S)\n", Name, Type, Type, Name);
    PlatformConsoleOut("{\n");
    PlatformConsoleOut("    s32 Result = -1;\n\n");
    PlatformConsoleOut("    for(s32 Index = 0;\n");
    PlatformConsoleOut("        Head;\n");
    PlatformConsoleOut("        ++Index, Head = Head->Next)\n");
    PlatformConsoleOut("        {\n");
    PlatformConsoleOut("        if(Head == %S)\n", Name);
    PlatformConsoleOut("        {\n");
    PlatformConsoleOut("            Result = Index;\n");
    PlatformConsoleOut("            break;\n");
    PlatformConsoleOut("        }\n");
    PlatformConsoleOut("    }\n\n");
    PlatformConsoleOut("    return Result;\n");
    PlatformConsoleOut("}\n");
    
    PlatformConsoleOut("\ntypedef b32 %S_predicate(void *Context, %S *A, %S *B);\n", Type, Type, Type);
    
    PlatformConsoleOut("\ninline %S *\nGet%S(%S *Head, %S_predicate *Predicate, void *Context, %S *Match)\n", Type, Name, Type, Type, Type);
    PlatformConsoleOut("{\n");
    PlatformConsoleOut("    %S *Result = 0;\n\n", Type);
    PlatformConsoleOut("    while(Head)\n");
    PlatformConsoleOut("    {\n");
    PlatformConsoleOut("        if(Predicate(Context, Head, Match))\n");
    PlatformConsoleOut("        {\n");
    PlatformConsoleOut("            Result = Head;\n");
    PlatformConsoleOut("            break;\n");
    PlatformConsoleOut("        }\n");
    PlatformConsoleOut("        else\n");
    PlatformConsoleOut("        {\n");
    PlatformConsoleOut("            Head = Head->Next;\n");
    PlatformConsoleOut("        }\n");
    PlatformConsoleOut("    }\n\n");
    PlatformConsoleOut("    return Result;\n");
    PlatformConsoleOut("}\n");
    
    PlatformConsoleOut("\ninline %S *\nGet%STail(%S *Head)\n", Type, Name, Type);
    PlatformConsoleOut("{\n");
    PlatformConsoleOut("    %S *Result = Head;\n\n", Type);
    PlatformConsoleOut("    if(Result != 0)\n");
    PlatformConsoleOut("    {\n");
    PlatformConsoleOut("        while(Result->Next != 0)\n");
    PlatformConsoleOut("        {\n");
    PlatformConsoleOut("            Result = Result->Next;\n");
    PlatformConsoleOut("        }\n");
    PlatformConsoleOut("    }\n\n");
    PlatformConsoleOut("    return Result;\n");
    PlatformConsoleOut("}\n");
    
    PlatformConsoleOut("\ninline %S *\nPush%S(%S **HeadRef, memory_arena *Arena)\n", Type, Name, Type);
    PlatformConsoleOut("{\n");
    PlatformConsoleOut("    %S *Result = PushStruct(Arena, %S);\n\n", Type, Type);
    PlatformConsoleOut("    Result->Next = *HeadRef;\n");
    PlatformConsoleOut("    *HeadRef = Result;\n\n");
    PlatformConsoleOut("    return Result;\n");
    PlatformConsoleOut("}\n");
    
    PlatformConsoleOut("\ninline %S *\nPushback%S(%S **HeadRef, memory_arena *Arena)\n", Type, Name, Type);
    PlatformConsoleOut("{\n");
    PlatformConsoleOut("    %S *Result = PushStruct(Arena, %S);\n\n", Type, Type);
    PlatformConsoleOut("    Result->Next = 0;\n");
    PlatformConsoleOut("    if(*HeadRef == 0)\n");
    PlatformConsoleOut("    {\n");
    PlatformConsoleOut("        *HeadRef = Result;\n");
    PlatformConsoleOut("    }\n");
    PlatformConsoleOut("    else\n");
    PlatformConsoleOut("    {\n");
    PlatformConsoleOut("        %S *Tail = Get%STail(*HeadRef);\n", Type, Name);
    PlatformConsoleOut("        Tail->Next = Result;\n");
    PlatformConsoleOut("    }\n\n");
    PlatformConsoleOut("    return Result;\n");
    PlatformConsoleOut("}\n");
    
    PlatformConsoleOut("\ninline %S *\n%SMergeSort_(%S *Front, %S *Back, %S_predicate *Predicate, void *Context, sort_type SortType)\n", 
                       Type, Name, Type, Type, Type);
    PlatformConsoleOut("{\n");
    PlatformConsoleOut("    %S *Result = 0;\n", Type);
    PlatformConsoleOut("    if(Front == 0)\n");
    PlatformConsoleOut("    {\n");
    PlatformConsoleOut("        Result = Back;\n");
    PlatformConsoleOut("    }\n");
    PlatformConsoleOut("    else if(Back == 0)\n");
    PlatformConsoleOut("    {\n");
    PlatformConsoleOut("        Result = Front;\n");
    PlatformConsoleOut("    }\n");
    PlatformConsoleOut("    else\n");
    PlatformConsoleOut("    {\n");
    PlatformConsoleOut("        b32 PredicateResult = Predicate(Context, Front, Back);\n");
    PlatformConsoleOut("        if(SortType == Sort_Descending)\n");
    PlatformConsoleOut("        {\n");
    PlatformConsoleOut("            PredicateResult = !PredicateResult;\n");
    PlatformConsoleOut("        }\n");
    PlatformConsoleOut("        else\n");
    PlatformConsoleOut("        {\n");
    PlatformConsoleOut("            Assert(SortType == Sort_Ascending);\n");
    PlatformConsoleOut("        }\n");
    PlatformConsoleOut("        if(PredicateResult)\n");
    PlatformConsoleOut("        {\n");
    PlatformConsoleOut("            Result = Front;\n");
    PlatformConsoleOut("            Result->Next = %SMergeSort_(Front->Next, Back, Predicate, Context, SortType);\n", Name);
    PlatformConsoleOut("        }\n");
    PlatformConsoleOut("        else\n");
    PlatformConsoleOut("        {\n");
    PlatformConsoleOut("            Result = Back;\n");
    PlatformConsoleOut("            Back->Next = %SMergeSort_(Front, Back->Next, Predicate, Context, SortType);\n", Name);
    PlatformConsoleOut("        }\n");
    PlatformConsoleOut("    }\n\n");
    PlatformConsoleOut("    return Result;\n");
    PlatformConsoleOut("}\n");
    
    
    PlatformConsoleOut("\ninline void\n%SFrontBackSplit(%S *Head, %S **FrontRef, %S **BackRef)\n", Name, Type, Type);
    PlatformConsoleOut("{\n");
    PlatformConsoleOut("    %S *Fast;\n", Type);
    PlatformConsoleOut("    %S *Slow;\n", Type);
    PlatformConsoleOut("    Slow = Head;\n");
    PlatformConsoleOut("    Fast = Head->Next;\n\n");
    PlatformConsoleOut("    while(Fast != 0)\n");
    PlatformConsoleOut("    {\n");
    PlatformConsoleOut("        Fast = Fast->Next;\n");
    PlatformConsoleOut("        if(Fast != 0)\n");
    PlatformConsoleOut("        {\n");
    PlatformConsoleOut("            Slow = Slow->Next;\n");
    PlatformConsoleOut("            Fast = Fast->Next;\n");
    PlatformConsoleOut("        }\n");
    PlatformConsoleOut("    }\n\n");
    PlatformConsoleOut("    *FrontRef = Head;\n");
    PlatformConsoleOut("    *BackRef = Slow->Next;\n");
    PlatformConsoleOut("    Slow->Next = 0;\n");
    PlatformConsoleOut("}\n");
    
    
    PlatformConsoleOut("\ninline void\n%SMergeSort(%S **HeadRef, %S_predicate *Predicate, void *Context, sort_type SortType)\n", Name, Type, Type);
    PlatformConsoleOut("{\n");
    PlatformConsoleOut("    %S *Head = *HeadRef;\n\n", Type);
    PlatformConsoleOut("    if((Head!= 0) &&\n");
    PlatformConsoleOut("       (Head->Next != 0))\n");
    PlatformConsoleOut("    {\n");
    PlatformConsoleOut("        %S *Front;\n", Type);
    PlatformConsoleOut("        %S *Back;\n", Type);
    PlatformConsoleOut("        %SFrontBackSplit(Head, &Front, &Back);\n\n", Name);
    PlatformConsoleOut("        %SMergeSort(&Front, Predicate, Context, SortType);\n", Name);
    PlatformConsoleOut("        %SMergeSort(&Back, Predicate, Context, SortType);\n\n", Name);
    PlatformConsoleOut("        *HeadRef = %SMergeSort_(Front, Back, Predicate, Context, SortType);\n", Name);
    PlatformConsoleOut("    }\n");
    PlatformConsoleOut("}\n");
    
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
        else if(StringsAreEqual(String("linked_list"), Option->Entry))
        {
            GenerateLinkedList(Struct);
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