
void translation_unit();
void external_declaration(int l);
int type_specifier();
void struct_specifier();
void struct_declaration_list();
void struct_declaration();
void function_calling_convention(int *fc);
void struct_member_alignment();
void declarator();
void direct_declarator();
void direct_declarator_postfix();
void parameter_type_list();
void funcbody();
void initializer();

void statement(int *bsym,int *csym);
void compound_statement();
int is_type_specifier(int v);
void expression_statement();
void if_statement();
void for_statement();
void continue_statement();
void break_statement();
void return_statement();


void expression();
void assignment_expression();
void relational_expression();
void equality_expression();
void realtional_expression();
void additive_expression();
void multiplicative_expression();
void unary_expression();
void sizeof_expression();
void postfix_expression();
void primary_expression();
void argument_expression_list();

extern int token;
extern int tkvalue;
extern int syntax_level;
extern int syntax_state;
extern void error(char * fmt,...);
extern void syntax_indent();
extern void expect(char *msg);
extern void get_token();
extern void skip(int c);
