#include "script.h"

extern int TextureWidthHeight[Object_Count];
extern bool TexturehasBump[Object_Count];

extern char ObjektTyp[Object_Count][13];
extern u32 ObjektColorKey[Object_Count];

extern bool ObjectMirrow[Object_Count];
extern bool ObjectIllumination[Object_Count];
extern bool ObjectNoGround[Object_Count];
extern float ObjectRadius[Object_Count];
extern int ObjectTextureID[Object_Count];
extern bool ObjectCulling[Object_Count];

int OBJCOUNT;

int CountObjectsCommand(TiXmlElement *map){
	int num=0;
	TiXmlElement *object = map->FirstChildElement("object");
	
	while (object){
		object= object->NextSiblingElement("object");
		num++;
	}
	return num;
}

int ReturnObjectsCount(void)	{return OBJCOUNT;}

void LoadObjectCommand(TiXmlElement *map){
	OBJCOUNT=CountObjectsCommand(map);
	
	char FileNameCom[60];
	//char usca[60];
	char ObjTyp[13];
	int id;
	int num=0;
	uint r,g,b;
	int scale;
	bool dontrenderground;
	bool mirrowable;
	bool ilu;
	bool cull;
	float rad;
	
	TiXmlElement *light;
	int lightnum;
	TiXmlElement *object = map->FirstChildElement("object");

	
	while (object){
		id=-1;

		//id
		if (object->Attribute("textureid"))
			sscanf(object->Attribute("textureid"),"%i",&id);
		ObjectTextureID[num]=id;

		//filename
		strcpy(FileNameCom,"/wolveslayer/obj/");
		if (object->Attribute("mesh")){
			strcat(FileNameCom,object->Attribute("mesh"));
			ucase(FileNameCom,FileNameCom);
			

			if(strncmp ("[wall]",object->Attribute("mesh"),6) == 0){
				if(!TexturehasBump[id])strcpy(ObjTyp,"WALL");
				else strcpy(ObjTyp,"BUMPWALL");
			}
			else if(strncmp ("[house]",object->Attribute("mesh"),7) == 0)strcpy(ObjTyp,"HOUSE");
			else strcpy(ObjTyp,"MODEL");

		}
		
		//color-id
		r=0;g=0;b=0;
		if (object->Attribute("colorkey")){
			sscanf(object->Attribute("colorkey"),"%i,%i,%i",&r,&g,&b);
			ObjektColorKey[num]=(r) | (g<<8) | (b<<16) | (0<<24);
		}
		
		//scale
		scale=40;
		if(object->Attribute("scale"))
			sscanf(object->Attribute("scale"),"%i",&scale);

		//radius
		rad=-1;
		if(object->Attribute("radius")){
			sscanf(object->Attribute("radius"),"%f",&rad);
			if(rad<0)rad=0;
			if(rad>.5)rad=.5;
		}
		ObjectRadius[num]=rad;
		
		//render ground?
		dontrenderground=false;
		if (object->Attribute("renderground")){
			if(strncmp ("false",object->Attribute("renderground"),5) == 0)dontrenderground=true;
		}
		ObjectNoGround[num]=dontrenderground;

		//mirrowable?
		mirrowable=true;
		if (object->Attribute("mirrowable")){
			if(strncmp ("false",object->Attribute("mirrowable"),5) == 0)mirrowable=false;
		}		
		ObjectMirrow[num]=mirrowable;

		//selfilluminated?
		ilu=false;
		if (object->Attribute("selfilluminated")){
			if(strncmp ("true",object->Attribute("selfilluminated"),4) == 0)ilu=true;
		}		
		ObjectIllumination[num]=ilu;
		
		//cullfront?
		cull=false;
		if (object->Attribute("culling")){
			if(strncmp ("true",object->Attribute("culling"),4) == 0)cull=true;
		}		
		ObjectCulling[num]=cull;

		//Final call to load that shit
		if(id<Object_Count && id>=0){
			strcpy(ObjektTyp[num],ObjTyp);
		}
		if(strncmp ("MODEL",ObjektTyp[num],5) == 0 && id>=0)LoadMD2Model(FileNameCom,num+10,TextureWidthHeight[id],scale);
		
			
			//Light (objects children)
			light = object->FirstChildElement("light");
			lightnum=0;
			while (light && lightnum<10){
			
			
				light = object->NextSiblingElement("light");
				lightnum++;
			}
		
		//next object
		object= object->NextSiblingElement("object");
		num++;
		IncreaseCounter();
		UpdateCounter();
	}
}
