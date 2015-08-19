#include "include/DynString.h"
#include <stdio.h>
extern int a;
int main(){
    int a;
    DynString pstr;
    dynstring_init(&pstr,8);
    dynstring_chcat(&pstr,'a');
    dynstring_chcat(&pstr,'b');
    printf("%s\n",pstr.data);
    return 0;
}
