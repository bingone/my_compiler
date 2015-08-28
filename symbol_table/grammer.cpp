#include "include/syntax_state.h"
#include "include/grammer.h"
#include "include/TokenCode.h"
#include "include/external_declaration.h"
/***********************************************************************/


/**
翻译单元
<translation_unit>::={<external_declaration>}<TK_EOF>

*/
void translation_unit(){
    while(token != TK_EOF)
        external_declaration(SC_GLOBAL);    /// 函数外部解析状态
}

/**
外部声明
<external_declaration>::=<type_specifier>(<TK_SEMICOLON>
                                         |<declarator><funcbody>
                                         |<declarator>[<TK_ASSIGN><initializer>]
                                         {TK_COMMA><declarator>[<TK_ASSIGN><initializer>]}<TK_SEMICOLON>
                                          )
DO:parse external statement
storage type:local | global
*/
void external_declaration(int l){
	Type btype,type;
	int v,has_init,addr;
	Symbol *sym;
    if(!type_specifier(&btype))
        expect("<Type specifier>");
    if(token == TK_SEMICOLON && T_STRUCT == btype.t){
        get_token();
        return;
    }
    while(1){
		type = btype;
        declarator(&type,&v,NULL);
        if(token == TK_BEGIN){
            if(l == SC_LOCAL)
                error("don't support function nesting define");
			if((type.t & T_BTYPE) != T_FUNC)
				error("<function define>");
			sym = sym_search(v);
			if(sym){								/// 函数之前声明过，现在给出函数定义
				if((sym->type.t & T_BTYPE) !- T_FUNC)
					error("'%s'redefine",get_tkstr(v));
				sym->type = type;
			}else
				sym = func_sym_push(v,&type);
			sym->r = SC_SYM|SC_GLOBAL;
            funcbody(sym);
            break;
        }else {
			if((type.t & T_BTYPE) == T_FUNC){	/// 函数声明
				if(sym_search(v) == NULL)
					sym = sym_push(v,&type,SC_GLOBAL|SC_SYM,0);
			}else{
				r = 0;
				if(!(type.t & T_ARRAY))
					r |= SC_LVAL;
				r |= l;
				has_init = (token == TK_ASSIGN);
				if(has_init){
					get_token();
					initializer(&type);
				}
				sym = var_sym_put(&type,r,v,addr);
			}
				
            // if(token == TK_ASSIGN){
                // get_token();
                // initializer();
            // }
            if(token == TK_COMMA)
                get_token();
            else{
                syntax_state = SNTX_LF_HT;
                skip(TK_SEMICOLON);
                break;
            }
        }
    }
}

/**
类型区分符
<type_specifier>::=<KW_INT>
    |<KW_CHAR>
    |<KW_SHORT>
    |<KW_VOID>
    |<struct_specifier>

parse type specifier
return: is find type specifier
*/
int type_specifier(Type *type){
    int type_found = 0,t = 0;
	Type type1;
    switch(token){
        case KW_CHAR:
			t = T_CHAR;
            type_found = 1;
            syntax_state = SNTX_SP;
            get_token();
            break;
        case KW_SHORT:
			t = T_SHORT;
            type_found = 1;
            syntax_state = SNTX_SP;
            get_token();
            break;
        case KW_VOID:
			t = T_VOID;
            type_found = 1;
            syntax_state = SNTX_SP;
            get_token();
            break;
        case KW_INT:
			t = T_INT;
            type_found = 1;
            syntax_state = SNTX_SP;
            get_token();
            break;
        case KW_STRUCT:
            syntax_state = SNTX_SP;
            struct_specifier();
            type_found = 1;
			t = T_STRUCT;
            break;
        default:
            break;
    }
	type->t = t;
    return type_found;
}

