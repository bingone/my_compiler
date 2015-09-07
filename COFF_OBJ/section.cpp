#include "include/Section.h"
#include <stdio.h>
#include <string.h>

///���ɽڱ�
///COFF��ʼ���õ���ȫ�ֱ���
DynArry sections;	/// ������
Section *sec_text,		/// �����
		*sec_data,		/// ���ݽ�
		*sec_bss,		/// δ��ʼ�����ݽ�
		*sec_idata,		/// ������
		*sec_rdata,		/// ֻ�����ݽ�
		*sec_rel,		/// �ض�λ��Ϣ��
		*sec_symtab,	/// ���ű��
		*sec_dynsymtab;	/// ���ӿ���Ž�
int nsec_image;			/// ӳ���ļ��ڸ���
/**
�½���
name: ������
Characteristics: ������
����ֵ�������ӽ�
nsec_image��ȫ�ֱ�����ӳ���ļ��нڸ���
*/
Section * section_new(char *name,int Characteristics){
	Section *sec;
	int initsize = 8;
	sec = mallocz(sizeof(Section));
	strcpy(sec->sh.Name,name);
	sec->sh.Characteristics = Characteristics;
	sec->index = sections.count + 1;///�ڱ���Ŵ�1��ʼ
	sec->data = mallocz(sizeof(char) * initsize);
	if(!(Characteristics & IMAGE_SCN_LNK_REMOVE))
		nsec_image++;
	dynarry_add(&section,sec);
	return sec;
}

/**
��������Ԥ������increment��С���ڴ�ռ�
sec: Ԥ���ڴ�ռ�Ľ�
increment��Ԥ���Ŀռ��С
����ֵ��Ԥ���ڴ�ռ���׵�ַ

*/
void * section_ptr_add(Section * sec,int increment){
	int offset,offset1;
	offset = sec->data_offset;
	offset1 = offset + increment;
	if(offset1 > sec->data_allocated)
		section_realloc(sec,offset1);
	sec->data_offset = offset1;
	return sec->data + offset;
}
/**
�����������·����ڴ棬�������ݳ�ʼ��Ϊ0
sec�����·����ڴ�Ľ�
new_size���������³���
*/
void section_realloc(Section * sec,int new_size){
	int size;
	char * data;
	size = sec->data_allocated;
	while(size < new_size)
		size = size * 2;
	data = realloc(sec->data,size);
	if(!data)
		error("memory allocation error!");
	memset(data + sec->data_allocated,0,size - sec->data_allocated);	
	/// �����ݳ�ʼ��Ϊ0
	sec->data = data;
	sec->data_allocated = size;
}
/*****************************************************************************/
/// ���ɷ��ű�
/**
����COFF���ű��
symtab: COFF���ű���
Characteristics: ������
strtab_name������ű���ص��ַ�����
����ֵ�� �洢COFF���ű�Ľ�
*/
Section * new_coffsym_section(char * symtab_name,int Characteristics,
								char * strtab_name){
	Section * sec;
	sec = section_new(symtab_name,Characteristics);
	sec->link = section_new(strtab_name,Characteristics);
	sec->hashtab = mallocz(sizeof(int) * MAXKEY);
	return sec;
}
/**
����COFF����
symtab: ����COFF���ű�Ľ�
name: ��������
val���������ص�ֵ
sec_index������˷��ŵĽ�
type��COFF��������
StorageClass: ����COFF���ű������
*/
int coffsym_add(Section * symtab,char * name,int val,int sec_index,
					short type,char StorageClass){
	CoffSym * cfsym;
	int cs,keyno;
	char * csname;
	Section * strtab = symtab->link;
	int * hashtab;
	hashtab = symtab->hashtab;
	cs = coffsym_search(symtab,name);
	if(0==cs){
		cfsym = section_ptr_add(symtab,sizeof(CoffSym));
		scname = coffstr_add(strtab,name);
		cfsym->Name = csname - strtab->data;
		cfsym->Value = val;
		cfsym->Section = sec_index;
		cfsym->Type = type;
		cfsym->StorageClass = StorageClass;
		keyno = elf_hash(name);
		cfsym->Next = hashtab[keyno];
		cs = cfsym - (CoffSym *)symtab->data;
		hashtab[keyno] = cs;
	}
	return cs;
}
/**
����COFF����
symtab: ����COFF���ű�Ľ�
name: ��������
����ֵ�� ����COFF���ű������
*/
int coffsym_search(Section * symtab,char *name){
	CoffSym * cfsym;
	int cs,keyno;
	char * csname;
	Section * strtab;
	keyno = elf_hash(name);
	strtab = symtab->link;
	cs = symtab->hashtab[keyno];
	while(cs){
		cfsym = (CoffSym *)symtab->data + cs;
		csname = strtab->data + cfsym->Name;
		if(!strcmp(name,csname))
			return cs;
		cs = cfsym->Next;
	}
	return cs;
}
/*****************************************************************************/
/// �����ض�λ��Ϣ
/**
�����ض�λ��Ŀ
section:�������ڵĽ�
sym: ����ָ��
offset����Ҫ�����ض�λ�Ĵ��������������Ӧ�ڵ�ƫ��λ��
type: �ض�λ����
*/
void coffreloc_add(Section * sec,Symbol * sym,int offset,char type){
	int cfsym;
	char *name;
	if(!sym->c)
		coffsym_add_update(sym,0,IMAGE_SYM_UNDEFINED,CST_FUNC,
			IMAGE_SYM_CLASS_EXTERNAL);
	name = ((TkWord *)tktable.data[sym->v])->spelling;
	cfsym = coffsym_search(sec_symtab,name);
	coffreloc_direct_add(offset,cfsym,sec->index,type);
}
/**
�����ض�λ��Ϣ
section:�������ڵĽ�
sym: ����ָ��
offset����Ҫ�����ض�λ�Ĵ��������������Ӧ�ڵ�ƫ��λ��
type: �ض�λ����
*/
void coffreloc_direct_add(int offset,int cfsym,char section,char type){
	CoffReloc * rel;
	rel = section_ptr_add(sec_rel,sizeof(CoffReloc));
	rel->offset = offset;
	rel->cfsym = cfsym;
	fel->section = section;
	rel->type = type;
}
/*****************************************************************************/
/// ����obj�ļ�
///COFF��ʼ���õ���ȫ�ֱ���
DynArry sections;	/// ������
Section *sec_text,		/// �����
		*sec_data,		/// ���ݽ�
		*sec_bss,		/// δ��ʼ�����ݽ�
		*sec_idata,		/// ������
		*sec_rdata,		/// ֻ�����ݽ�
		*sec_rel,		/// �ض�λ��Ϣ��
		*sec_symtab,	/// ���ű��
		*sec_dynsymtab;	/// ���ӿ���Ž�
