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
}

void cleanup(){

    printf("\ntktable.count = %d\n",tktable.count);
    for(int i = TK_IDENT;i < tktable.count; i++)
        free(tktable.data[i]);
    free(tktable.data);
}
