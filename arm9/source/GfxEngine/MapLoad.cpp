#include "3D.h"


typedef struct{
	v16 v[4];
	u8 sidewalls;
}v16x4;

u32 MapImage[256][256];

u8 MapLightR[128*128];
u8 MapLightG[128*128];
u8 MapLightB[128*128];

extern u8 WorldLightR[128*128];
extern u8 WorldLightG[128*128];
extern u8 WorldLightB[128*128];

bool outside;

int MapW,MapH;
int MapWreal,MapHreal;

char Mapchange[25][60];
int MapChangePosX[25],MapChangePosY[25];
int MapChangeTOPosX[25],MapChangeTOPosY[25];
int MapChangeCounter;
bool MapChangeDoor[25];
int MapDoorKey[25];
int MapDoorAngle[25];

extern int ObjektTexA[Object_Count];
extern char ObjektTyp[Object_Count][13];
extern u32 ObjektColorKey[Object_Count];//For managing mapchange attributes

extern char DoorSpeech[25][10][256];//to hold 10 textes for each ddor with 256 chars
extern int DoorSpeechCount[25];//Holds the Number of speeches a Door has...

v16 GetTerrain(int x,int y,int num){
	extern v16x4 Terrain[128][128];
	return Terrain[x][y].v[num];
}

void ResetMapChange(void){
	int b;MapChangeCounter=0;
	
	for(int a=0;a<25;a++){
		MapChangeDoor[a]=false;//Is no door
		MapDoorKey[a]=0;//so it doesnt need a key
		MapChangePosX[a]=-1;
		MapChangePosY[a]=-1;
		MapChangeTOPosX[a]=-1;
		MapChangeTOPosY[a]=-1;
		strcpy(Mapchange[a],"");
		MapDoorAngle[a]=0;
		for(b=0;b<10;b++)strcpy(DoorSpeech[a][b],"");
		DoorSpeechCount[a]=0;
		
	}
}

void AddMapChange(int x,int y, char Filename[],int tox, int toy){
	MapChangeDoor[MapChangeCounter]=false;//Is no door
	MapDoorKey[MapChangeCounter]=0;//so it doesnt need a key
	MapChangePosX[MapChangeCounter]=x;
	MapChangePosY[MapChangeCounter]=y;
	MapChangeTOPosX[MapChangeCounter]=tox;
	MapChangeTOPosY[MapChangeCounter]=toy;
	strcpy(Mapchange[MapChangeCounter],Filename);
	MapChangeCounter++;
}

void AddMapDoor(int x,int y, char Filename[],int tox, int toy,int key){
	MapChangeDoor[MapChangeCounter]=true;//Is a door
	MapDoorKey[MapChangeCounter]=key;//so it needs a key
	MapChangePosX[MapChangeCounter]=x;
	MapChangePosY[MapChangeCounter]=y;
	MapChangeTOPosX[MapChangeCounter]=tox;
	MapChangeTOPosY[MapChangeCounter]=toy;
	strcpy(Mapchange[MapChangeCounter],Filename);
	MapChangeCounter++;
	MapDoorAngle[MapChangeCounter]=0;
}

char* GetMapDoor(int x,int y){
	for (int i=0;i<MapChangeCounter;i++){
		if(MapChangePosX[i]==x && MapChangePosY[i]==y && MapChangeDoor[i]==true)return Mapchange[i];
	}
	return ".";
}

int GetMapDoorAngle(int x,int y){
	for (int i=0;i<MapChangeCounter;i++){
		if(MapChangePosX[i]==x && MapChangePosY[i]==y && MapChangeDoor[i]==true)return MapDoorAngle[i];
	}
	return -1;
}

void OpenMapDoor(int x,int y){
extern bool Key[100];
extern char DoorSpeech[25][10][256];//to hold 10 textes for each ddor with 256 chars
extern int DoorSpeechCount[25];//Holds the Number of speeches a Door has...
	for (int i=0;i<MapChangeCounter;i++){
		if(MapChangePosX[i]==x && MapChangePosY[i]==y && MapChangeDoor[i]==true && MapDoorAngle[i]==0){
			MapDoorAngle[i]=10;
			//else TextBoxmode(DoorSpeech[i],DoorSpeechCount[i],-1);
		}
	}
}

