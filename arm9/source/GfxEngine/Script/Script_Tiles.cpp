#include "Script.h"

extern int BodenTexture[Ground_Count];
extern bool BodenEnable[Ground_Count];
extern u32 BodenColorKey[Ground_Count];
extern bool BodenTransEnable[Ground_Count];
extern bool BodenBumpEnable[Ground_Count];

extern int AutotileTextur[4];
extern int AutotilePal[4];
extern u32 AutotileColorKeyMaster[4];
extern int AutotileIgnorecolorsNum[4];

extern int Wasser[1];
extern u32 WasserKey;
extern int WasserPal[1];

extern int Ufer[10];
extern int UferA[10];
extern int UferB[10];
extern int UferC[10];
extern int UferPal[10];
extern bool UferBump[10];

void LoadBodenCommand(TiXmlElement *map){
	int a=0;
	char FileNameCom[60];
	uint r,g,b;
	
	TiXmlElement *boden = map->FirstChildElement("ground");
	
	while (boden && a<Ground_Count){
		//filename
		strcpy(FileNameCom,"/wolveslayer/tiles/");
		if (boden->Attribute("file"))
			strcat(FileNameCom,boden->Attribute("file"));
					
		//color-id
		r=0;g=0;b=0;
		if (boden->Attribute("colorkey")){
			sscanf(boden->Attribute("colorkey"),"%i,%i,%i",&r,&g,&b);
			BodenColorKey[a]=(r) | (g<<8) | (b<<16) | (0<<24);
		}

		//mirrowing
		if (boden->Attribute("mirrowing"))
			if(strncmp ("true",boden->Attribute("mirrowing"),4) == 0)BodenTransEnable[a]=true;
		
		//Here is that final call to load that texture
		BodenEnable[a]=true;
		
		if(boden->Attribute("bump-mapping")){
			if(strncmp ("true",boden->Attribute("bump-mapping"),4) != 0){
				LoadBodenTexture(FileNameCom,a);
				BodenBumpEnable[a]=false;
			}		
			else {
				LoadMBump5Texture(FileNameCom,a);
				BodenBumpEnable[a]=true;
			}
		}		
		else {
			LoadBodenTexture(FileNameCom,a);
			BodenBumpEnable[a]=false;
		}

		boden =boden->NextSiblingElement("ground");
		a++;
	}
}

void LoadAutoBodenCommand(TiXmlElement *map){
	int a=0;
	int aa=0;
	char FileNameCom[60];
	uint r,g,b;
	
	TiXmlElement *aboden = map->FirstChildElement("autoground");
	TiXmlElement *ig;
	
	while (aboden && a<16){
		//filename
		strcpy(FileNameCom,"/wolveslayer/tiles/");
		if (aboden->Attribute("file"))
			strcat(FileNameCom,aboden->Attribute("file"));
		
		//color-id
		r=0;g=0;b=0;
		if (aboden->Attribute("colorkey")){
			sscanf(aboden->Attribute("colorkey"),"%i,%i,%i",&r,&g,&b);
			AutotileColorKeyMaster[a]=(r) | (g<<8) | (b<<16) | (0<<24);
		}
		
		//Here is that final call to load that texture
		LoadAutotileTexture(FileNameCom,a);
		
		//load the ignore commands
		ig = aboden->FirstChildElement("ignore");
		aa=0;
		while (ig && aa<10){
			//color-id
			r=0;g=0;b=0;
			if (ig->Attribute("colorkey")){
				sscanf(ig->Attribute("colorkey"),"%i,%i,%i",&r,&g,&b);
				AddAutIgnore(a, u32((r) | (g<<8) | (b<<16) | (0<<24)));
			}		
			
			ig = ig->NextSiblingElement("ignore");
			a++;
		}
		
		aboden =aboden->NextSiblingElement("autoground");
		a++;
	}
}

void LoadWaterCommand(TiXmlElement *map){
	char FileNameCom[60];
	uint r,g,b;
	int size;
	
	TiXmlElement *wasser = map->FirstChildElement("water");
	
	if(wasser){
		//filename
		strcpy(FileNameCom,"/wolveslayer/tiles/");
		if (wasser->Attribute("file"))
			strcat(FileNameCom,wasser->Attribute("file"));
		
		//color-id
		r=0;g=0;b=0;
		if (wasser->Attribute("colorkey")){
			sscanf(wasser->Attribute("colorkey"),"%i,%i,%i",&r,&g,&b);
			WasserKey=(r) | (g<<8) | (b<<16) | (0<<24);
		}
		LoadModelTexture(FileNameCom,&Wasser[0],&WasserPal[0],0,&size);
	}
}

void LoadTerrainBorderCommand(TiXmlElement *map){
	int x,y;
	int x2,y2;
	char FileNameCom[60];
	bool bumpmapping;
	TiXmlElement *terrainborder = map->FirstChildElement("terrainborder");
	if(terrainborder){
		//bump atribute
		bumpmapping=false;
		if (terrainborder->Attribute("bumpmapping")){
			if(strncmp ("true",terrainborder->Attribute("bumpmapping"),4) == 0)bumpmapping=true;
		}
		
		//filename
		strcpy(FileNameCom,"/wolveslayer/tiles/");
		if (terrainborder->Attribute("file")){
			strcat(FileNameCom,terrainborder->Attribute("file"));
			if(bumpmapping==false)LoadUferTexture(FileNameCom,&Ufer[0],&UferPal[0],0);
			else LoadMBump3Texture(FileNameCom,0,&UferPal[0],&Ufer[0],&UferB[0],&UferC[0]);
			UferBump[0]=bumpmapping;
		}
	}
}

