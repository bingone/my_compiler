#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/definemaro.h"
#include "include/TokenCode.h"
#include "include/DynString.h"
#include "include/DynArray.h"
#include "include/TkWord.h"
#include "include/color_token.h"
#include "include/main.h"

int main(int argc,char **argv){
    fin = fopen("C:/Users/shaoyan.xsy/Desktop/a.txt","rb");
    if(!fin){
        printf("File open failed!\n");
        return 0;
    }
    init();
    getch();
    get_token();
	translation_unit();
    cleanup();
    fclose(fin);
    printf("%s Syntax_analysis success!\n",argv[1]);
    return 1;
}
void init(){
    line_num = 1;
    init_lex();
					/// add in symbol
	stack_init(&local_sym_stack,8);
	stack_init(&global_sym_stack,8);
	sym_sec_rdata = sec_sym_put(".rdata",0);
	
	int_type.t = T_INT;
	char_pointer_type.t = T_CHAR;
	mk_pointer(&char_pointer_type);
	default_func_type.t = T_FUNC;
	default_func_type.ref = sym_push(SC_ANOM,&int_type,KW_CDECL,0);
}

void cleanup(){
	sym_pop(&global_sym_stack,NULL);
	sym_destory(&local_sym_stack);
	sym_destory(&global_sym_stack);
					/// add in symbol
    printf("\ntktable.count = %d\n",tktable.count);
    for(int i = TK_IDENT;i < tktable.count; i++)
        free(tktable.data[i]);
    free(tktable.data);
}