/**
结构区分符
<struct_specifier>::=
    <KW_STRUCT><IDENTIFIER><TK_BEGIN><struct_declaration_list><TK_END>
    |<KW_STURCT><IDENTIFIER>

parse struct

*/
void struct_specifier(Type *type){
    int v;
	Symbol * s;
	Type type1;
	
    get_token();
    v = token;
    syntax_state = SNTX_DELAY;  /// determing output format until take out the next word
    get_token();
    if(token == TK_BEGIN)   /// apply for struct definition
        syntax_state = SNTX_LF_HT;
    else if(token == TK_CLOSEPA)    /// apply for sizeof(struct struct_name)
        syntax_state = SNTX_NUL;
    else
        syntax_state = SNTX_SP;
    syntax_indent();

    if(v < TK_IDENT)
        expect("struct name");  /// key word can't apply for struct name
	s = struct_search(v);
	if(!s){
		type1.t = KW_STRUCT;
		/// -1赋值给s->c, 表示结构体尚未定义
		s = sym_push(v|SC_STRUCT,&type1,0,-1);
		s->r = 0;
	}
	type->t = T_STRUCT;
	type->ref = s;
    if(token == TK_BEGIN)
        struct_declaration_list(type);
}
/**
结构声明符表
struct declaration list
<struct_declaration_list>::=<struct_declaration>{<struct_declaration>}
*/
void struct_declaration_list(Type *type){
    int maxalign,offset;
    syntax_state = SNTX_LF_HT;  /// the first structure member doesn't write in the same line with '{'
    syntax_level++;             /// 结构体成员变量声明，缩进增加一级
    get_token();
	Symbol *s,**ps;
	s = type->ref;
	if(s->c != -1)
		error("struct has been definition");
	maxalign = 1;
	ps = s->next;
	offset = 0;
    while(token != TK_END){
        struct_declaration(&maxalign,&offset,&ps); /// 原书是 struct_declaration(&maxalign,&offset);
		get_token();
	}
    skip(TK_END);
	get_token();
	skip(TK_SEMICOLON);
    syntax_state = SNTX_LF_HT;
	s->c = calc_align(offset,maxalign);	/// 结构体大小
	s->r = maxalign;	/// 结构体对齐
}
/**
功能：计算字节对其位置
n 未对齐前值
align 对齐粒度
*/
int calc_align(int n,int align){
	return ((n + align - 1) & (~(align - 1)));
}
/**
结构声明
<struct_declaration>::=
    <type_specifier><declarator>{<TK_COMMA><declarator>}
    <TK_SEMICOLON>
*/
void struct_declaration(int * maxalign,int * offset, Symbol **ps){
	int v,size,align;
	Symbol **s;
	Type type1,btype;
	int force_align;
    type_specifier(&btype);
    while(1){
		v = 0;
		type1 = btype;
        declarator(&type1,&v,&force_align);
		size = type_size(&type1,&align);
		if(force_align & ALIGN_SET)
			align = force_align & ~ALIGN_SET;
		*offset = calc_align(*offset,align);
		if(align > *maxalign)
			*maxalign = align;
		ss = sys_push(v|SC_MEMBER,&type1,0,*offset);
		*offset +=size;
		**ps = ss;
		*ps = &ss->next;
		
        if(token == TK_SEMICOLON)
            break;
        skip(TK_COMMA);
    }
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}
/**
函数调用约定
function calling
function statement
<function_callint_convention>::=<KW_CDECL>|<KW_STDCALL>
*/
void function_calling_convention(int *fc){
    *fc = KW_CDECL;
    if(token == KW_CDECL || KW_STDCALL == token){
        *fc = token;
        syntax_state = SNTX_SP;
        get_token();
    }
}
/**
结构成员对齐
<struct_member_alignment>::=<KW_ALIGN><TK_OPENPA><TK_CINT><TK_CLOSEPA>
*/
void struct_member_alignment(int * force_align){
	int align = 1;
    if(KW_ALIGN == token){
        get_token();
        skip(TK_OPENPA);
        if(TK_CINT == token){
			get_token();
			align = tkvalue;
		}
            
        else
            expect("integer constant");
        skip(TK_CLOSEPA);
		if(align != 1 && align != 2 && align != 4)
			align = 1;
		align |= ALIGN_SET;
		*force_align = align;
    }else
		*force_align = 1;
}
/**
声明符
<declarator>::={<TK_STAR>}[<function_calling_convention>] [<struct_member_alignment>]<direct_declarator>
*/
void declarator(Type *type,int *v,int *force_align){
    int fc;
    while(TK_STAR == token){
        mk_pointer(type);
		get_token();
	}
    function_calling_convention(&fc);
	if(force_align)
		struct_member_alignment();
    direct_declarator(type,v,fc);
}
/**
直接声明符
<direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
*/
void direct_declarator(Type *type,int *v,int func_call){
    if(TK_IDENT <= token){
        get_token();
		*v = token;
	}
    else
        expect("identifier");
    direct_declarator_postfix(type,func_call);
}
/**
直接声明符后缀
<direct_declarator_postfix>::={<TK_OPENBR><TK_CINT><TK_CLOSEBR>
                            |<TK_OPENBR><TK_CLOSEBR>
                            |<TK_OPENPA><parameter_type_list><TK_CLOSEPA>
                            |<TK_OPENPA><TK_CLOSEPA>}
*/
void direct_declarator_postfix(Type *type,int func_call){
    int n;
	Symbol *s;
    if(TK_OPENPA == token)
        parameter_type_list(type,func_call);
    else if(TK_OPENBR == token){
        get_token();
		n = -1;
        if(TK_CINT == token){
            get_token();
            n = tkvalue;
        }
        skip(TK_CLOSEBR);
        direct_declarator_postfix(type,func_call);
		s = sym_push(SC_ANOM,type,0,n);
		type->t = T_ARRAY|T_PTR;
		get_token();/// TODO
    }
}
/**

形参列表
<parameter_type_list>::=<type_specifier>{<declarator>}
    {<TK_COMMA><type_specifier>{<declarator>}}<TK_COMMA><TK_ELLIPSIS>
*/
void parameter_type_list(Type *type,int func_call){
    int n;
	Symbol **plast,*s,*first;
	Type pt;
    get_token();
	first = NULL;
	plast = &first;
    while(TK_CLOSEPA != token){
        if(!type_specifier(&pt))
            error("invalid identifier");
        declarator(&pt,&n,NULL);
		s = sym_push(n|SC_PARAMS,&pt,0,0);
		*plast = s;
		plast = &s->next;
        if(TK_CLOSEPA == token)
            break;
        skip(TK_COMMA);
        if(TK_ELLIPSIS == token){
            func_call = KW_CDECL;
            get_token();
            break;
        }
    }
    syntax_state = SNTX_DELAY;
    skip(TK_CLOSEPA);
	get_token();								/// TODO 
	/// 此处将函数返回类型存储，然后指向参数，最后将type设为函数类型，引用的相关信息放在ref中
	s = sym_push(SC_ANOM,type,func_call,0);
	s->next = first;
	type->t = T_FUNC;
	type->ref = s;
    if(TK_BEGIN == token)   /// define function
        syntax_state = SNTX_LF_HT;
    else
        syntax_state = SNTX_NUL;
    syntax_indent();
}
/**
函数体
<funcbody>::=<compound_statement>
*/
void funcbody(){
	/**放以匿名符号在局部符号表中*/
	sum_direct_push(&local_sym_stack,SC_ANOM,&int_type,0);
    compound_statement(NULL,NULL);
	/**清空局部符号栈*/
	sym_pop(&local_sym_stack,NULL);
}
/**
初值符
<initializer>::=<assignment_expreession>
*/
void initializer(Type *type){
	if(type->t & T_ARRAY)
		get_token();
	else
		assignment_expression();
}
/********************************************************************************/
/*****************************statement语句**************************************/
/********************************************************************************/