void MapDoorHandle(void){
	extern float PlPosX,PlPosY;
	extern int CamPosX,CamPosY;
	extern int Blend;

	for (int i=0;i<MapChangeCounter;i++){
		if(MapDoorAngle[i]>=1 && MapDoorAngle[i]<=80)MapDoorAngle[i]+=10;
	
		if(MapDoorAngle[i]>=80){
			char Filename[50];
			int newx=0,newy=0;
			strcpy(Filename,GetMapChange(MapChangePosX[i],MapChangePosY[i]));
			if(Filename[0]!='.'){
				newx=MapChangeTOPosX[i]-6;
				newy=MapChangeTOPosY[i]-8;
				printf("\x1b[2J");
				
				for(int b=8;b>=0;b--){
					Blend=b;
					RunTime();
					E3D_StartRender();
					UpdateDynamic();
					Refresh3D();
					glPopMatrix (1);
					glFlush(0);
				}
				
				LoadScrip(Filename);
				PlPosX=newx;PlPosY=newy;
				CamPosX=newx;CamPosY=newy;
				
				for(int b=0;b<=8;b++){
					Blend=b;
					RunTime();
					E3D_StartRender();
					UpdateDynamic();
					Refresh3D();
					glPopMatrix (1);
					glFlush(0);
				}
				
			}	
		}//Mapchange if door is opened	
	}
}

char* GetMapChange(int x,int y){
	for (int i=0;i<MapChangeCounter;i++){
		if(MapChangePosX[i]==x && MapChangePosY[i]==y)return Mapchange[i];
	}
	return ".";
}

int GetMapChangePOSX(int x,int y){
	for (int i=0;i<MapChangeCounter;i++){
		if(MapChangePosX[i]==x && MapChangePosY[i]==y)return MapChangeTOPosX[i];
	}
	return 0;
}

int GetMapChangePOSY(int x,int y){
	for (int i=0;i<MapChangeCounter;i++){
		if(MapChangePosX[i]==x && MapChangePosY[i]==y)return MapChangeTOPosY[i];
	}
	return 0;
}

//For getting tiled size of map
int MapGetWr(void){
	return  (MapWreal);
}

int MapGetHr(void){
	return  (MapHreal);
}


//For getting BodenDatas
u32 MapBodenGetRGB(int x,int y){
	int xx=(x*3)+1;
	int yy=(y*3)+1;

	if(x<0)return 0;
	if(y<0)return 0;
	if(x>=MapWreal)return 0;
	if(y>=MapHreal)return 0;//Provides engine to draw shit arround the level
	return MapImage[xx][yy];	
	//return  (MapColors[xx+yy*MapW].w);
}

//For getting Objectrotation
u8 MapObjGetRot(int x,int y){
	int xx=(x*3)+1;
	int yy=(y*3)+2;
	int rot=0;
	u8 rot2=0;
	
	if(x<0)return 0;
	if(y<0)return 0;
	if(x>=MapWreal)return 0;
	if(y>=MapHreal)return 0;//Provides engine to draw shit arround the level
	
	rot= u8(MapImage[xx][yy] & (255));	
	if(rot>=0)rot2=0;
	if(rot>=63)rot2=2;
	if(rot>=127)rot2=4;
	if(rot>=191)rot2=6;
	return rot2;
}

//For getting ObjectDatas
u32 MapObjectGetRGB(int x,int y){
	if(x>-1 && y>-1){
		if(x<MapGetWr() && y<MapGetHr()){
			int xx=(x*3)+2;
			int yy=(y*3)+1;
			//return  (MapColors[xx+yy*MapW].w);
			return MapImage[xx][yy];	
		}
	}
	return 0;
}

