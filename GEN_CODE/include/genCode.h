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
extern Operand opstack[200];
extern Operand * optop;
extern void error(char * fmt,...);