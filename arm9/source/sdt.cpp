#include "sdt.h"


void InitFS(void){
#ifdef KOS
		REG_IME=0;	
		sysSetCartOwner( BUS_OWNER_ARM9 );
		fs_init(); 
		fs_romdisk_mount("/wolveslayer", (uint8*)find_first_romfs_file((uint8*)0x08000000), 0); 
#endif

#ifdef libfat
		fatInitDefault();
#endif
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
