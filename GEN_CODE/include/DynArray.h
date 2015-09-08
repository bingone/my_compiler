/** define dynamic array data struct*/
typedef struct DynArray {
    int count;      /// array length
    int capacity;   /// buffer size
    void **data;    /// parr
}DynArray;

/** operator */
void dynarray_init(DynArray *parr,int initsize);

void dynarray_free(DynArray *parr);

int dynarray_search(DynArray *parr,int key);

void dynarray_realloc(DynArray *parr,int new_size);

void dynarray_add(DynArray *parr,void *data);

extern void error(char * fmt,...);
