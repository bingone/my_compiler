/** define dynamic array data struct*/
typedef struct DynArray {
    int count;      /// array length
    int capacity;   /// buffer size
    void **data;    /// parr
}DynArray;

/** operator */
extern void dynarray_init(DynArray *parr,int initsize);

extern void dynarray_free(DynArray *parr);

extern int dynarray_search(DynArray *parr,int key);

extern void dynarray_realloc(DynArray *parr,int new_size);

extern void dynarray_add(DynArray *parr,void *data);

extern void error(char * fmt,...);
