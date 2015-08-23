/** error level*/
enum e_ErrorLevel{
    LEVEL_WARNING,
    LEVEL_ERROR,
};

/** work stage*/
enum e_WorkStage{
    STAGE_COMPILE,
    STAGE_LINK,
};

void handle_exception(int stage,int level,char * fmt,va_list ap);
void warning(char * fmt,...);
void error(char * fmt,...);
void expect(char *msg);
void skip(int c);
void link_error(char * fmt,...);

extern FILE * fin;
extern int line_num;
extern DynArray tktable;
extern DynString sourcestr;
extern int token;
