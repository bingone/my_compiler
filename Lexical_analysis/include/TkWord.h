typedef struct TkWord{
    int tkcode;
    struct TkWord * next;           /// ���ʱ���
    char * spelling;                /// �����ַ���
    struct Symbol * sym_struct;     /// ָ�򵥴�����ʾ�Ľṹ����
    struct Symbol * sym_identifier; /// ָ�򵥴ʱ�ʾ�ı�ʶ��

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