bool IsObjHouse(int x,int y){
	u32 Obj;
	int texturecounter,choose=-1;
	
	if(x>-1 && y>-1){
		if(x<MapGetWr() && y<MapGetHr()){
			Obj=MapObjectGetRGB(x,y);
			for(texturecounter=0;texturecounter<10;texturecounter++)
				if(Obj==ObjektColorKey[texturecounter])choose=texturecounter;
			
			if(choose!=-1 && strncmp (ObjektTyp[choose],"HOUSE",5) == 0)return true;
		}
	}
	return false;
}

bool IsObjWall(int x,int y){
	u32 Obj;
	int texturecounter,choose=-1;
	
	if(x>-1 && y>-1){
		if(x<MapGetWr() && y<MapGetHr()){
			Obj=MapObjectGetRGB(x,y);
			for(texturecounter=0;texturecounter<10;texturecounter++)
				if(Obj==ObjektColorKey[texturecounter])choose=texturecounter;
			
			if(choose!=-1 && strncmp (ObjektTyp[choose],"WALL",4) == 0)return true;
		}
	}
	return false;
}

bool IsObjBumpWall(int x,int y){
	u32 Obj;
	int texturecounter,choose=-1;
	
	if(x>-1 && y>-1){
		if(x<MapGetWr() && y<MapGetHr()){
			Obj=MapObjectGetRGB(x,y);
			for(texturecounter=0;texturecounter<10;texturecounter++)
				if(Obj==ObjektColorKey[texturecounter])choose=texturecounter;
			
			if(choose!=-1 && strncmp (ObjektTyp[choose],"BUMPWALL",8) == 0)return true;
		}
	}
	return false;
}

//for getting height
float GetHight(int x,int y){
	int xx=(x*3)+2;
	int yy=(y*3)+2;

	if(x<0)return 0;
	if(y<0)return 0;
	if(x>=MapWreal)return 0;
	if(y>=MapHreal)return 0;//Provides engine to draw shit arround the level

	uint r=0;
	//sscanf(MapColors[xx+yy*MapW].w,"%02X",&r);
	r=MapImage[xx][yy] & (255);
	float r2=float((r>>5) /4.0f);
	
	return (r2);
}

bool Isheight(int x,int y){

}

//Maploading routine
void LoadMap(char* filename){
	
	int width=0;
	int height=0;

	FILE *bmp = fopen(filename,"r") ;
	
	fseek(bmp,18,SEEK_SET) ;

	fread(&width,4,1,bmp) ;
	fread(&height,4,1,bmp) ;
	MapW=width;
	MapH=height;
	MapWreal=width/3;
	MapHreal=height/3;
		
	int i,j;
	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
			MapImage[i][j]=0;
		
	unsigned long colorCoding ;
	fread(&colorCoding,4,1,bmp) ;

	fseek(bmp,34,SEEK_SET) ;
	unsigned long dataLength ;
	fread(&dataLength,4,1,bmp) ;

	int q ;
	unsigned long color ;
	unsigned short r,g,b;
	unsigned long abuf;
	int point;
	
	fseek(bmp,54,SEEK_SET) ;
	switch ((colorCoding & 0xFFFF0000) >> 16) {
		case 24:
			for (i=height-1;i>=0;i--){
				for (q=0;q<width;q++){
						
					fread(&color,3,1,bmp) ;
					b = (color & 0x0FF) ;
					g = ((color >> 8) & 0x0FF) ;
					r = ((color >> 16) & 0x0FF) ;
					
					if(q>=0 && q<256)
						if(i>=0 && i<256)					
							MapImage[q][i]= (r) | (g<<8) | (b<<16) | (0<<24);

					if(q%3==0 && i%3==0){
						point=(q/3)+(i/3)*128;
						if(point<128*128 && point>=0){
							MapLightR[point]=r;
							MapLightG[point]=g;
							MapLightB[point]=b;
		
							WorldLightR[point]=r;
							WorldLightG[point]=g;
							WorldLightB[point]=b;
						}
					}
				} 

				if ((width*3) & 3)
					fread(&abuf,4-((width*3) & 3),1,bmp) ;
				
			} ;
		break ;
	} ;	
	fclose(bmp) ;
}
