#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/TokenCode.h"
#include "include/DynString.h"
#include "include/DynArray.h"
#include "include/TkWord.h"
/** read char from file */
void getch(){
    ch = getc(fin);
}

void skip_white_space(){
    while(ch==' ' || ch == '\t' || ch == '\r'){
        if(ch=='\r'){
            getch();
            if(ch != '\n')
                return;
            line_num++;
        }
        printf("%c",ch);
        getch();
    }

}

void parse_comment(){
    getch();
    do{
        do{
            if(ch == '\n' || ch == '*' || ch == EOF)
                break;
            else
                getch();
        }while(1);
        if(ch == '\n'){
            line_num++;
            getch();
        }else if(ch == '*'){
            getch();
            if(ch == '/'){
                getch();
                return;
            }
        }else{
            error("not find match comment symbol until end of file");
            return;
        }
    }while(1);
}

int is_nodigit(char c){
    return (c>='a' && c<='z')||
        (c>='A' && c<='Z')||
        c=='_';
}

int is_digit(char c){
    return c>='0' && c<='9';
}

void parse_identifier(){
    dynstring_reset(&tkstr);
    dynstring_chcat(&tkstr,ch);
    getch();
    while(is_nodigit(ch) || is_digit(ch)){
        dynstring_chcat(&tkstr,ch);
        getch();
    }
    dynstring_chcat(&tkstr,'\0');
}

void parse_num(){
    dynstring_reset(&tkstr);
    dynstring_reset(&sourcestr);
    do{
        dynstring_chcat(&tkstr,ch);
        dynstring_chcat(&sourcestr,ch);
        getch();
    }while(is_digit(ch));

    if(ch == '.'){
        do{
            dynstring_chcat(&tkstr,ch);
            dynstring_chcat(&sourcestr,ch);
            getch();
        }while(is_digit(ch));
    }
    dynstring_chcat(&tkstr,'\0');
    dynstring_chcat(&sourcestr,'\0');
    tkvalue = atoi(tkstr.data);
}

/**
sep:字符的常量界限符为单引号(')
    字符串常量界限符为双引号(")
*/
void parse_string(char sep){
    char c;
    dynstring_reset(&tkstr);
    dynstring_reset(&sourcestr);
    dynstring_chcat(&sourcestr,sep);
    getch();
    while(1){
        if(ch == sep)
            break;
        else if(ch == '\\'){
                    ///parse Escape character
            dynstring_chcat(&sourcestr,ch);
            getch();
            switch(ch){
                case '0':
                    c = '\0';
                    break;
                case 'a':
                    c = '\a';
                    break;
                case 'b':
                    c = '\b';
                    break;
                case 't':
                    c = '\t';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'v':
                    c = '\v';
                    break;
                case 'f':
                    c = '\f';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case '\"':
                    c = '\"';
                    break;
                case '\'':
                    c = '\'';
                    break;
                case '\\':
                    c = '\\';
                    break;
                default:
                    c = ch;
                    if(c >= '!' && c <= '~')
                        warning("illegal character: \'\\%c\'",c);
                    else
                        warning("illegal character: \'\\0x%x\'",c);
                    break;
            }
            dynstring_chcat(&tkstr,c);
            dynstring_chcat(&sourcestr,ch);
            getch();
        }else{
            dynstring_chcat(&tkstr,ch);
            dynstring_chcat(&sourcestr,ch);
            getch();
        }


            dynstring_chcat(&tkstr,'\0');
            dynstring_chcat(&sourcestr,sep);
            dynstring_chcat(&sourcestr,'\0');
            getch();



    }













}


/** ignore tab space '\n comment */
void preprocess(){
    while(1){
        if(ch == ' ' || ch == '\t' || ch == '\r')
            skip_white_space();
        else if(ch == '/'){
                        ///向前多看一个字符来判断是否为注释，若不是在放回缓冲区中
            getch();
            if(ch=='*')
                parse_comment();
            else{
                ungetc(ch,fin);///rollback char
                ch = '/';
                break;
            }
        }else
            break;
    }
}



/**get word*/
void get_token(){
    preprocess();
    switch(ch){
        case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':
        case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':case 'n':
        case 'o':case 'p':case 'q':case 'r':case 's':case 't':
        case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':
        case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':
        case 'H':case 'I':case 'J':case 'k':case 'L':case 'M':case 'N':
        case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':
        case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z':
        case '_':
            TkWord * tp;
            parse_identifier();
            tp = tkword_insert(tkstr.data);
            token = tp->tkcode;
            break;

        case '0':case '1':case '2':case '3':case '4':
        case '5':case '6':case '7':case '8':case '9':
            parse_num();
            token = TK_CINT;
            break;

        case '+':
            getch();
            token = TK_PLUS;
            break;

        case '-':
            getch();
            if(ch == '>')
                token = TK_POINTSTO;
            else
                token = TK_MINUS;
            break;

        case '/':
            token = TK_DIVIDE;
            getch();
            break;

        case '%':
            token = TK_MOD;
            getch();
            break;
        case '=':
            getch();
            if(ch == '='){
                token = TK_EQ;
                getch();
            }else
                token = TK_ASSIGN;
            break;

        case '!':
            getch();
            if(ch=='='){
                token = TK_NEQ;
                getch();
            }else
                error("don't support '!'(not operator symbol)");
            break;

        case '<':
            getch();
            if(ch=='='){
                token = TK_LEQ;
                getch();
            }else
                token = TK_LT;
            break;

        case '>':
            getch();
            if(ch=='='){
                token = TK_GEQ;
                getch();
            }else
                token = TK_GT;
            break;

        case '.':
            getch();
            if(ch=='.'){
                getch();
                if(ch != '.')
                    error("Ellipsis spell wrong");
                else
                    token = TK_ELLIPSIS;
                getch();
            }else
                token = TK_DOT;
            break;

        case '&':
            token = TK_AND;
            getch();
            break;

        case ';':
            token = TK_SEMICOLON;
            getch();
            break;

        case ']':
            token = TK_CLOSEBR;
            getch();
            break;

        case '}':
            token = TK_END;
            getch();
            break;

        case ')':
            token = TK_CLOSEPA;
            getch();
            break;

        case '[':
            token = TK_OPENBR;
            getch();
            break;

        case '{':
            token = TK_BEGIN;
            getch();
            break;

        case ',':
            token = TK_COMMA;
            getch();
            break;

        case '(':
            token = TK_OPENPA;
            getch();
            break;

        case '*':
            token = TK_STAR;
            getch();
            break;

        case '\'':
            parse_string(ch);
            token = TK_CCHAR;
            tkvalue = *(char *)tkstr.data;
            break;

        case '\"':
            parse_string(ch);
            token = TK_CSTR;
            break;

        case EOF:
            token = TK_EOF;
            break;
        default:
            error("the char can't be parse: \\x%02x",ch);
            getch();
            break;
    }
}
