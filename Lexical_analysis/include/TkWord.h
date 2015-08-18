typedef struct TkWord{
    int tkcode;
    struct TkWord * next;           /// ���ʱ���
    char * spelling;                /// �����ַ���
    struct Symbol * sym_struct;     /// ָ�򵥴�����ʾ�Ľṹ����
    struct Symbol * sym_identifier; /// ָ�򵥴ʱ�ʾ�ı�ʶ��

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
///���ʱ����ҵ��ʣ�keynoҪ�鵥�ʵ�hashֵ p Ҫ��ĵ���
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

