

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
Stack local_sym_stack;		/// 局部符号表
Type char_pointer_type;	/// 字符串指针
Type int_type;			/// int 类型
Type default_func_type;	/// 缺省函数类型

Type char_pointer_type;	/// 字符串指针
Type int_type;			/// int 类型
Type default_func_type;	/// 缺省函数类型

/**代码生成*/
Operand opstack[OPSTACK_SIZE];	/// 操作数栈
Operand * optop;				/// 操作数栈顶
int rsym;						/// 记录return指令位置
int ind = 0;					/// 指令在代码节的位置
int loc;						/// 局部变量在栈中的位置
int func_begin_ind;				/// 函数开始指令
int func_ret_sub;				/// 函数返回释放栈空间
Symbol * sym_sec_rdata;			/// 只读节符号
