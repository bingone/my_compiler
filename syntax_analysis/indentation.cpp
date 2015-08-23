#include <stdio.h>
#include "include/indentation.h"
#include "include/syntax_state.h"
#include "include/TokenCode.h"
#include "include/color_token.h"
/**
通过yntax_state 判断语法状态，按照格式输出
*/
void syntax_indent(){
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
            if(TK_END == token)     /// 遇到 '}' 缩进减少一级
                syntax_level--;
            printf("\n");
            print_Tab(syntax_level);
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