/**
语句
<statement>::=<compound_statement>
            |<if_statement>
            |<return_statement>
            |<break_statement>
            |<continue_statement>
            |for_statement>
            <expression_statement>

*/
void statement(){
    switch(token){
        case TK_BEGIN:
            compound_statement();
            break;
        case KW_IF:
            if_statement();
            break;
        case KW_RETURN:
            return_statement();
            break;
        case KW_BREAK:
            break_statement();
            break;
        case KW_CONTINUE:
            continue_statement();
            break;
        case KW_FOR:
            for_statement();
            break;
        default:
            expression_statement();
            break;
    }
	get_token();	///
}
/**
复合语句
<compound_statement>::=<TK_BEGIN>{<declaration>}{<statement>}<TK_END>

*/
void compound_statement(int * bsym,int * csym){
	Symbol *s;
	s = (Symbol *)stack_get_top(&local_sym_stack);
    syntax_state = SNTX_LF_HT;
    syntax_level++;     ///复合语句，缩�
	get_token();
    while(is_type_specifier(token)){
        external_declaration(SC_LOCAL);
		get_token();
	}
    while(TK_END != token)
		statement();
	sym_pop(&local_sym_stack,s);
    syntax_state = SNTX_LF_HT;
    get_token();
}
/**
判断是否为类型区分符
*/
int is_type_specifier(int v){
    switch(v){
        case KW_CHAR:case KW_SHORT:case KW_INT:case KW_VOID:case KW_STRUCT:
            return 1;
        default:
            break;
    }
    return 0;
}
/**
表达式语句
<expression_statement>::=<TK_SEMICOLON>|<expression><TK_SEMICOLON>
*/
void expression_statement(){
    if(TK_SEMICOLON != token)
        expression();
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}
/**
if
<if_statement>::=<KW_IF><TK_OPENPA><expression><TK_CLOSEPA><statement>[<KW_ELSE><statement>]
*/
void if_statement(){
    syntax_state = SNTX_SP;
    get_token();
    skip(TK_OPENPA);
    expression();
    syntax_state = SNTX_LF_HT;
    skip(TK_CLOSEPA);
	get_token();				///               add
    statement();
    if(KW_ELSE == token){
        syntax_state = SNTX_LF_HT;
        get_token();
        statement();
    }
}
/**
for
<for_statement>::=<KW_FOR><TK_OPENPA><expression_statement><expression_statement><expression><TK_CLOSEPA><statement>
*/
void for_statement(){
        get_token();
        skip(TK_OPENPA);
		get_token();
        if(TK_SEMICOLON != token)
            expression();
        skip(TK_SEMICOLON);
		get_token();
        if(TK_SEMICOLON != token)
            expression();
        skip(TK_SEMICOLON);
		get_token();
        if(TK_CLOSEPA != token)
            expression();
        syntax_state = SNTX_LF_HT;
        skip(TK_CLOSEPA);
		get_token();
        statement();
}
/**
<continue_statement>::=<KW_CONTINUE><TK_SEMICOLON>
*/
void continue_statement(){
    get_token();
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}
/**
<break_statement>::=<KW_BREAK><TK_SEMICOLON>
*/
void break_statement(){
    get_token();
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}
/**
<return_statement>::=<KW_RETURN><TK_SEMICOLON>
                    |<KW_RETURN><expression><TK_SEMICOLON>
*/
void return_statement(){
    syntax_state = SNTX_DELAY;
    get_token();
    if(TK_SEMICOLON == token)       /// for return
        syntax_state = SNTX_NUL;
    else
        syntax_state = SNTX_SP;
    syntax_indent();

    if(TK_SEMICOLON != token)
        expression();
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}
/**
<expression>::=<assignment_expression>{TK_COMMA><assignment_expression>}
*/
void expression(){
    while(1){
        assignment_expression();
        if(TK_COMMA != token)
            break;
        get_token();
    }
}
/**
赋值表达式
<assignment_expression>::=<equality_expression>{<TK_ASSIGN><assignment_expression>}
*/
void assignment_expression(){
    equality_expression();
    if(TK_ASSIGN == token){
        get_token();
        assignment_expression();
    }
}
/**
相等类表达式
<equality_expression>::=<relational_expression>{
                                            <TK_EQ><relational_expression>
                                           |<TK_NEQ><relational_expression>}
*/
void equality_expression(){
    relational_expression();
    while(TK_EQ == token || TK_NEQ == token){
        get_token();
        relational_expression();
    }
}
/**
关系表达式
<relational_expression>::=<additive_expression>{
                                                <TK_LF><additive_expression>
                                               |<TK_GT><additive_expression>
                                               |<TK_LEQ><additive_expression>
                                               |<TK_GEQ><additive_expression>
                                                }
*/
void relational_expression(){
    additive_expression();
    while((TK_LT == token || TK_LEQ == token) ||
           TK_GT == token || TK_GEQ == token){
        get_token();
        additive_expression();
    }
}
/**
加减类表达式
<additive_expression>::=<multiplicative_expression>
                        {<TK_PLUS><multiplicative_expression>
                        |<TK_MINUS><multiplicative_expression>}
*/
void additive_expression(){
    multiplicative_expression();
    while(TK_PLUS == token || TK_MINUS == token){
        get_token();
        multiplicative_expression();
    }
}
/**
乘除类表达式
<multiplicative_expression>::=<unary_expression>
                            {<TK_STAR><unary_expression>
                            |<TK_DIVIDE><unary_expression>
                            |<TK_MOD><unary_expression>}
*/
void multiplicative_expression(){
    unary_expression();
    while(TK_STAR == token || TK_DIVIDE == token || TK_MOD == token){
        get_token();
        unary_expression();
    }
}
/**
一元表达式
<unary_expression>::=<postfix_expression>
                    |<TK_AND><unary_expression>
                    |<TK_STAE><unary_expression>
                    |<TK_PLUS><unary_expression>
                    |<TK_MINUS><unary_expression>
                    |<sizeof_expression>

*/
void unary_expression(){
    switch(token){
        case TK_AND:case TK_STAR:case TK_PLUS:case TK_MINUS:
            get_token();
            unary_expression();
            break;
        case KW_SIZEOF:
            sizeof_expression();
            break;
        default:
            postfix_expression();
            break;
    }
}
/**
<sizeof_expression>::=<KW_SIZEOF><TK_OPENPA><type_specifier><TK_CLOSEPA>
*/
void sizeof_expression(){
	int align,size;
	Type type;
    get_token();
    skip(TK_OPENPA);
    type_specifier(&type);
    skip(TK_CLOSEPA);
	
	size = type_size(&type,&align);
	if(size < 0)
		error("sizeof failed!");
}

