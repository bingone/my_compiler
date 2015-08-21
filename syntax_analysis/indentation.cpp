void syntax_ident(){
    switch(syntax_state){
        case SNTX_NUL:
            color_token(LEX_NORMAL);
            break;
        case SNTX_SP:
            printf(" ");
            color_token(LEX_NORMAL);
            break;
        case SNTX_LF_HT:
        {
            if(TK_END == token)
                syntax_level--;
            printf("\n");
            rint_tab(syntax_level);
        }
            color_token(LEX_NORMAL);
            break;
        case SNTX_DELAY:
            break;
    }
    syntax_state = SNTX_NUL;
}
/**
print n tab key
*/
void print_Tab(int n){
    for(int i = 0;i < n;i++)
        printf("\t");
}
