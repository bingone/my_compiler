#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/definemaro.h"
#include "include/TokenCode.h"
#include "include/DynString.h"
#include "include/DynArray.h"
#include "include/TkWord.h"
void init_lex(){
    TkWord * tp;
    static TkWord keywords[] = {
        {TK_PLUS,       NULL,   "+",            NULL,   NULL},
        {TK_MINUS,      NULL,   "-",            NULL,   NULL},
        {TK_STAR,       NULL,   "*",            NULL,   NULL},
        {TK_DIVIDE,     NULL,   "/",            NULL,   NULL},
        {TK_MOD,        NULL,   "%",            NULL,   NULL},
        {TK_EQ,         NULL,   "==",           NULL,   NULL},
        {TK_NEQ,        NULL,   "!=",           NULL,   NULL},
        {TK_LT,         NULL,   "<",            NULL,   NULL},
        {TK_LEQ,        NULL,   "<=",           NULL,   NULL},
        {TK_GT,         NULL,   ">",            NULL,   NULL},
        {TK_GEQ,        NULL,   ">=",           NULL,   NULL},
        {TK_ASSIGN,     NULL,   "=",            NULL,   NULL},
        {TK_POINTSTO,   NULL,   "->",           NULL,   NULL},
        {TK_DOT,        NULL,   ".",            NULL,   NULL},
        {TK_AND,        NULL,   "&",            NULL,   NULL},
        {TK_OPENPA,     NULL,   "(",            NULL,   NULL},
        {TK_CLOSEPA,    NULL,   ")",            NULL,   NULL},
        {TK_OPENBR,     NULL,   "[",            NULL,   NULL},
        {TK_CLOSEBR,    NULL,   "]",            NULL,   NULL},
        {TK_BEGIN,      NULL,   "{",            NULL,   NULL},
        {TK_END,        NULL,   "}",            NULL,   NULL},
        {TK_SEMICOLON,  NULL,   ";",            NULL,   NULL},
        {TK_COMMA,      NULL,   ",",            NULL,   NULL},
        {TK_ELLIPSIS,   NULL,   "...",          NULL,   NULL},
        {TK_EOF,        NULL,   "End_Of_File",  NULL,   NULL},

        /** 常量*/
        {TK_CINT,       NULL,   "cint",         NULL,   NULL},
        {TK_CCHAR,      NULL,   "cchar",        NULL,   NULL},
        {TK_CSTR,       NULL,   "cstr",         NULL,   NULL},

        /**关键字*/
        {KW_CHAR,       NULL,   ",",            NULL,   NULL},
        {KW_SHORT,      NULL,   ",",            NULL,   NULL},
        {KW_INT,        NULL,   ",",            NULL,   NULL},
        {KW_VOID,       NULL,   ",",            NULL,   NULL},
        {KW_STRUCT,     NULL,   ",",            NULL,   NULL},
        {KW_IF,         NULL,   ",",            NULL,   NULL},
        {KW_ELSE,       NULL,   ",",            NULL,   NULL},
        {KW_FOR,        NULL,   ",",            NULL,   NULL},
        {KW_CONTINUE,   NULL,   ",",            NULL,   NULL},
        {KW_BREAK,      NULL,   ",",            NULL,   NULL},
        {KW_RETURN,     NULL,   ",",            NULL,   NULL},

        {KW_ALIGN,      NULL,   "__align",      NULL,   NULL},
        {KW_CODEL,      NULL,   " __codel",     NULL,   NULL},
        {KW_STDCALL,    NULL,   " __stdall",    NULL,   NULL},

        /**标识符*/
        {0,             NULL,   NULL,           NULL,   NULL}

    };
    dynarray_init(&tktable,8);
    for(tp = &keywords[0];tp->spelling != NULL;tp++)
        tkword_direct_insert(tp);

}
