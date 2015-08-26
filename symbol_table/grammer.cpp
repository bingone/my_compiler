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
int type_specifier(){
    int type_found = 0;
    switch(token){
        case KW_CHAR:
            type_found = 1;
            syntax_state = SNTX_SP;
            get_token();
            break;
        case KW_SHORT:
            type_found = 1;
            syntax_state = SNTX_SP;
            get_token();
            break;
        case KW_VOID:
            type_found = 1;
            syntax_state = SNTX_SP;
            get_token();
            break;
        case KW_INT:
            type_found = 1;
            syntax_state = SNTX_SP;
            get_token();
            break;
        case KW_STRUCT:
            syntax_state = SNTX_SP;
            struct_specifier();
            type_found = 1;
            break;
        default:
            break;
    }
    return type_found;
}

/**
结构区分符
<struct_specifier>::=
    <KW_STRUCT><IDENTIFIER><TK_BEGIN><struct_declaration_list><TK_END>
    |<KW_STURCT><IDENTIFIER>

parse struct

*/
void struct_specifier(){
    int v;
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
    if(token == TK_BEGIN)
        struct_declaration_list();
}
/**
结构声明符表
struct declaration list
<struct_declaration_list>::=<struct_declaration>{<struct_declaration>}
*/
void struct_declaration_list(){
    int maxalign,offset;
    syntax_state = SNTX_LF_HT;  /// the first structure member doesn't write in the same line with '{'
    syntax_level++;             /// 结构体成员变量声明，缩进增加一级
    get_token();
    while(token != TK_END){
        struct_declaration(); /// 原书是 struct_declaration(&maxalign,&offset);
		get_token();
	}
    skip(TK_END);
	get_token();
	skip(TK_SEMICOLON);
    syntax_state = SNTX_LF_HT;
}
/**
结构声明
<struct_declaration>::=
    <type_specifier><declarator>{<TK_COMMA><declarator>}
    <TK_SEMICOLON>
*/
void struct_declaration(){
    type_specifier();
    while(1){
        declarator();
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
void struct_member_alignment(){
    if(KW_ALIGN == token){
        get_token();
        skip(TK_OPENPA);
        if(TK_CINT == token)
            get_token();
        else
            expect("integer constant");
        skip(TK_CLOSEPA);
    }
}
/**
声明符
<declarator>::={<TK_STAR>}[<function_calling_convention>] [<struct_member_alignment>]<direct_declarator>
*/
void declarator(){
    int fc;
    while(TK_STAR == token)
        get_token();
    function_calling_convention(&fc);
    struct_member_alignment();
    direct_declarator();
}
/**
直接声明符
<direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
*/
void direct_declarator(){
    if(TK_IDENT <= token)
        get_token();
    else
        expect("identifier");
    direct_declarator_postfix();
}
/**
直接声明符后缀
<direct_declarator_postfix>::={<TK_OPENBR><TK_CINT><TK_CLOSEBR>
                            |<TK_OPENBR><TK_CLOSEBR>
                            |<TK_OPENPA><parameter_type_list><TK_CLOSEPA>
                            |<TK_OPENPA><TK_CLOSEPA>}
*/
void direct_declarator_postfix(){
    int n;
    if(TK_OPENPA == token)
        parameter_type_list();
    else if(TK_OPENBR == token){
        get_token();
        if(TK_CINT == token){
            get_token();
            n = tkvalue;
        }
        skip(TK_CLOSEBR);
        direct_declarator_postfix();
		get_token();
    }
}
/**

形参列表
<parameter_type_list>::=<type_specifier>{<declarator>}
    {<TK_COMMA><type_specifier>{<declarator>}}<TK_COMMA><TK_ELLIPSIS>
*/
void parameter_type_list(){
    int func_call;          /// 原书将这个变量当做参数
    get_token();
    while(TK_CLOSEPA != token){
        if(!type_specifier())
            error("invalid identifier");
        declarator();
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
	get_token();
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
    compound_statement();
}
/**
初值符
<initializer>::=<assignment_expreession>
*/
void initializer(){
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
void compound_statement(){
    syntax_state = SNTX_LF_HT;
    syntax_level++;     ///复合语句，缩�
	get_token();
    while(is_type_specifier(token)){
        external_declaration(SC_LOCAL);
		get_token();
	}
    while(TK_END != token)
		statement();
	
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
    get_token();
    skip(TK_OPENPA);
    type_specifier();
    skip(TK_CLOSEPA);
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
    int t;
    switch(token){
        case TK_CINT:
        case TK_CCHAR:
            get_token();
            break;
        case TK_CSTR:
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
