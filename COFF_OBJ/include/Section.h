typedef struct Section{
	int data_offset;	/// 当前数据偏移位置
	char *data;			/// 节数据
	int data_allocated;	/// 分配内存空间
	char index;			/// 节序号
	struct Section *link;/// 关联的其他节，符号节关联字符串节
	int *hashtab;		/// 哈希表，仅用于存储符号表
	IMAGE_SECTION_HEADER sh;/// 节头
}Section;

Section * section_new(char *name,int Characteristics);
void * section_ptr_add(Section * sec,int increment);
void section_realloc(Section * sec,int new_size);
Section * new_coffsym_section(char * symtab_name,int Characteristics,
								char * strtab_name);
int coffsym_add(Section * symtab,char * name,int val,int sec_index,
					short type,char StorageClass);
int coffsym_search(Section * symtab,char *name);
void coffreloc_add(Section * sec,Symbol * sym,int offset,char type);
void coffreloc_direct_add(int offset,int cfsym,char section,char type);
void init_coff();
void free_sections();
void write_obj(char * name);
void fpad(FILE * fp,int new_pos);