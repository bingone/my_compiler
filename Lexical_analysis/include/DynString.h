/** define dynamic string data struct*/
typedef struct DynString {
    int count;      /// str length
    int capacity;   /// buffer size
    char *data;     /// pstr
}DynString;

/** operator */
extern void dynstring_init(DynString *pstr,int initsize);


extern void dynstring_free(DynString *pstr);

extern void dynstring_reset(DynString *pstr);

extern void dynstring_realloc(DynString *pstr,int new_size);

extern void dynstring_chcat(DynString *pstr,int ch);

extern void error(char * fmt,...);
