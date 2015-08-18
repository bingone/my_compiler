/** define dynamic array data struct*/
typedef struct DynArray {
    int count;      /// array length
    int capacity;   /// buffer size
    void **data;    /// parr
}DynArray;

/** operator */
void dynarray_init(DynArray *parr,int initsize){
    if(parr != NULL){
        parr->data = (void **)malloc(sizeof(char) * initsize);
        parr->count = 0;
        parr->capacity = initsize;
    }
}

void dynarray_free(DynArray *parr){
    if(parr == NULL) return;
    void **p;
    for(p=parr->data;parr->count;--parr->count)
        if(*p) free(*p);
    free(parr->data);
    parr->data = NULL;
}

int dynarray_search(DynArray *parr,int key){
    int i;
    int **p = (int **)parr->data;
    for(i=0;i<parr->count;++i,++p)
        if(key==**p) return i;
    return -1;
}

void dynarray_realloc(DynArray *parr,int new_size){
    int capacity;
    void *data;
    capacity = parr->capacity;
    while(capacity < new_size)
        capacity *= 2;
    data = realloc(parr->data,capacity);
    if(!data) error("memory assign failed!");
    parr->capacity = capacity;
    parr->data = (void**)data;
}

void dynarray_add(DynArray *parr,void *data){   ///apend ch
    int count = parr->count + 1;
    if(count > parr->capacity)
        dynarray_realloc(parr,count * sizeof(void *));
    (parr->data)[count-1] = data;
    parr->count = count;
}
