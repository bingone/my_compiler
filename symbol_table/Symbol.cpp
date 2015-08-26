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
		ts = (TkWord *)tktable.data[(v & ~SC_STRUCT)];	/// 	???
		if(v & SC_STRUCT)
			pps = &ts->sym_struct;
		else
			pps = &ts->sym_identifier;
		ps->prev_tok = *pps;
	}
	return ps;
}

/**
将函数符号放入全局符号表中
*/
Symbol * func_sym_push(int v,Type *type){
	Symbol *s,*ps;
	s = sym_direct_push(&global_sym_stack,v,type,0);
	ps = ((TkWord *)tktable.data[v])->sym_identifier;
	/// 同名符号，函数符号放在最后->->...s
	while(ps != NULL)
		ps = ps->prev_tok;
	s->prev_tok = NULL;
	ps = s;
	return s;
}
/**
插入变量符号
*/
Symbol * var_sym_put(Type *type,int r,int v,int addr){
	Symbol *sym = NULL;
	if((r & SC_VALMASK) == SC_LOCAL)		/// 局部变量
		sym = sym_push(v,type,r,addr);
	else if(v && (r & SC_VALMASK) == SC_GLOBAL){	///	全局变量
		sym = sym_search(v);
		if(sym)
			error("%s redefine\n",((TkWord *)tktable.data[v])->spelling);
		else
			sym = sym_push(v,type,r|SC_SYM,0);
	}
	/// else 字符串常量
	return sym;
}

/**
将节命称放入全局符号表中
sec:节命称
c: 	符号关联值
*/
Symbol * sec_sym_put(char *sec,int c){
	TkWord *tp;
	Symbol *s;
	Type type;
	type.t = T_INT;
	tp = tkword_insert(sec);
	token = tp->tkcode;
	s = sym_push(token,&type,SC_GLOBAL,c);
	return s;
}

/**
符号删除
弹栈直到栈顶符号为'b'
*/
void sym_pop(Stack *ptop,Symbol *b){
	Symbol *s,*ps;
	TkWord *ts;
	int v;
	s = (Symbol *)stack_get_top(ptop);
	while(s != b){
		v = s->v;
		/// 更新单词表中sym_sturct sym_identifier 
		if((v & SC_STRUCT) || v < SC_ANOM){
			ts = (TkWord *)tktable.data[(v & ~SC_STRUCT)];
			if(v & SC_STRUCT)
				ps = &ts->sym_struct;
			else
				ps = &ts->sym_identifier;
		}
		stack_pop(ptop);
		s = (Symbol *)stack_get_top(ptop);
	}
}
/**
符号的查找

*/
Symbol *struct_search(int v){
	if(v >= tktable.count)
		return NULL;
	else
		return ((TkWord *)tktable.data[v])->sym_struct;
}
Symbol *sym_search(int v){
	if(v >= tktable.count)
		return NULL;
	else
		return ((TkWord *)tktable.data[v])->sym_identifier;
}



















