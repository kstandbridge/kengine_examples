internal point *
ParseJsonPoints(memory_arena *Arena, string Json)
{
    TIMED_FUNCTION();

    point *Result = 0;

    tokenizer Tokenizer_ = Tokenize(Json, String(""));
    tokenizer *Tokenizer = &Tokenizer_;

    point *CurrentPoint = 0;
    RequireToken(Tokenizer, Token_OpenCurlyBracket);
    while(Parsing(Tokenizer))
    {
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
    }

    return Result;
}