int nsec_image;			/// ӳ���ļ��ڸ���
/*****************************************************************************/
void init_coff(){
	dynarry_init(&sections,8);
	nsec_image = 0;
	
	sec_text = section_new(".text",IMAGE_SCN_MEM_EXECUTE|IMAGE_SCN_CNT_CODE);
	sec_data = section_new(".data",IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_WRITE|
									IMAGE_SCN_CNT_INITIALIZED_DATA);
	sec_rdata = section(".rdata",IMAGE_SCN_MEM_READ
								|IMAGE_SCN_CNT_INITIALIZED_DATA);
	sec_idata = section_new(".idata",IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_WRITE|
									IMAGE_SCN_CNT_INITIALIZED_DATA);
	sec_bss = section_new(".bss",IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_WRITE|
									IMAGE_SCN_CNT_INITIALIZED_DATA);
	sec_rel = section_new(".rel",IMAGE_SCN_LNK_REMOVE|IMAGE_SCN_MEM_READ);
	sec_symtab = new_coffsym_section(".symtab",IMAGE_SCN_LNK_REMOVE|
												IMAGE_SCN_MEM_READ,".strtab");
	sec_dynsymtab = new_coffsym_section(".dynsym",IMAGE_SCN_LNK_REMOVE|
												IMAGE_SCN_MEM_READ,".dynstr");
	coffsym_add(sec_symtab,"",0,0,0,IMAGE_SYM_CLASS_NULL);
	coffsym_add(sec_symtab,".data",0,sec_data->index,0,IMAGE_SYM_CLASS_STATIC);
	coffsym_add(sec_symtab,".bss",0,sec_bss->index,0,IMAGE_SYM_CLASS_STATIC);
	coffsym_add(sec_symtab,".rdata",sec_rdata->index,0,0,IMAGE_SYM_CLASS_STATIC);
	coffsym_add(sec_symtab,"",0,0,0,IMAGE_SYM_CLASS_NULL);
}
/**
�ͷ����н�����
*/
void free_sections(){
	Section * sec;
	for(int i = 0;i < section.count;i++){
		sec = (Section *)sections.data[i];
		if(sec->hashtab != NULL)
			free(sec->hashtab);
		free(sec->data);
	}
	dynarry_free(&sections);
}
/**
���Ŀ���ļ�
*/
void write_obj(char * name){
	int file_offset,i;
	FOLE * fout = fopen(name,"wb");
	int sh_size,nsec_obj;
	IMAGE_FILE_HEADER * fh;
	
	nsec_obj = sections.count - 2;
	sh_size = sizeof(IMAGE_SECTION_HEADER);
	file_offset = sizeof(IMAGE_FILE_HEADER) + nsec_obj * sh_size;
	fpad(fout,file_offset);
	fh = mallocz(sizeof(IMAGE_FILE_HEADER));
	for(i = 0;i < nsec_obj;i++){
		Section *sec = (Section *)sections.data[i];
		if(sec->data == NULL) continue;
		fwrite(sec->data,1,sec->data_offset,fout);
		sec->sh.PointerToRawData = file_offset;
		sec->sh.SizeOfRawData = sec->data_offset;
		file_offset += sec->data_offset;
	}
	fseek(fout,SEEK_SET,0);
	fh->Machine = IMAGE_FILE_MACHINE_I386;
	fh->NumberOfSections = nsec_obj;
	fh->PointerToRawData = sec_symtab->PointerToRawData;
	fh->NumberOfSymbols = sec_symtab->sh.SizeOfRawData / sizeof(CoffSym);
	fwrite(fh,1,sizeof(IMAGE_FILE_HEADER),fout);
	for(i = 0;i < nsec_obj;i++){
		Section * sec = (Section *)sections.data[i];
		fwrite(sec->sh.Name,1,sh_size,fout);
	}
	free(fh);
	fclose(fout);
}
/**
�ӵ�ǰ��дλ�õ�new_pos λ����0��ļ�����
new_pos ��յ�λ��
*/
void fpad(FILE * fp,int new_pos){
	int curpos = ftell(fp);
	while(++curpos <= new_size)
		fputc(0,fp);
}