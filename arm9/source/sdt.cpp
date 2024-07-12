#include <filesystem.h>

#include "sdt.h"

void InitFS(void){
    nitroFSInit(NULL);
}

int Dummy_fprintf(FILE *kf, const char *fmt, ...)
{
	return 255;
}

void dispfsinfo(void){
	#ifndef FAT
			#ifdef DisplayFatDevice
				extern void Print(char* Text,int x,int y);
				extern int DeviceID;
				if(DeviceID==0)Print("fatlib_init succes",10,10);
				if(DeviceID==1)Print("NinjaFat_init succes",10,10);
				if(DeviceID==2)Print("NeoFat_init succes",10,10);
				//for(int a=0;a<120;a++)swiWaitForVBlank();
			#endif
	#endif
}
