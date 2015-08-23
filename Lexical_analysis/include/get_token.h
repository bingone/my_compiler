void getch();
void skip_white_space();
void parse_comment();
int is_nodigit(char c);
int is_digit(char c);
void parse_identifier();
void parse_num();
void parse_string(char sep);
void preprocess();
void get_token();




extern char ch;
extern DynString tkstr;
extern int tkvalue;
extern void warning(char * fmt,...);
