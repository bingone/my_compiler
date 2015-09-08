#include "include/Stack.h"
extern Stack global_sym_stack;
extern Stack local_sym_stack;

void init();
void cleanup();

extern void init_lex();
extern void getch();
extern void get_token();
extern void color_token(int lex_state);
extern void translation_unit();