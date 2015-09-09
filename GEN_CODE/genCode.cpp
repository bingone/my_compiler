#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/definemaro.h"
#include "include/DynString.h"
#include "include/DynArray.h"
#include "include/TkWord.h"
#include "include/Stack.h"
#include "include/Symbol.h"
#include "include/genCode.h"
/**
操作数入栈
type 操作数数据类型
r	 操作数存储类型
value 操作数值
*/
void operand_push(Type * type,int r,int value){
	if(optop >= opstack + (OPSTACK_SIZE - 1))
		error("memory allocate failed!");
	optop++;
	optop->type = *type;
	optop->r = r;
	optop->value = value;
}
void operand_pop(){
	optop--;
}
/**
交换栈顶两个操作数顺序
*/
void operand_swap(){
	Operand tmp;
	tmp = optop[0];
	optop[0] = optop[-1];	/// 有点非主流
	optop[-1] = tmp;
}
/**
操作数赋值
t 操作数数据类型
r 操作数存储类型
value 操作数值
*/
void operand_assign(Operand *opd,int t,int r ,int value){
	opd->type.t = t;
	opd->r = r;
	opd->value = value;
}
/*****************************************************************************/
///生成通用指令
/**
向代码节写入一个字节
*/

void gen_byte(char c){
	int ind1;
	ind1 = ind + 1;
	if(ind1 > sec_text->data_allocated)
		section_realloc(sec_text,ind1);
	sec_text->data[ind] = c;
	ind = ind1;
}
/**生成指令前缀*/
void gen_prefix(char opcode){
	gen_byte(opcode);
}
/**生成指令操作码，单字节*/
void gen_opcode1(char opcode){
	gen_byte(opcode);
}
void gen_opcode2(char first,char second){
	gen_byte(first);
	gen_byte(second);
}
/**
生成指令寻址方式字节ModR/M
mod			ModR/M[7:6]
reg_opcode	ModR/M[5:3]指令的另外3位操作码 源操作数
r_m			ModR/M[2:0]目标操作数
sym			符号指针
c			符号关联值
*/
void gen_modrm(int mod,int reg_opcode,int r_m,Symbol * sym,int c){
	mod <<= 6;
	reg_opcode <<= 3;
	if(mod == 0xc0){
		/// mod = 11 寄存器寻址
		/// 89 E5(mod=11 reg_opcode=100ESP r=101 EBP) mov ebp,esp
		gen_byte(mod | reg_opcode | (r_m & SC_VALMASK));
	}else if((r_m & SC_VALMASK) == SC_GLOBAL){
		/// mod = 00 r = 101 直接寻址
		/// 8b 05 50 30 40 00 mov eax,dword ptr ds:[403050]
		gen_byte(0x50 | reg_opcode);
		gen_addr32(r_m,sym,c);
	}else if((r_m & SC_VALMASK) == SC_LOCAL){
		if(c == (char)c){
			/// mod = 01 r = 101 disp8[ebp]
			/// 89 45 fc mov dword ptr ss:[ebp-4],eax
			gen_byte(0x45 | reg_opcode);
			gen_byte(c);
		}else{
			/// mod = 10 r = 101 disp32[ebp]
			/// 89 85 A)FDFFFF mov dword ptr ss:[ebp-260],eax
			gen_byte(0x85 | reg_opcode);
			gen_dword(c);
		}
	}else{
		// mod = 00
		// 89 01(mod=00 reg_opcode = 000eax r=001ecx)mov dword ptr ds:[ecx],eax
		gen_byte(0x00 | reg_opcode |(r_m & SC_VALMASK));
	}
}
/*****************************************************************************/
///生成操作数
/**
生成四字节操作数
*/
void gen_dword(unsigned int c){
	gen_byte(c);
	gen_byte(c>>8);
	gen_byte(c>>16);
	gen_byte(c>>24);
}
/**
生成全局符号地址，增加COFF重定位记录
r		符号存储类型
sym 	符号指针
c		符号关联值
代码生成时无法获知符号符号地址，增加重定位
*/
void gen_addr32(int r,Symbol * sym,int c){
	if(r & SC_SYM)
		cofferloc_add(sec_text,sym,ind,IMAGE_REL_I386_DIR32);
	gen_dword(c);
}
/*****************************************************************************/
///生成数据传送指令
/**
将操作数opd加载到寄存器r中
r_m	符号存储类型
opd 操作数指针
*/
void load(int r,Operand *opd){
	int v,ft,fc,fr;
	fr = opd->r;
	ft = opd->type.t;
	fc = opd->value;
	
	v = fr & SC_VALMASK;
	if(fr & SC_LVAL){
		if((ft & T_BTYPE) == T_CHAR){
			/// movsx -- move with sign-extention
			/// 0F BE /r movsx r32 ,r/m8 move byte to doubleword,sign-extention
			gen_opcode2(0x0f,0xbe);
		}else if((ft & T_BTYPE) == T_SHORT){
			/// movsx--move with sign-extention
			/// 0F BF /r movsx r32,r/m16 move word to doubleword,sign-extention
			gen_opcode2(0x0f,0xbf);
		}else{
			/// 8B /r mov r32,r/m32 mov r/m32 to r32
			gen_opcode1(0x8b);
		}
		gen_modrm(ADDR_OTHER,r,fr,opd->sym,fc);
	}else{
		if(SC_GLOBAL == v){
			///B8 +rd mov r32,imm32		mov imm32 to r32
			gen_opcode1(0xb8 + r);
			gen_addr32(fr,opd->sym,fc);
		}else if(SC_LOCAL == v){
			/// 8D /r LEA r32,m16	Store effective address for m in register
			gen_opcode1(0x8d);
			gen_modrm(ADDR_OTHER,r,SC_LOCAL,opd->sym,fc);
		}else if(SC_CMP ==v){		/// 适用于c=a>b
			/* c=a>b 生成代码
				00401384	39C8		cmp eax,ecx
				00401386	B8 00000000	mov eax,00
				0040138B	0F9FC0		setg allocate
				0040138E	8945FC		mov dword ptr ss:[ebp-4],eax
			
			*/
			/* B8 + rd mov r32,imm32 mov imm32 to r32 */
			gen_opcode1(0xb8 + r);	/*mov r,0*/
			gen_dword(0);
			// OF 9F setg r/m8 set byte if greater(ZF=0 and SF=OF)
			// OF 8F cw/cd 	jg rell6/32 jump new if greater (ZF=0 and SF=OF)
			gen_opcode2(0x0f,fc + 16);
			gen_modrm(ADDR_REG,0,r,NULL,0);
		}else if(v != r){
			// 89 /r mov r/m32,r32 move r32 to r/m32
			gen_opcode1(0x89);
			gen_modrm(ADDR_REG,v,r,NULL,0);
		}
	}
}
/**
将寄存器'r'中的值存入操作数'oopd'中
r_m	符号存储类型
opd 操作数指针
*/
void store(int r,Operand * opd){
	int fr,bt;
	fr = opd->r & SC_VALMASK;
	bt = opd->type.t & T_BTYPE;
	if(bt == T_SHORT)
		gen_prefix(0x66);		/// Operand-size override , 66H
	if(bt == T_CHAR)
		/// 88 /r mov r/m,r8
		gen_opcode1(0x88);
	else
		/// 89 /r mov r/m32,r32 move r32 to r/m32
		gen_opcode1(0x89);
	if(fr == SC_GLOBAL || fr==SC_LOCAL || (opd->r & SC_LVAL))
		gen_modrm(ADDR_OTHER,r,opd->r,opd->sym,opd->value);
}
/**
栈顶操作数加载到'rc'类寄存器中
rc:	寄存器类型
opd:操作数指针
*/
int load_1(int rc,Operand * opd){
	int r;
	r = opd->r & SC_VALMASK;
	/// 需要加载到寄存器中情况
	/// 栈顶操作数目前尚未分配寄存器
	/// 栈顶操作数已分配寄存器，但为左值*p
	if(r >= SC_GLOBAL || (opd->r & SC_LVAL)){
		r = allocate_reg(rc);
		load(r,opd);
	}
	opd->r = r;
	return r;
}
/**
将栈顶操作数加载到'rc1'类寄存器中，将次栈顶加载到'rc2'中类寄存器中
*/
void load_2(int rc1,int rc2){
	load_1(rc2,optop);
	load_1(rc1,optop-1);
}
/**
将栈顶操作数存入次栈顶操作数中
*/
void store0_1(){
	int r,t;
	r = load_1(REG_ANY,optop);
	/// 左值溢出到栈中，必须加载到寄存器中
	if((optop[-1].r & SC_VALMASK) == SC_LLOCAL){
		Operand opd;
		t = allocate_reg(REG_ANY);
		operand_assign(&opd,T_INT,SC_LOCAL | SC_LVAL, optop[-1].value);
		load(t,&opd);
		optop[-1].r = t|SC_LVAL;
	}
	store(r,optop-1);
	operand_swap();
	operand_pop();
}
/*****************************************************************************/
///生成算术与逻辑运算指令
/**
生成二元运算，对指针操作数进行特殊处理
op 运算符类型
*/
void gen_op(int op){
	int u,bt1,bt2;
	Type type1;
	bt1 = optop[-1].type.t & T_BTYPE;
	bt2 = optop[ 0].type,t & T_BTYPE;
	if(bt1 ==T_PTR || bt2 == T_PTR){
		if(op >= TK_EQ && op <= TK_GEQ){	/// 关系运算
			gen_opi(op);
			optop->type.t = T_INT;
		}else if(bt1 ==T_PTR && bt2 == T_PTR){		/// 都为指针
			if(op != TK_MINUS)		///
				error("两个指针只能进行关系运算或减法运算");
			u = pointed_size(&optop[-1].type);
			gen_opi(op);
			optop->type.t = T_INT;
			operand_push(&int_type,SC_GLOBAL,u);
			gen_op(TK_DIVIDE);
		}else{								/// 一个指针，一个非指针，且非关系运算
			if(op != TK_MINUS && op != TK_PLUS)
				error("指针只能进行关系运算或加减法运算");
			/// 指针为第一操作数
			if(bt2 == T_PTR)
				operand_swap();
			type1 = optop[-1].type;
			operand_push(&int_type,SC_GLOBAL,pointed_size(&optop[-1].type));
			gen_op(TK_STAR);
			gen_opi(op);
			optop->type = type1;
		}
	}else{
		gen_opi(op);
		if(op >= TK_EQ && op <= TK_GEQ)
			optop->type.t = T_INT;	/// 关系运算结果为T_INT
	}
}
/**
生成整数运算
op 运算符类型
*/
void gen_opi(int op){
	int r,fr,opc;
	switch(op){
		case TK_PLUS:
			opc = 0;
			gen_opi2(opc,op);
			break;
		case TK_MINUS:
			opc = 5;
			gen_opi2(opc,op);
			break;
		case TK_STAR:
			load_2(REG_ANY,REG_ANY);
			r = optop[-1].r;
			fr = optop[0].r;
			operand_pop();
			/// IMUL 有符号乘法
			/// 0F AF/r IMUL r32,r/m32 
			///doubleword register = doubleword register * r/m doubleword
			gen_opcode2(0x0f,0xaf);
			gen_modrm(ADDR_REG,r,fr,NULL,0);
			break;
		case TK_DIVIDE:
		case TK_MOD:
			opc = 7;
			load_2(REG_EAX,REG_ECX);
			r = optop[-1].r;
			fr = optop[0].r;
			operand_pop();
			spill_reg(REG_EDX);
		///CWD/CDQ--Convert Word to Doubleword/convert doubleword to qradword
			gen_opcode1(0x99);
			/// IDIV 有符号除法
			/// F7 /7 IDIV r/m32 signed divide edx:eax by r/m doubleword
			/// edx:eax 被除数 r/m32 除数
			/// 结果：商 eax 余数 edx
			gen_opcode1(0xf7);
			gen_modrm(ADDR_REG,opc,fr,NULL,0);
			
			if(op == TK_MOD)
				r = REG_EDX;
			else
				r = REG_EAX;
			optop->r = r;
			break;
		default:
			opc = 7;
			gen_opi2(opc,op);
			break;
	}
}

