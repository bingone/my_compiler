
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <STDARG.H>

#include "include/definemaro.h"
#include "include/DynString.h"
#include "include/DynArray.h"
#include "include/TkWord.h"
#include "include/TokenCode.h"
#include "include/exception_handler.h"

/**
exception hadler
stage: compiler | link
level:
fmt: output format
ap:  arguement list

*/
void handle_exception(int stage,int level,char * fmt,va_list ap){
    char buf[1024];
    vsprintf(buf,fmt,ap);
    if(STAGE_COMPILE == stage){
        if(LEVEL_WARNING == level)
            printf("%s(in %d lines):compile warning:%s!\n",fin,line_num,buf);
        else{
            printf("%s(in %d lines):compile error:%s!\n",fin,line_num,buf);
            exit(-1);
        }
    }else{
        printf("linking error %s!\n",buf);
        exit(-1);
    }
}

void warning(char * fmt,...){
    va_list ap;
    va_start(ap,fmt);
    handle_exception(STAGE_COMPILE,LEVEL_WARNING,fmt,ap);
}

void error(char * fmt,...){
    va_list ap;
    va_start(ap,fmt);
    handle_exception(STAGE_COMPILE,LEVEL_ERROR,fmt,ap);
}

void expect(char *msg){
    error("lack %s",msg);
}

char * get_tkstr(int v){
    if(v > tktable.count)
        return NULL;
    else if(v >= TK_CINT && v <= TK_CSTR)
        return sourcestr.data;
    else
        return ((TkWord *)tktable.data[v])->spelling;
}

void skip(int c){
    if(token != c)
        error("lack '%s'",get_tkstr(c));
}

void link_error(char * fmt,...){
    va_list ap;
    va_start(ap,fmt);
    handle_exception(STAGE_LINK,LEVEL_ERROR,fmt,ap);
    va_end(ap);
}
