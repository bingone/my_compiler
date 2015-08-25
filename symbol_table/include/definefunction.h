#define MAXKEY 1024
void init_lex();

void getch();
void get_token();
void color_token(int lex_state);
void handle_exception(int stage,int level,char * fmt,va_list ap);
void warning(char * fmt,...);
void error(char * fmt,...);
void expect(char *msg);
char * get_tkstr(int v);
void skip(int c);
void link_error(char * fmt,...);
