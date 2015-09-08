enum e_TypeCode{
	T_INT	= 0,	/// int
	T_CHAR	= 1,	/// char
	T_SHORT	= 2,	/// short
	T_VOID	= 3,	/// void
	T_PTR	= 4,	/// 指针
	T_FUNC	= 5,	/// 函数
	T_STRUCT= 6,	///struct
	
	T_BTYPE	= 0x000f, /// 基本类型掩码
	T_ARRAY = 0x0010, /// 数组
};
typedef struct Type{
	int t;				///数据类型
	struct Symbol *ref;	///引用符号
}Type;
/**
与数据类型相关的全局变量
Type char_pointer_type;	/// 字符串指针
Type int_type;			/// int 类型
Type default_func_type;	/// 缺省函数类型
*/

/**
存储类型
*/
enum e_StorageClass{
	SC_GLOBAL	= 0x00f0,	/// 包括整型常量、字符常量、字符串常量、全局变量、函数定义
	SC_LOCAL	= 0x00f1,	/// 栈中变量
	SC_LLOCAL	= 0x00f2,	/// 寄存器溢出存放栈中
	SC_CMP		= 0x00f3,	/// 使用标志寄存器
	SC_VALMASK	= 0x00ff,	/// 存储类型掩码
	SC_LVAL		= 0x0100,	/// 左值
	SC_SYM		= 0x0200,	/// 符号
	
	SC_ANOM		= 0x10000000,	/// 匿名符号
	SC_STRUCT	= 0x20000000,	/// 结构体符号
	SC_MEMBER	= 0x40000000,	/// 结构成员变量
	SC_PARAMS	= 0x80000000,	/// 函数参数
};
typedef struct Symbol{
	int v;		///符号的单词编码
	int r;		///符号关联的寄存器
	int c;		///符号关联值
	Type type;	///符号数据类型
	struct Symbol *next;	///关联的其他符号
	struct Symbol *prev_tok;///指向前一定义的同名符号
}Symbol;


Symbol * sym_direct_push(Stack *ss,int v,Type *type,int c);
Symbol * sym_push(int v,Type *type,int r,int c);
Symbol * func_sym_push(int v,Type *type);
Symbol * var_sym_put(Type *type,int r,int v,int addr);
Symbol * sec_sym_put(char *sec,int c);
void     sym_pop(Stack *ptop,Symbol *b);
Symbol *struct_search(int v);
Symbol *sym_search(int v);

extern Stack global_sym_stack;		/// 全局符号表
extern Stack local_sym_stack;		/// 局部符号表
extern Type char_pointer_type;	/// 字符串指针
extern Type int_type;			/// int 类型
extern Type default_func_type;	/// 缺省函数类型