/**
返回类型长度
t: 数据类型指针
a: 对齐值
*/
int type_size(Type *t,int *a){
	Symbol *s;
	int bt;
	int PTR_SIZE = 4;	/// 指针长度4字节
	bt = t->t & T_BTYPE;
	switch(bt){
		case T_STRUCT:
			s = t->ref;
			*a = s->r;
			return s->c;
		case T_PTR:
			if(t->t & T_ARRAY){
				s = t->ref;
				return type_size(*s->type,a) * s->c;
			}else{
				*a = PTR_SIZE;
				return PTR_SIZE;
			}
		case T_INT:
			*a = 4;
			return 4;
		case T_SHORT:
			*a = 2;
			return 2;
		default:	/// char void function
			*a = 1;
			return 1;
	}
}
/**
后缀表达式
<postfix_expression>::=<primary_expression>
                      {<TK_OPENPA><expression><TK_CLOSEPA>
                      |<TK_OPENPA><TK_CLOSEPA>
                      |<TK_OPENPA><arguement_expression_list><TK_CLOSEPA>
                      |<TK_DOT><IDENTIFIER>
                      |<TK_POINTSTO><IDENTIFIER>}
*/
void postfix_expression(){
    primary_expression();
    while(1){
        if(TK_DOT == token || TK_POINTSTO == token){
            get_token();
            token |= SC_MEMBER;
            get_token();
        }else if(TK_OPENBR == token){
            get_token();
            expression();
            skip(TK_CLOSEBR);
			get_token();
        }else if(TK_OPENPA == token){
            argument_expression_list();
        }else
            break;
    }
}
/**
初值表达式
<primary_expression>::=<IDENTIFIER>
                      |<TK_CINT>
                      |<TK_CSTR>
                      |<TK_CCHAR>
                      |<TK_OPEN{A><expression><TK_CLOSEPA>
*/
void primary_expression(){
    int t,addr;
	Type type;
	Symbol *s;
    switch(token){
        case TK_CINT:
        case TK_CCHAR:
            get_token();
            break;
        case TK_CSTR:
			t = T_CHAR;
			type.t = t;
			mk_pointer(&type);
			type.t |= T_ARRAY;
			var_sym_put(&type,SC_GLOBAL,0,addr);
			initializer(&type);
            get_token();
            break;
        case TK_OPENPA:
            get_token();
            expression();
            skip(TK_CLOSEPA);
            break;
        default:
            t = token;
            get_token();
            if(t < TK_IDENT)
                expect("identifier");
			s = sym_search(t);
			if(!s){
				if(token != TK_OPENPA)
					error("'%s' 未声明\n",get_tkstr(t));
				s = func_sym_push(t,&default_func_type);	///允许函数不声明，直接引用。
				s->r = SC_GLOBAL|SC_SYM;
			}
            break;
    }
}
/**
实参表达式
<argument_expression_list>::=<assignment_expression> {<TK_COMMA><assignment_expression>}
*/
void argument_expression_list(){
    get_token();
    if(TK_CLOSEPA != token){
        while(1){
            assignment_expression();
            if(TK_CLOSEPA == token)
                break;
            skip(TK_COMMA);
			get_token();
        }
    }
    skip(TK_CLOSEPA);
	get_token();
}
