

//#define KOS
#define libfat

#include <stdio.h>
#include <nds.h>

#ifdef libfat
	#include <fat.h>
#endif

//KOS Filesystem
#ifdef KOS

	#include <kos/kosstdio.h>
	#include <kos/fs_romdisk.h>

/*
	#undef FILE
	#undef fopen
	#undef fwrite
	#undef fread
	#undef fclose
	#undef fseek
	#undef fputs
	#undef fgets
	#undef fputc
	#undef getc
	#undef putc
	#undef fgetc
	#undef ftell
	#undef tmpfile
	#undef feof
	#undef stdin
	#undef stdout
	#undef fscanf
	#undef fflush
	

	#define FILE KOS_FILE
	#define fopen KOS_fopen
	#define fwrite KOS_fwrite
	#define fread KOS_fread
	#define fclose KOS_fclose
	#define fseek KOS_fseek
	#define fputs KOS_fputs
	#define fgets KOS_fgets
	#define fputc KOS_fputc
	#define getc KOS_getc
	#define putc KOS_putc
	#define fgetc KOS_fgetc
	#define ftell KOS_ftell
	#define tmpfile KOS_tmpfile
	#define feof KOS_feof
	#define stdin KOS_stdin
	#define stdout KOS_stdout
	#define fscanf KOS_fscanf
	#define fflush KOS_fflush
int Dummy_fprintf(FILE *kf, const char *fmt, ...);

	#define fprintf Dummy_fprintf
*/
#endif



void InitFS(void);
void dispfsinfo(void);

