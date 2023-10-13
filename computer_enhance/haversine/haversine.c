internal point *
ParseJsonPoints(memory_arena *Arena, string Json, string FileName)
{
    point *Result = 0;

    json_element *DocElement = ParseJsonDocument(Arena, Json, FileName);
    if(DocElement)
    {
        json_element *PairsElement = GetJsonElement(DocElement, String("pairs"));
        if(PairsElement)
        {
            if(JsonElement_Array == PairsElement->Type)
            {
                json_element *PairElement = PairsElement->Children;
                
                point *CurrentPoint = 0;
                while(PairElement)
                {
                    json_element *X0Element = GetJsonElement(PairElement, String("x0"));
                    json_element *Y0Element = GetJsonElement(PairElement, String("y0"));
                    json_element *X1Element = GetJsonElement(PairElement, String("x1"));
                    json_element *Y1Element = GetJsonElement(PairElement, String("y1"));
                    if(X0Element && Y0Element && X1Element && Y1Element)
                    {
                        if(CurrentPoint == 0)
                        {
                            CurrentPoint = Result = PushStruct(Arena, point);
                        }
                        else
                        {
                            CurrentPoint->Next = PushStruct(Arena, point);
                            CurrentPoint = CurrentPoint->Next;
                        }
                            CurrentPoint->X0 = F64FromString(X0Element->Value);
                            CurrentPoint->Y0 = F64FromString(Y0Element->Value);
                            CurrentPoint->X1 = F64FromString(X1Element->Value);
                            CurrentPoint->Y1 = F64FromString(Y1Element->Value);
                    }
                    PairElement = PairElement->Next;
                }
            }
        }
    }

    return Result;
}

internal point *
ParseJsonPoints_(memory_arena *Arena, string Json)
{
    BEGIN_TIMED_FUNCTION();

    point *Result = 0;

    tokenizer Tokenizer_ = Tokenize(Json, String(""));
    tokenizer *Tokenizer = &Tokenizer_;

    point *CurrentPoint = 0;
    RequireToken(Tokenizer, Token_OpenCurlyBracket);
    while(Parsing(Tokenizer))
    {
        BEGIN_TIMED_BLOCK(ParseToken);

        token Token = GetToken(Tokenizer);
        if(Token.Type == Token_OpenCurlyBracket)
        {
            if(CurrentPoint == 0)
            {
                Result = CurrentPoint = PushStruct(Arena, point);
            }
            else
            {
                CurrentPoint->Next = PushStruct(Arena, point);
                CurrentPoint = CurrentPoint->Next;
            }
        }
        else if(Token.Type == Token_String)
        {
            if(!StringsAreEqual(Token.Text, String("pairs")))
            {
                RequireToken(Tokenizer, Token_Colon);
                token ValueToken = RequireToken(Tokenizer, Token_Number);
                if(Parsing(Tokenizer))
                {
                    if(StringsAreEqual(Token.Text, String("x0")))
                    {
                        CurrentPoint->X0 = F64FromString(ValueToken.Text);
                    }
                    else if(StringsAreEqual(Token.Text, String("y0")))
                    {
                        CurrentPoint->Y0 = F64FromString(ValueToken.Text);
                    }
                    else if(StringsAreEqual(Token.Text, String("x1")))
                    {
                        CurrentPoint->X1 = F64FromString(ValueToken.Text);
                    }
                    else if(StringsAreEqual(Token.Text, String("y1")))
                    {
                        CurrentPoint->Y1 = F64FromString(ValueToken.Text);
                    }
                }
            }
        }

        END_TIMED_BLOCK(ParseToken);
    }

    return Result;
}