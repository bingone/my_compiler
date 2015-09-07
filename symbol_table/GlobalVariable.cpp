

#include <stdio.h>
#include "include/definemaro.h"
#include "include/DynString.h"
#include "include/DynArray.h"
#include "include/TkWord.h"
#include "include/Stack.h"
#include "include/Symbol.h"

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



/**符号表*/
Stack global_sym_stack;		/// 全局符号表
Stack local_sum_stack;		/// 局部符号表

Type char_pointer_type;	/// 字符串指针
Type int_type;			/// int 类型
Type default_func_type;	/// 缺省函数类型