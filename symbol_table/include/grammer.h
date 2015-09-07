
void translation_unit();
void external_declaration(int l);
int type_specifier(Type *type);
void struct_specifier();
void struct_declaration_list(Type *type);
int calc_align(int n,int align);
void struct_declaration(int * maxalign,int * offset, Symbol ***ps);
void function_calling_convention(int *fc);
void struct_member_alignment();
void declarator(Type *type,int *v,int *force_align);
void direct_declarator(Type *type,int *v,int func_call);
void direct_declarator_postfix(Type *type,int func_call);
void parameter_type_list(Type *type,int func_call);
void funcbody(Symbol * sym);
void initializer(Type *type);

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
int type_size(Type *t,int *a);
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
extern char * get_tkstr(int v);
