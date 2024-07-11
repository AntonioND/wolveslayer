#include "3D.h"

//Those setting are for new game choice
char StartMap[40];
int StartMapX,StartMapY;

char MaincharMd2[40];
char MaincharText[40];

char waffeMd2[40];
char waffeText[40];


//THis is for testing on em...it should double speed...
bool Emuspeed=false;

void Setit(char* setting){
	char Filename[40];
	
	//Readout startmap
	if(strncmp ("StartMap ",setting,9) == 0){
		sscanf(setting,"%*s %s",Filename);
		strcpy(StartMap,"/wolveslayer/maps/");
		strcat(StartMap,Filename);		
	}

	if(strncmp ("StartPos ",setting,9) == 0){
		sscanf(setting,"%*s %d",&StartMapX);
		sscanf(setting,"%*s %*d %d",&StartMapY);
	}
	
	if(strncmp ("StartChar ",setting,10) == 0){
			sscanf(setting,"%*s %s",Filename);
			strcpy(MaincharMd2,"/wolveslayer/chars/");
			strcat(MaincharMd2,Filename);		

			sscanf(setting,"%*s %*s %s",Filename);
			strcpy(MaincharText,"/wolveslayer/chars/");
			strcat(MaincharText,Filename);		
	}

	if(strncmp ("StartWaffe ",setting,11) == 0){
			sscanf(setting,"%*s %s",Filename);
			strcpy(waffeMd2,"/wolveslayer/items/");
			strcat(waffeMd2,Filename);		

			sscanf(setting,"%*s %*s %s",Filename);
			strcpy(waffeText,"/wolveslayer/items/");
			strcat(waffeText,Filename);		
	}
	
}

void ReadSettings(void){
    char text[100];
	int count=0;
	
	FILE *ScriptFile = fopen("/wolveslayer/Settings.txt","r") ;
	if (ScriptFile!=NULL) {

	while (!feof(ScriptFile)){
		fgets(text, 99, ScriptFile); 
			
			if(text[0]!='/'){
				Setit(text);
				count++;
			}
			
			strcpy(text,"");
		}
		fclose(ScriptFile);		
	}
	
	//Add playerpos
	extern float PlPosX,PlPosY;
	extern int CamPosX,CamPosY;

	PlPosX=StartMapX-6;
	PlPosY=StartMapY-8;
	CamPosX=StartMapX-6;
	CamPosY=StartMapY-8;
		
	//Start script file
	ClearItems();
	ResetInventory();	
	LoadItemList();
	LoadScrip(StartMap);
}
