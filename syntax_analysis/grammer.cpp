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
void parameter_type_list(int func_call);
void funcbody();
void initializer();;
/**

<translation_unit>::={<external_declaration>}<TK_EOF>

*/
void translation_unit(){
    while(token != TK_EOF)
        external_declaration(SC_GLOBAL);
}

/**

<external_declaration>::=<type_specifier>(<TK_SEMICOLON>
                                         |<declarator><funcbody>
                                         |<declarator>[<TK_ASSIGN><initializer>]
                                         {TK_COMMA><declarator>[<TK_ASSIGN><initializer>]}<TK_SEMICOLON>
                                          )
DO:parse external statement
storage type:local | global
*/
void external_declaration(int l){
    if(!type_specifier())
        expect("<Type specifier>");
    if(token == TK_SEMICOLON){
        get_token();
        return;
    }
    while(1){
        declarator();
        if(token == TK_BEGIN){
            if(l == SC_LOCAL)
                error("don't support function nesting define");
            funcbody();
            break;
        }else {
            if(token == TK_ASSIGN){
                get_token();
                initializer();
            }
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
        syntax_state = SNTX_NULL;
    else
        syntax_state = SNTX_SP;
    syntax_ident();

    if(v < TK_IDENT)
        expect("struct name");  /// key word can't apply for struct name
    if(token == TK_BEGIN)
        struct_declaration_list();
}
/**

struct declaration list
<struct_declaration_list>::=<struct_declaration>{<struct_declaration>}
*/
void struct_declaration_list(){
    int maxalign,offset;
    syntax_state = SNTX_LF_HT;  /// the first structure member doesn't write in the same line with '{'
    syntax_level++;             /// 结构体成员变量声明，缩进增加一级
    get_token();
    while(token != TK_END)
        struct_declaration(&maxalign,&offset);
    skip(TK_END);
    syntax_state = SNTX_LF_HT;
}
/**

<struct)declaration>::=
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
    }
}
/**

形参列表
<parameter_type_list>::=<type_specifier>{<declarator>}
    {<TK_COMMA><type_specifier>{<declarator>}}<TK_COMMA><TK_ELLIPSIS>
*/
void parameter_type_list(int func_call){
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
    if(TK_BEGIN == token)   /// define function
        syntax_state = SNTX_LF_HT;
    else
        syntax_state = SNTX_NUL;
    syntax_indent();
}
/**

<funcbody>::=<compound_statement>
*/
void funcbody(){
    compound_statement();
}
/**

<initializer>::=<assignment_expreession>
*/
void initializer(){
    assignment_expression();
}
