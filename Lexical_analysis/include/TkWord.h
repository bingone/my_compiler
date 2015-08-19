typedef struct TkWord{
    int tkcode;
    struct TkWord * next;           /// 单词编码
    char * spelling;                /// 单词字符串
    struct Symbol * sym_struct;     /// 指向单词所表示的结构定义
    struct Symbol * sym_identifier; /// 指向单词表示的标识符

}TkWord;

void * mallocz(int size);
int elf_hash(char *key);
TkWord * tkword_direct_insert(TkWord *tp);
TkWord * tkword_find(char * p,int keyno);
TkWord * tkword_insert(char *p);


extern FILE * fin;
extern int line_num;
extern DynArray tktable;
extern DynString sourcestr;
extern int token;
extern TkWord * tk_hashtable[MAXKEY];


