enum e_LexState{
    LEX_NORMAL,
    LEX_SEP,
};

void color_token(int lex_state){
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    char *p;
    switch(lex_state){
        case LEX_NORMAL:{
            if(token >= TK_IDENT)
                SetConsoleTextAttribute(h,FOREGROUND_INTENSITY);
            else if(token >= KW_CHAR)
                SetConsoleTextAttribute(h,FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            else if(token >= KW_CINT)
                SetConsoleTextAttribute(h,FOREGROUND_GREEN | FOREGROUND_RED);
            else
                SetConsoleTextAttribute(h,FOREGROUND_RED | FOREGROUND_INTENSITY);
            p = get_tkstr(token);
            printf("%s\n",p);
            break;

        case LEX_SEP:
            printf("%c",ch);
            break;
        }

    }
}