/**
生成整数二元运算
opc ModR/M[5:3]
op: 运算符类型
*/
void gen_opi2(int opc,int op){
	int r,fr,c;
	if((optop->r & SC_VALMASK | SC_LVAL | SC_SYM) == SC_GLOBAL){
		r = load_1(REG_ANY,&optop[-1]);
		c = optop->value;
		if(c==(char)c){
			/// adc add with carry
			/// add
			/// sub subtract
			/// cmp compare two operands
			gen_opcode1(0x83);
			gen_modrm(ADDR_REG,opc,r,NULL,0);
			gen_byte(c);
		}else{
			gen_opcode1(0x81);
			gen_modrm(ADDR_REG,opc,r,NULL,0);
			gen_byte(c);
		}
	}else{
		load_2(REG_ANY,REG_ANY);
		r = optop[-1].r;
		fr = optop[0].r;
		gen_opcode1((op<<3) | 0x01);
		gen_modrm(ADDR_REG,fr,r,NULL,0);
	}
	operand_pop();
	if(op >= TK_EQ && op <= GEQ){
		optop->r = SC_CMP;
		switch(op){
			case TK_EQ:
				optop->value = 0x84;
				break;
			case TK_NEQ:
				optop->value = 0x85;
				break;
			case TK_LT:
				optop->value = 0x8c;
				break;
			case TK_LEQ:
				optop->value = 0x8e;
				break;
			case TK_GT:
				optop->value = 0x8f;
				break;
			case TK_GDQ:
				optop->value = 0x8d;
				break;
			
		}
	}
}
/*****************************************************************************/
///生成控制转移指令
/**
jump code
高地址向低地址，跳转地址待定
t 前一跳转指令地址
*/
int gen_jmpforword(int t){
	/// jmp--jump
	///E9 cd jmp rel32Jump new,relative,displacement relative to next instruction
	gen_opcode1(0xe9);
	return makelist(t);
}
/**
生成向低地址跳转指令，跳转地址确定
a 跳转到的目标地址
*/
void jmpbackword(int a){
	int r;
	r = a - ind - 2;
	if(r==(char)r){
	/// EB cb rel8 Jump short,relative,displacement relative to next instruction
		gen_opcode1(0xe9);
		gen_dword(a - ind - 4);
	}
}
/**
条件跳转指令
返回值 新跳转指令地址
*/
int gen_jcc(int t){
	int v;
	int inv = 1;
	v = optop->r & SC_VALMASK;
	if(v == SC_CMP){
		/// jcc jump if condition is met
		/// 0F 8F cw/cd JG rel16/32 jump new if greater(zf=0 and sf=of)
		gen_opcode2(0x0f,optop->value ^ inv);
		t = makelist(t);
	}else{
		if(optop->r & (SC_VALMASK | SC_LVAL | SC_SYM) == SC_GLOBAL)
			t = gen_jmpforword(t);
		else{
			v = load_1(REG_ANY,optop);
			/// test logical compare
			/// 85 /r test r/m32,r32 and r32 with r/m32,set sf,zf,pf
			gen_opcode1(0x85);
			gen_modrm(ADDR_REG,v,v,NULL,0);
			/// jcc
			gen_opcode2(0x0f,0x85 ^ inv);
			t = makelist(t);
		}
	}
	operand_pop();
	return t;
}
/*****************************************************************************/
/// 生成函数调用
/**
生成函数调用代码，参数入栈，然后生成call
nb_args 参数个数
*/
void gen_invoke(int nb_args){
	int size,r,args_size,i,func_call;
	args_size = 0;
	for(i = 0;i < nb_args;i++){
		r = load_1(REG_ANY,optop);
		size = 4;
		/// push push word or doubleword into stack
		/// 50 + rd push r32 push r32
		gen_opcode1(0x50 + r);	/// push r
		args_size += size;
		operand_pop();
	}
	spill_regs();
	func_call = optop->type.ref->r; 	/// 调用方式
	gen_call();
	if(args_size && func_call != KW_STDCALL)
		gen_addsp(args_size);
	operand_pop();
}
/**
生成函数调用指令
nb_args 参数个数
*/
void gen_call(){
	int r;
	if((optop->r & (SC_VALMASK | SC_LVAL) == SC_GLOBAL){
		/// 重定位信息
		coffreloc_add(sec_text,optop->sym,ind + 1,IMAGE_REL_I386_REL32);
		/// call call prcedure E8 cd
		/// call rel32 call near/relative,displacement relative to next instruction
		gen_opcode1(0xe8);
		gen_dword(optop->value - 4);
	}else{
		/// FF /2 call r/m32 call near,absolute indirect address given in r/m32
		r = load_1(REG_ANY,optop);
		gen_opcode1(0xff);
		gen_opcode1(0xd0 + r);
	}
}
/**
寄存器分配
若所需寄存器被占用，先将其内容溢出到栈中
rc 寄存器类型
*/
int allocate_reg(int rc){
	int r;
	Operand * p;
	int used;
	// find empty register
	for(r = 0;r <= REG_EBX;r++){
		if(rc & REG_ANY || r==rc){
			used = 0;
			for(p = opstack;p <= optop;p++){
				if((p->r & SC_VALMASK) == r)
					used = 1;
			}
			if(used == 0) return r;
		}
	}
	/// 如果没有空寄存器，从操作数栈底开始查找到第一个占用的寄存器溢出到栈中
	for(p = opstack;p <= optop;p++){
		r = p->r & SC_VALMASK;
		if(r < SC_GLOBAL && (rc & REG_ANY || r==rc)){
			spill_reg(r);
			return r;
		}
	}
	return -1;
}
/**
寄存器溢出
将寄存器'r'溢出到内存栈中，标记释放'r'寄存器的操作数为局部变量
r  寄存器编码
*/
void spill_reg(int r){
	int size,align;
	Operand * p,opd;
	Type * type;
	for(p = opstack;p <= optop;p++){
		if((p->r & SC_VALMASK) == r){
			r = p->r & SC_VALMASK;
			type = &p->type;
			if(p->r & SC_LVAL)
				type = &int_type;
			size = type_size(type,&align);
			loc = calc_align(loc - size,align);
			operand_assign(&opd,type->t,SC_LOCAL | SC_LVAL,loc);
			store(r,&opd);
			if(p->r & SC_LVAL)
				p->r = (p->r & ~(SC_VALMASK) | SC_LLOCAL);
			else
				p->r = SC_LOCAL | SC_LVAL;
			p->value = loc;
			break;
		}
	}
}
/**
将占用的寄存器全部溢出到栈中
*/
void spill_regs(){
	int r;
	Operand * p;
	for(p = opstack;p <= optop;p++){
		r = p->r & SC_VALMASK;
		if(r < SC_GLOBAL)
			spill_reg(r);
	}
}

