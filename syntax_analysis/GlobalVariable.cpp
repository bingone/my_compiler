

#include <stdio.h>
#include "include/definemaro.h"
#include "include/DynString.h"
#include "include/DynArray.h"
#include "include/TkWord.h"

#define MAXKEY 1024                 ///hashtable size
TkWord * tk_hashtable[MAXKEY];      ///hashtable
DynArray tktable;                   ///wordlist
DynString tkstr;                    ///word string
DynString sourcestr;                ///word source string
int tkvalue;                        ///word in int value
char ch;                            ///now, hold word
int token;                          ///word code
int line_num;
FILE * fin;                         ///file

int syntax_state;    ///语法状态
int syntax_level;   /// 缩进级别
