// GEMS Scanner and Data Extractor
// -------------------------------
// Written by Valley Bell on 18 August 2013

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <ctype.h>
//#include <math.h>

typedef unsigned char	bool;
typedef unsigned char	UINT8;
typedef signed char		INT8;
typedef unsigned short	UINT16;
typedef signed short	INT16;
typedef unsigned int	UINT32;
typedef signed int		INT32;

#define false	0x00
#define true	0x01


#ifdef _MSC_VER
#define INLINE	__inline
#else
#define _stricmp	strcasecmp
#define INLINE	inline
#endif


typedef struct file_info
{
	UINT32 Pos;
	UINT32 Len;
} FILE_INF;


UINT8 LoadInsData(const char* FileName);

UINT8 GemsScan(UINT32 GemsLen, UINT8* GemsData);

static INLINE UINT32 ReadBE24(UINT8* Buffer);
static INLINE INT16 ReadBE16(UINT8* Buffer);
static UINT32 ReadDataWrite(UINT8* Data, UINT32* RetData);
static UINT32 FindJumpTo(UINT32 RomSize, UINT8* RomData, UINT32 FuncPtr);
static UINT32 GetNextAddr(UINT32* AddrList, UINT32 CurAddress);

void SaveData(const char* FileBase, UINT8 DataID, const char* DataName, const UINT32 FileSize, const UINT8* FileData);


char OutFileBase[0x200];
bool ReadOnly;

int main(int argc, char* argv[])
{
	FILE* hFile;
	int RetVal;
	int argbase;
	const char* OutPath;
	char* TempPnt1;
	char* TempPnt2;
	UINT32 StrLen;
	
	UINT32 InLen;
	UINT8* InData;
	
	printf("GEMS Scanner\n------------\n");
	if (argc < 2)
	{
		printf("Tries to find the GEMS driver and extract all GEMS-related data.");
		printf("\n");
		printf("Usage: GemsScan.exe [-Path DumpPath] [-ReadOnly] ROM.bin\n");
		return 0;
	}
	
	argbase = 1;
	OutPath = NULL;
	ReadOnly = false;
	while(argbase < argc && argv[argbase][0] == '-')
	{
		if (! _stricmp(argv[argbase], "-Path"))
		{
			argbase ++;
			if (argbase < argc)
			{
				OutPath = argv[argbase];
				argbase ++;
			}
		}
		else if (! _stricmp(argv[argbase], "-ReadOnly"))
		{
			ReadOnly = true;
			argbase ++;
		}
		else
		{
			break;
		}
	}
	if (argbase >= argc)
	{
		printf("Not enough arguments.\n");
		return 0;
	}
	
	OutFileBase[0x1FF] = '\0';
	if (OutPath == NULL)
	{
		strncpy(OutFileBase, argv[argbase + 0], 0x1FF);	// copy full file path
		// search for last path seperator
		TempPnt1 = strrchr(OutFileBase, '\\');
		TempPnt2 = strrchr(OutFileBase, '/');
		if (TempPnt2 > TempPnt1)
			TempPnt1 = TempPnt2;
		if (TempPnt1 == NULL)
			TempPnt1 = OutFileBase;
	}
	else
	{
		// search for last path seperator of file name
		TempPnt1 = strrchr(argv[argbase + 0], '\\');
		TempPnt2 = strrchr(argv[argbase + 0], '/');
		if (TempPnt2 > TempPnt1)
			TempPnt1 = TempPnt2;
		if (TempPnt1 == NULL)
			TempPnt1 = argv[argbase + 0];
		
		strncpy(OutFileBase, OutPath, 0x1FF);	// copy output path
		
		StrLen = strlen(OutFileBase);
		TempPnt2 = &OutFileBase[StrLen - 1];
		if (*TempPnt2 != '\\' && *TempPnt2 != '/')
		{
			TempPnt2[1] = '/';	// add additional path seperator, if needed
			StrLen ++;
		}
		// copy file name
		strncpy(OutFileBase + StrLen, TempPnt1, 0x1FF - StrLen);
		TempPnt1 = OutFileBase + StrLen;
	}
	// TempPnt1 points to the actual file title now
	TempPnt2 = strrchr(OutFileBase, '.');
	if (TempPnt2 != NULL)
		*TempPnt2 = 0x00;	// remove file extention
	
	hFile = fopen(argv[argbase + 0], "rb");
	if (hFile == NULL)
	{
		printf("Error opening file!\n");
		return 1;
	}
	
	fseek(hFile, 0x00, SEEK_END);
	InLen = ftell(hFile);
	if (InLen > 0x800000)	// 8 MB
		InLen = 0x800000;
	
	fseek(hFile, 0x00, SEEK_SET);
	InData = (UINT8*)malloc(InLen);
	fread(InData, 0x01, InLen, hFile);
	
	fclose(hFile);
	
	RetVal = GemsScan(InLen, InData);
	if (RetVal)
		return RetVal;
	
#ifdef _DEBUG
	getchar();
#endif
	
	return 0;
}

