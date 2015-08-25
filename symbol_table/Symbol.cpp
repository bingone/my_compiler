#include "include/Symbol.h"

/**
v 符号编号
type 符号数据类型
c 符号关联值
*/
Symbol * sym_direct_push(Stack *ss,int v,Type *type,int c){
	Symbol s,*p;
	s.v = v;
	s.type.t = type->t;
	s.type.ref = type->ref;
	s.c = c;
	s.next = NULL;
	p = (Symbol *)stack_push(ss,&s,sizeof(Symbol));
	return p;
}
/**
将符号放在符号栈中，动态判断时放入全局符号栈还是局部符号栈
*/
Symbol * sym_push(int v,Type *type,int r,int c){
	Symbol *ps,**pps;
	TkWord *ts;
	Stack  *ss;
	if(0 == stack_is_empty(&local_sym_stack))
		ss = &local_sym_stack;
	else 
		ss = &global_sym_stack;
	ps = sym_direct_push(ss,v,type,c);
	ps->r = r;
	/// 不记录结构体成员及匿名符号
	if((v & SC_STRUCT) || v < SC_ANOM){
		/// 更新单词sym_struct 或sym_identifier 字段
		ts = (TkWord *)tktable.data[(v & ~SC_STRUCT)];
		if(v & SC_STRUCT)
			pps = &ts->sym_struct;
		else
			pps = &ts->sym_identifier;
		ps->prev_tok = *pps;
	}
	return ps;
}


























