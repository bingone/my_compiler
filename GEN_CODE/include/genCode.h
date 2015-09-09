enum e_AddrForm{
	ADDR_OTHER,		/// 寄存器间接寻址
	ADDR_REG = 3,	/// 寄存器直接寻址
};
typedef struct Operand{
	Type type;				/// 数据类型
	unsigned short r;		/// 寄存器或存储类型
	int value;				/// 常量值
	struct Symbol * sym;	/// 关联符号
}Operand;

void operand_push(Type * type,int r,int value);
void operand_pop();
void operand_swap();
void operand_assign(Operand *opd,int t,int r ,int value);
void gen_byte(char c);
void gen_prefix(char opcode);
void gen_opcode1(char opcode);
void gen_opcode2(char first,char second);
void gen_modrm(int mod,int reg_opcode,int r_m,Symbol * sym,int c);
void gen_dword(unsigned int c);
void gen_addr32(int r,Symbol * sym,int c);
void load(int r,Operand *opd);
void store(int r,Operand * opd);
int load_1(int rc,Operand * opd);
void load_2(int rc1,int rc2);
void store0_1();
void gen_op(int op);
void gen_opi(int op);
void gen_opi2(int opc,int op);
int gen_jmpforword(int t);
void jmpbackword(int a);
int gen_jcc(int t);
void gen_invoke(int nb_args);
void gen_call();
int allocate_reg(int rc);
void spill_reg(int r);
void spill_regs();



extern Operand opstack[OPSTACK_SIZE];
extern Operand * optop;
extern void error(char * fmt,...);