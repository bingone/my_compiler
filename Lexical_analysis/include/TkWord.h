typedef struct TkWord{
    int tkcode;
    struct TkWord * next;           /// 单词编码
    char * spelling;                /// 单词字符串
    struct Symbol * sym_struct;     /// 指向单词所表示的结构定义
    struct Symbol * sym_identifier; /// 指向单词表示的标识符

}TkWord;
/** global variable */


void * mallocz(int size){
    void *ptr;
    ptr = malloc(size);
    if(!ptr && size) error("malloc assign failed!");
    memset(ptr,0,size);
    return ptr;
}

int elf_hash(char *key){
    int h=0,g;
    while(*key){
        h = (h<<4) + *key++;
        g = h & 0xf0000000;
        if(g) h ^= g >> 24;
        h &= ~g;
    }
    return h%MAXKEY;
}

/**operator */
TkWord * tkword_direct_insert(TkWord *tp){
    int keyno;
    dynarray_add(&tktable,tp);
    keyno = elf_hash(tp->spelling);
    tp->next = tk_hashtable[keyno];
    tk_hashtable[keyno] = tp;
    return tp;
}
///单词表中找单词，keyno要查单词的hash值 p 要查的单词
TkWord * tkword_find(char * p,int keyno){  ///hash and Sequential search
    TkWord * tp1;
    for(tp1 = tk_hashtable[keyno];tp1;tp1 = tp1->next){
        if(!strcmp(p,tp1->spelling)){
            return tp1;
        }
    }
    return NULL;
}

TkWord * tkword_insert(char *p){
    TkWord * tp;
    int keyno;
    char * s;
    char * end;
    int length;
    keyno = elf_hash(p);
    tp = tkword_find(p,keyno);
    if(tp != NULL) return tp;
    length = strlen(p);
    tp = (TkWord *) mallocz(sizeof(TkWord) + length + 1);
    tp->next = tk_hashtable[keyno];
    tk_hashtable[keyno] = tp;
    dynarray_add(&tktable,tp);
    tp->tkcode = tktable.count + 1;
    s=(char *)tp + sizeof(TkWord);
    tp->spelling = s;
    for(end = p + length;p < end;)
        *s++ = *p++;
    *s = '\0';
    return tp;
}

