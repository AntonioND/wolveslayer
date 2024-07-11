#include "script.h"

extern int Figuren[10];
extern int FigurenPal[10];

extern char MaincharMd2[20];
extern char MaincharText[20];
extern char waffeMd2[40];
extern char waffeText[40];

char Names[8][20];
int Atps[8];
int Hps[8];
float rads[8];
bool boss[8];


void LoadChars(TiXmlElement *map){
	int textsize;
//loadmainchar and weapon
	LoadModelTexture(MaincharText,Figuren,FigurenPal,0,&textsize);
	LoadMD2Model(MaincharMd2,0,textsize,35);
	
	LoadModelTexture(waffeText,Figuren,FigurenPal,1,&textsize);
	LoadMD2Model(waffeMd2,1,textsize,35);



//load enemy defenition stuff
//	<enemydef name="Wolve1">
//		<mesh 		file="enemys/wolf.md2" 	scale="40"	radius=".8"/>
//		<texture 	file="enemys/wolf.bmp"			/>
//		<atributes 	healthpower="100" 	attackpower="2"	endgame="true"/>
//	</enemydef>

	char FileNameCom[60];
	char TextureNameCom[60];
	int Num=-1;
	int scale;
	TiXmlElement *endef = map->FirstChildElement("enemydef");
	//TiXmlElement *atributes;
	
	int a;
	for(a=0;a<8;a++)
		strcpy(Names[a],"-.-öäüß");

	while (endef && Num<7){
		
		TiXmlElement *texture=endef->FirstChildElement("texture");		
		TiXmlElement *mesh=endef->FirstChildElement("mesh");
		TiXmlElement *atributes=endef->FirstChildElement("atributes");
		

		
		if(mesh && texture && atributes){
			Num++;

			//copy enemydef-name
			if (endef->Attribute("name"))
				strcpy(Names[Num],endef->Attribute("name"));
	
			//add attackpower
			if (atributes->Attribute("attackpower"))
				sscanf(atributes->Attribute("attackpower"),"%i",&Atps[Num]);
	
			//add hp
			if (atributes->Attribute("healthpower"))
				sscanf(atributes->Attribute("healthpower"),"%i",&Hps[Num]);

			//get the texturefilename
			strcpy(TextureNameCom,"/wolveslayer/chars/");
			if (texture->Attribute("file"))
				strcat(TextureNameCom,texture->Attribute("file"));
			
			//get meshdata
			strcpy(FileNameCom,"/wolveslayer/chars/");
			if (mesh->Attribute("file"))
				strcat(FileNameCom,mesh->Attribute("file"));
			
			//getscale factor
			scale=35;
			if (mesh->Attribute("scale"))
				sscanf(mesh->Attribute("scale"),"%i",&scale);
			
			//get radius
			rads[Num]=.8;
			if (mesh->Attribute("radius"))
				sscanf(mesh->Attribute("radius"),"%f",&rads[Num]);
			
			//get boss status
			boss[Num]=false;
			if (atributes->Attribute("endgame"))
				if(strncmp(atributes->Attribute("endgame"),"true",4)==0)
					boss[Num]=true;
	
			//loadstuff
			if(texture->Attribute("file") && mesh->Attribute("file")){			
				LoadModelTexture(TextureNameCom,Figuren,FigurenPal,2+Num,&textsize);
				LoadMD2Model(FileNameCom,2+Num,textsize,scale);
			}
		}


		//next object
		endef= endef->NextSiblingElement("enemydef");
	}

//now place the enemys
//	<enemy name="Wolve1" pos="3,5"/>
	TiXmlElement *ene = map->FirstChildElement("enemy");
	
	char eName[20];
	int enenum;
	int x,y;
	
	while (ene){

		if (ene->Attribute("name")){
			
			//get the enemy name and search in the defenitions for right one
			strcpy(eName,ene->Attribute("name"));
			enenum=-1;
			for(a=0;a<8;a++)
				if(strncmp(Names[a],eName,strlen(eName))==0)enenum=a;
			
			if (ene->Attribute("pos") && enenum>=0){
				sscanf(ene->Attribute("pos") ,"%i,%i",&x,&y);
				AddEnemy(x,y,enenum+2,Atps[enenum],Hps[enenum],rads[enenum],boss[enenum]);
			}
		}
		
		ene= ene->NextSiblingElement("enemy");		
	}

}