UINT8 GemsScan(UINT32 RomSize, UINT8* RomData)
{
/*
	Usual code that loads the GEMS Z80 driver:
	
	Method 1a: [Kid Chameleon / Aladdin / Sega Channel Demo #4]
 1a		1E5750	41F9 ssss ssss	lea		$ssssss.l, A0	; ss - start address
 1a		1E5756	43F9 eeee eeee	lea		$eeeeee.l, A1	; ee - end address
M1		1E575C	2009			move.l	A1, D0
M1		1E575E	9088			sub.l	A0, D0
M1		1E5760	5340			subq.w	#1, D0
C		1E5762	43F9 00A0 0000	lea		$A00000.l, A1
C		1E5768	12D8			move.b	(A0)+, (A1)+
C		1E576A	51C8 FFFC		dbra	D0, $1E5768
	
	Method 1b: [Taz-Mania, Wacky Worlds]
 1b		050618	41F9 01CC	lea	($01CC,PC), A0	; [$00507E6]
 1b		05061C	43F9 1A47	lea	($1A47,PC), A1	; [$0052065]
M1		050620	2009		move.l	A1, D0
M1		050622	9088		sub.l	A0, D0
M1		050624	5340		subq.w	#1, D0
		... (see Method 1a)
	
	Method 2: [Garfield - Caught in the Act]
M2		0053BE	41F9 ssss ssss	lea		$ssssss.l, A0	; ss - start address
M2		0053C4	303C llll		move.w	#$llll, D0		; ll - data length - 1
C		0053C8	43F9 00A0 0000	lea	$A00000.l, A1
C		0053CE	12D8			move.b	(A0)+, (A1)+
C		0053D0	51C8 FFFC		dbra	D0, $0053CE
	
	Lines with a number at the beginning mark the bytes we will search for.

	Usual code that loads the GEMS data pointers:

	Common code:
	0E14D4	70FF		moveq	#-1, D0
	0E14D6	2F00		move.l	D0, -(A7)
	0E14D8	4EBA xxxx	jsr	(-$xx, PC)		; can also be a 4-byte JSR (4EB9)
	0E14DC	700B		moveq	#$B, D0
	0E14DE	2F00		move.l	D0, -(A7)
	0E14E0	4EBA xxxx	jsr	(-$xx, PC)		; can also be a 4-byte JSR (4EB9)
	
	Method 1: [Kid Chameleon]
	0E14E4	2F3C aaaa aaaa	move.l	#$cccccc, -(A7)
	0E14EA	4EBA xxxx		jsr		(-$xx, PC)
	repeat 4 times
	
	Method 2a: [Aladdin]
	1E586E	2F2E 0008		move.l	($08,A6), -(A7)
	1E5872	4EB9 yyyy yyyy	jsr		$yyyyyy.l
	repeat 4 times
	
	Method 2b: [Sega Channel Demo #4]
	004C68	2F2E 0008		move.l	($08,A6), -(A7)
	004C6C	4EBA xxxx		jsr		(-$xx,PC)
	repeat 4 times
*/
	// Magic Bytes for Loader
	const UINT8 MAGIC_DRV_C[0x0C] =
		{0x43, 0xF9, 0x00, 0xA0, 0x00, 0x00, 0x12, 0xD8, 0x51, 0xC8, 0xFF, 0xFC};
	const UINT8 MAGIC_DRV_M1[0x06] =
		{0x20, 0x09, 0x90, 0x88, 0x53, 0x40};
	const UINT8 MAGIC_LEA_A0[0x02] = {0x41, 0xF9};
	const UINT8 MAGIC_LEA_PC_A1[0x02] = {0x43, 0xFA};
	const UINT8 MAGIC_MOVEI_W[0x02] = {0x30, 0x3C};
	// Magic Bytes for Pointer Setup
	const UINT8 MAGIC_PTR_C1[0x05] =
		{0x70, 0xFF, 0x2F, 0x00, 0x4E};
	const UINT8 MAGIC_PTR_C2[0x05] =
		{0x70, 0x0B, 0x2F, 0x00, 0x4E};
	const UINT8 PTRM_SIZES[0x04] = {0x02, 0x04, 0x02, 0x02};
	const char* DATA_NAMES[0x05] =
		{"Driver", "Instruments", "Envelopes", "Sequences", "Samples"};
	
	FILE_INF DriverInf;
	FILE_INF FileInf[4];
	UINT32 StackPtrs[4];
	UINT8 FoundSeqs;
	UINT8 Method;
	UINT8 PtrMode;
	UINT8 CurFile;
	UINT32 CurPos;
	UINT32 TempPos;
	UINT32 BasePos;
	UINT32 MinPtrArr[8];
	FILE_INF* TempFile;
	
	MinPtrArr[0] = RomSize;
	// --- 1. Search for GEMS Driver Loader ---
	
	// Search for Z80 loader
	Method = 0x00;
	for (CurPos = 0x100; CurPos < RomSize - 0x10; CurPos += 0x02)
	{
		if (! memcmp(&RomData[CurPos], MAGIC_DRV_C, 0x0C))
		{
			if (! memcmp(&RomData[CurPos - 0x06], MAGIC_DRV_M1, 0x06))
			{
				// LEA	StartAddress, A0
				// LEA	EndAddress, A1
				if (! memcmp(&RomData[CurPos - 0x0A], MAGIC_LEA_PC_A1, 0x02))
				{
					// relative LEA
					CurPos -= 0x0E;
					Method = 0x12;
					DriverInf.Pos = CurPos + 0x02 + ReadBE16(&RomData[CurPos + 0x02]);
					DriverInf.Len = CurPos + 0x06 + ReadBE16(&RomData[CurPos + 0x06]) - DriverInf.Pos;
				}
				else
				{
					// absolute LEA
					CurPos -= 0x12;
					Method = 0x11;
					DriverInf.Pos = ReadBE24(&RomData[CurPos + 0x02]);
					DriverInf.Len = ReadBE24(&RomData[CurPos + 0x08]) - DriverInf.Pos;
				}
				break;
			}
			else if (! memcmp(&RomData[CurPos - 0x0A], MAGIC_LEA_A0, 0x02) &&
					! memcmp(&RomData[CurPos - 0x04], MAGIC_MOVEI_W, 0x02))
			{
				// LEA	StartAddress, A0
				// MOVE	DriverSize, D0
				CurPos -= 0x0A;
				Method = 0x20;
				DriverInf.Pos = ReadBE24(&RomData[CurPos + 0x02]);
				DriverInf.Len = ReadBE16(&RomData[CurPos + 0x08]) + 1;
				break;
			}
		}
	}
	if (! Method)
	{
		printf("GEMS loader not found!\n");
		return 0x00;
	}
	printf("GEMS loader found at %06X (Method %u%c).\n", CurPos, Method >> 4, (Method & 0x0F) ? 0x60 + (Method & 0x0F) : '\0');
	printf("Driver:\t\t%06X, ", DriverInf.Pos);
	if ((Method & 0xF0) == 0x10)
		printf("Driver End: %06X (Size %04X)\n", DriverInf.Pos + DriverInf.Len, DriverInf.Len);
	else
		printf("Driver Size: %04X\n", DriverInf.Len);
	MinPtrArr[1] = DriverInf.Pos;
	
	SaveData(OutFileBase, 0x00, DATA_NAMES[0], DriverInf.Len, RomData + DriverInf.Pos);
	
	// --- 2. Search for GEMS pointer writes ---
	Method = 0x00;
	FoundSeqs = 0x00;
	for (CurPos = 0x100; CurPos < RomSize - 0x10; CurPos += 0x02)
	{
		if (! memcmp(&RomData[CurPos], MAGIC_PTR_C1, 0x05))	// check for GEMS command FF [command start]
		{
			BasePos = CurPos;
			
			PtrMode = RomData[CurPos + 0x05] & 0x03;
			CurPos += 0x06 + PTRM_SIZES[PtrMode];
			if (memcmp(&RomData[CurPos], MAGIC_PTR_C2, 0x05))	// check for GEMS command 0B [write pointers]
				continue;
			PtrMode = RomData[CurPos + 0x05] & 0x03;
			CurPos += 0x06 + PTRM_SIZES[PtrMode];
			
			memset(&FileInf, 0x00, sizeof(FILE_INF) * 4);
			Method = 0x10;
			for (CurFile = 0; CurFile < 4; CurFile ++)
			{
				if (RomData[CurPos] != 0x2F)
				{
					printf("Unexpected instruction %04X found at %06X!\n", ReadBE16(&RomData[CurPos]), CurPos);
					break;
				}
				CurPos += ReadDataWrite(&RomData[CurPos], &FileInf[CurFile].Pos);
				if ((INT32)FileInf[CurFile].Pos >= 0)
					Method |= 0x01;	// immediate pointers used
				else
					Method |= 0x02;	// pointers from stack
				if (RomData[CurPos] != 0x4E)
				{
					printf("Unexpected instruction %04X found at %06X!\n", ReadBE16(&RomData[CurPos]), CurPos);
					break;
				}
				PtrMode = RomData[CurPos + 0x01] & 0x03;
				CurPos += 0x02 + PTRM_SIZES[PtrMode];
			}
			printf("Pointer loader found at %06X (Method %u)\n", BasePos, Method & 0x0F);
			
			if (Method & 0x02)
			{
				// Find the beginning of the function
				// (This should be 3 commands ago, 2xJSR and 1x LINK.)
				TempPos = BasePos;
				
				BasePos = 0x00;
				while(1)
				{
					if (RomData[TempPos - 0x04] == 0x4E)
					{
						if (RomData[TempPos - 0x03] == 0xBA)
						{
							TempPos -= 0x04;	// relative JSR (4EBA)
							continue;
						}
						else if ((RomData[TempPos - 0x03] & 0xF8) == 0x50)
						{
							TempPos -= 0x04;	// LINK	Ax, #value (4E5x vvvv)
							BasePos = TempPos;
						}
					}
					else if ((RomData[TempPos - 0x04] & 0xF1) == 0x11 &&	// for Wacky Worlds
							(RomData[TempPos - 0x03] & 0xF8) == 0x40)
					{
						TempPos -= 0x04;	// MOVE.B	D0, (addr, Ax) (1x4x vvvv)
						continue;
					}
					else if (RomData[TempPos - 0x06] == 0x4E &&
							RomData[TempPos - 0x05] == 0xB9)
					{
						TempPos -= 0x06;	// absolute JSR (4EB9)
						continue;
					}
					else if ((RomData[TempPos - 0x02] & 0xF8) == 0x70)	// for Wacky Worlds
					{
						TempPos -= 0x02;	// MOVEQ	#value, D0	(7xvv)
						continue;
					}
					
					break;	// quit on unknown commands and CurFile == 0x01
				}
				
				if (! BasePos)
				{
					printf("Failed to find beginning of 'Write pointers' routine!\n");
					for (CurFile = 0; CurFile < 4; CurFile ++)
					{
						if ((INT32)FileInf[CurFile].Pos < 0)
							FileInf[CurFile].Pos = 0x00;
					}
				}
				else
				{
					printf("'Write Pointers' routine begins at %06X, ", BasePos);
					TempPos = FindJumpTo(RomSize, RomData, BasePos);
					printf("jump is at %06X\n", TempPos);
					
					// Read pointers written on the stack.
					for (CurFile = 0; CurFile < 4; CurFile ++)
					{
						TempPos -= 0x06;
						if (ReadBE16(&RomData[TempPos]) == 0x4879 ||	// PEA	addr.l
							ReadBE16(&RomData[TempPos]) == 0x2F3C ||	// MOVE	#value, -(A7)
							ReadBE16(&RomData[TempPos]) == 0x2EBC)		// MOVE	#value, (A7)
						{
							StackPtrs[CurFile] = ReadBE24(&RomData[TempPos + 0x02]);
						}
						else
						{
							if (ReadBE16(&RomData[TempPos + 0x04]) == 0x60FE ||				// check for BRA.S	*-2
								(ReadBE16(&RomData[TempPos + 0x04]) & 0xF1F8) == 0x2118)	// check for MOVE.L	(Ax)+, -(Ax)
								printf("Sorry, but this ROM uses large jump tables that obfuscate everything.\n");
							else
								printf("Unexpected instruction %04X found at %06X!\n",
										ReadBE16(&RomData[TempPos]), TempPos);
							for (; CurFile < 4; CurFile ++)
								StackPtrs[CurFile] = 0x00;
							break;
						}
					}
					printf("Pointers are written to stack at %06X\n", TempPos);
					
					// Then set respective pointers to the files.
					TempFile = FileInf;
					for (CurFile = 0; CurFile < 4; CurFile ++, TempFile ++)
					{
						if ((INT32)TempFile->Pos < 0)
						{
							TempPos = (-(INT32)TempFile->Pos - 0x08) / 0x04;
							if (TempPos < 4)
								TempFile->Pos = StackPtrs[TempPos];
							else
								TempFile->Pos = 0x00;
						}
					}
				}
			}
			
			// create pointer array for size calculation
			TempPos = 2;
			for (CurFile = 0; CurFile < 4; CurFile ++)
			{
				if (FileInf[CurFile].Pos)
				{
					MinPtrArr[TempPos] = FileInf[CurFile].Pos;
					TempPos ++;
				}
			}
			MinPtrArr[TempPos] = 0x00;
			
			// TempPos == 2 means, that no valid pointer was found
			if (TempPos > 2)
			{
				// print and save everything
				PtrMode = (FoundSeqs << 4) | 0x01;
				TempFile = FileInf;
				for (CurFile = 0; CurFile < 4; CurFile ++, TempFile ++, PtrMode ++)
				{
					if (! TempFile->Pos || (CurFile < 3 && TempFile->Pos == FileInf[CurFile + 1].Pos))
						TempFile->Len = 0x00;
					else
						TempFile->Len = GetNextAddr(MinPtrArr, TempFile->Pos) - TempFile->Pos;
					printf("%s:\t%06X (guessed size: %05X)\n", DATA_NAMES[1 + CurFile], TempFile->Pos, TempFile->Len);
					SaveData(OutFileBase, PtrMode, DATA_NAMES[1 + CurFile], TempFile->Len, RomData + TempFile->Pos);
				}
				
				FoundSeqs ++;
			}
		}
	}
	if (! Method)
	{
		printf("GEMS loader not found!\n");
		return 0x00;
	}
	
	return 0x00;
}

