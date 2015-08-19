

#include "windows.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/TokenCode.h"
#include "include/DefineFunction.h"
#include "include/DynString.h"
#include "include/DynArray.h"
#include "include/TkWord.h"
#include "include/color_token.h"
void color_token(int lex_state){
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    char *p;
    switch(lex_state){
        case LEX_NORMAL:{
            if(token >= TK_IDENT)
                SetConsoleTextAttribute(h,FOREGROUND_INTENSITY);
            else if(token >= KW_CHAR)
                SetConsoleTextAttribute(h,FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            else if(token >= TK_CINT)
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
