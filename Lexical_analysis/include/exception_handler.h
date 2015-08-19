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

extern FILE * fin;
extern int line_num;
extern DynArray tktable;
extern DynString sourcestr;
extern int token;