static INLINE UINT32 ReadBE24(UINT8* Buffer)
{
	// 68000 Addresses only use 24-bit, so I can (and should) ignore the first byte.
	return	(Buffer[0x01] << 16) |
			(Buffer[0x02] <<  8) |
			(Buffer[0x03] <<  0);
}

static INLINE INT16 ReadBE16(UINT8* Buffer)
{
	return	(Buffer[0x00] << 8) |
			(Buffer[0x01] << 0);
}

static UINT32 ReadDataWrite(UINT8* Data, UINT32* RetData)
{
	if (Data[0x01] == 0x3C)
	{
		// 2F3C aaaa aaaa	MOVE.L	#$aaaaaaaa, -(A7)
		*RetData = ReadBE24(&Data[0x02]);
		return 0x06;
	}
	else //if (Data[0x01] == 0x2E)
	{
		// 2F00				MOVE.L	D0, -(A7)
		*RetData = -ReadBE16(&Data[0x02]);
		return 0x04;
	}
	
	return 0x02;
}

static UINT32 FindJumpTo(UINT32 RomSize, UINT8* RomData, UINT32 FuncPtr)
{
	const UINT8 ASM_BRA = 0x60;
	const UINT8 ASM_BSR = 0x61;
	const UINT8 ASM_JMP_JSR = 0x4E;
	const UINT8 ASM_JMP = 0xF8;
	const UINT8 ASM_JSR = 0xB8;
	UINT32 CurPos;
	INT16 Ptr16;
	UINT32 Ptr32;
	UINT32 PtrSize;
	
	for (CurPos = 0x100; CurPos < RomSize - 0x10; CurPos += 0x02)
	{
		if (RomData[CurPos] == ASM_JMP_JSR)
		{
			if ((RomData[CurPos + 0x01] & 0xF8) == ASM_JMP ||
				(RomData[CurPos + 0x01] & 0xF8) == ASM_JSR)
			{
				switch(RomData[CurPos + 0x01] & 0x03)
				{
				case 0x00:	// Jxx	addr.W
					Ptr16 = ReadBE16(&RomData[CurPos + 0x02]);
					Ptr32 = (INT32)Ptr16 & 0x00FFFFFF;
					PtrSize = 0x02;
					break;
				case 0x01:	// Jxx	addr.L
					Ptr32 = ReadBE24(&RomData[CurPos + 0x02]);
					PtrSize = 0x04;
					break;
				case 0x02:	// Jxx	(addr.W, PC)
					Ptr16 = ReadBE16(&RomData[CurPos + 0x02]);
					Ptr32 = CurPos + 0x02 + Ptr16;
					PtrSize = 0x02;
					break;
				default:
					Ptr32 = 0x00;
					PtrSize = 0x00;
					break;
				}
				if (Ptr32 == FuncPtr)
					return CurPos;
				CurPos += PtrSize;
			}
		}
		else if (RomData[CurPos] == ASM_BRA || RomData[CurPos] == ASM_BSR)
		{
			Ptr32 = CurPos + 0x02;
			if (RomData[CurPos + 0x01] != 0x00)
			{
				// Bxx.S
				Ptr16 = (INT8)RomData[CurPos + 0x01];
				PtrSize = 0x00;	// actually size 1
			}
			else
			{
				// Bxx.W
				Ptr16 = ReadBE16(&RomData[CurPos + 0x02]);
				PtrSize = 0x02;
			}
			Ptr32 += Ptr16;
			
			if (Ptr32 == FuncPtr)
				return CurPos;
			CurPos += PtrSize;
		}
	}
	
	return 0x00;
}

static UINT32 GetNextAddr(UINT32* AddrList, UINT32 CurAddress)
{
	// based on the one I wrote for SMPSExtract
	UINT32 RetAddr;
	UINT32* AddrPtr;
	
	RetAddr = 0xFFFFFFFF;
	AddrPtr = AddrList;
	while(*AddrPtr)
	{
		if (*AddrPtr > CurAddress && *AddrPtr < RetAddr)
			RetAddr = *AddrPtr;
		AddrPtr ++;
	}
	
	return RetAddr;
}

void SaveData(const char* FileBase, UINT8 DataID, const char* DataName, const UINT32 FileSize, const UINT8* FileData)
{
	FILE* hFile;
	char* OutFile;
	
	if (ReadOnly)
		return;
	
	OutFile = (char*)malloc(strlen(FileBase) + strlen(DataName) + 0x10);
	sprintf(OutFile, "%s - %02u %s.bin", FileBase, DataID, DataName);
	
	hFile = fopen(OutFile, "wb");
	if (hFile == NULL)
	{
		printf("Error opening %s!\n", OutFile);
		free(OutFile);
		return;
	}
	
	fwrite(FileData, 0x01, FileSize, hFile);
	
	fclose(hFile);
	free(OutFile);
	
	return;
}
