typedef struct CoffSym{
	DWORD Name;
	DWORD Next;
	Dword Value;
	short SectionNumber;
	WORD Type;
	BYTE StorageClass;
	BYTE NumberOfAuxSymbols;
}CoffSym;
