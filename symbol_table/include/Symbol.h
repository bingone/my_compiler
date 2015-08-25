typedef struct Symbol{
	int v;		///符号的单词编码
	int r;		///符号关联的寄存器
	int c;		///符号关联值
	Type type;	///符号数据类型
	struct Symbol *next;	///关联的其他符号
	struct Symbol *prev_tok;///指向前一定义的同名符号
}Symbol;