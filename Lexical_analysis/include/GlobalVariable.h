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
