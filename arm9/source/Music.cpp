#include "3D.h"
#include "../sound/Sound9.h"

//some sounds
/*
u8* schwert_bin;
u32 schwert_bin_size=0;
u8* wolfaua_bin;
u32 wolfaua_bin_size=0;
u8* wolfstirbt_bin;
u32 wolfstirbt_bin_size=0;
*/


u8 ModfileA[768*1024];
int Modfilesize=0;
char ModFilename[60];

void InitSound(){
	SndInit9();
	SndSetMemPool(&ModfileA, 768*1024);
/*
	FILE *bmp;
	bmp = fopen("/rd/sfx/schwert.bin","rb") ;
	if (bmp!=NULL) {
		fseek(bmp,0,SEEK_END) ;
		schwert_bin_size=ftell(bmp);
		fseek(bmp,0,SEEK_SET) ;	
		for(int a=0;a<schwert_bin_size;a++)fread(&schwert_bin[a],1,1,bmp);
		fclose(bmp);
	}

	bmp = fopen("/rd/sfx/wolfaua.bin","rb") ;
	if (bmp!=NULL) {
		fseek(bmp,0,SEEK_END) ;
		wolfaua_bin_size=ftell(bmp);
		fseek(bmp,0,SEEK_SET) ;	
		for(int a=0;a<wolfaua_bin_size;a++)fread(&wolfaua_bin[a],1,1,bmp);
		fclose(bmp);
	}		

	bmp = fopen("/rd/sfx/wolfstirbt.bin","rb") ;
	if (bmp!=NULL) {
		fseek(bmp,0,SEEK_END) ;
		wolfstirbt_bin_size=ftell(bmp);
		fseek(bmp,0,SEEK_SET) ;	
		for(int a=0;a<wolfstirbt_bin_size;a++)fread(&wolfstirbt_bin[a],1,1,bmp);
		fclose(bmp);
	}
	
	setGenericSound(	11025,	
						127,	
						64,		
						1 );	
*/
}


void StartSong(const char *Name){
	if(strncmp (Name,ModFilename,60) == 0)return;
	strcpy(ModFilename,Name);
	
	SndStopMOD();
	FILE *bmp = fopen(Name,"rb") ;
	if (bmp==NULL) {
		return;
	}
		
	fseek(bmp,0,SEEK_END) ;
	Modfilesize=ftell(bmp);
	fseek(bmp,0,SEEK_SET) ;	
	
	//for(int a=0;a<Modfilesize;a++)fread(&ModfileA[a],1,1,bmp);
	fread(ModfileA,Modfilesize,1,bmp);
	
	fclose(bmp);
	
	SndPlayMOD(ModfileA);
}

void Playhandler(void){

}
