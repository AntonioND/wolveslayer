#include "3D.h"


void LoadMBump3Texture(char filename[],int num,int* targetp,int* target,int* targetb,int* targetc) 
{
	u8* buffer8;
	u16 pal[256];
	int width=0;
	int height=0;
		
	FILE *bmp = fopen(filename,"r") ;
	if (bmp==NULL) {
		PrintOUT(filename,5,5,false,strlen(filename));
		PrintOUT("wasnt found",5,15,false,strlen("wasnt found"));
		while(!(keysDown() & KEY_A))scanKeys();
		ScreenModeLOADING();
		return;
	}
	
	fseek(bmp,18,SEEK_SET) ;

	fread(&width,4,1,bmp) ;
	fread(&height,4,1,bmp) ;

	if(width!=64 || height!=64){
		char bug[40];
		PrintOUT(filename,5,5,false,strlen(filename));
		PrintOUT("Texture isnt 64x64",5,15,false,strlen("Texture isnt 64x64"));
		sprintf(bug,"its %dx%d",width,height);
		PrintOUT(bug,5,25,false,strlen(bug));	
		while(!(keysDown() & KEY_A))scanKeys();
		ScreenModeLOADING();
		return;	
	}
	
	unsigned long colorCoding ;
	fread(&colorCoding,4,1,bmp) ;

	if(((colorCoding & 0xFFFF0000) >> 16)!=8){
		char bug[40];
		PrintOUT(filename,5,5,false,strlen(filename));
		PrintOUT("Texture isnt 8bit...",5,15,false,strlen("Texture isnt 8bit..."));
		sprintf(bug,"its %dBit",int((colorCoding & 0xFFFF0000) >> 16));
		PrintOUT(bug,5,25,false,strlen(bug));	
		while(!(keysDown() & KEY_A))scanKeys();
		ScreenModeLOADING();
		return;		
	}
	
	fseek(bmp,34,SEEK_SET) ;
	unsigned long dataLength ;
	fread(&dataLength,4,1,bmp) ;

	fseek(bmp,46,SEEK_SET) ;
	unsigned long impcol ;
	fread(&impcol,4,1,bmp) ;

	int i,q ;
		
	switch ((colorCoding & 0xFFFF0000) >> 16) {
		
		case 8:
			//First read the pal
			fseek(bmp,54,SEEK_SET) ;
			for (i=0;i<256;i++){
					unsigned char r,g,b ;
					unsigned long color ;
					fread(&color,4,1,bmp) ;
					b = (color & 0x0FF) ;
					g = ((color >> 8) & 0x0FF) ;
					r = ((color >> 16) & 0x0FF) ;				
					pal[i]=RGB15(r>>3,g>>3,b>>3)| BIT(15);
			}
			buffer8 = new u8[width*height] ;
			for (i=0;i<height;i++){
				for (q=0;q<width;q++){
					u8 color;
					fread(&color,1,1,bmp) ;
					buffer8[q+(((height-1)-i)*width)]=color;
				}
			}
		break ;
	} ;
	
	fclose(bmp) ;
 
 	u8 buffLeft[32*64];
	u8 buffRight[32*64];
	u8 buffNorm[32*64];

	int x,y;
	//first we get the colors
	u16 red=RGB15(255>>3,0>>3,0>>3)| BIT(15);
	u16 blue=RGB15(0>>3,0>>3,255>>3)| BIT(15);
	u16 black=RGB15(0>>3,0>>3,0>>3)| BIT(15);
	
	//now we create 3 textures
	for(x=0;x<32;x++){
		for(y=0;y<64;y++){

			buffNorm[x+(y*32)]=buffer8[(x)+(y*64)];
			buffLeft[x+(y*32)]=0;
			buffRight[x+(y*32)]=0;
		
			if(pal[buffer8[(x+32)+(y*64)]]==red){
				buffLeft[x+(y*32)]=buffer8[(x)+(y*64)];
				buffRight[x+(y*32)]=0;
			}

			if(pal[buffer8[(x+32)+(y*64)]]==blue){
				buffLeft[x+(y*32)]=0;
				buffRight[x+(y*32)]=buffer8[(x)+(y*64)];
			}			
		}
	}
	
	free(buffer8);	
	
 
	WaitForFreeVblank();
    // TODO: Uncomment
	//targetp[num]= gluTexLoadPal( pal, 256, GL_RGB256 );

	glBindTexture (0, target[num]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, TEXTURE_SIZE_32, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD|(3<<29),(uint8*)buffNorm);

	glBindTexture (0, targetb[num]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, TEXTURE_SIZE_32, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD|(3<<29),(uint8*)buffLeft);

	glBindTexture (0, targetc[num]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, TEXTURE_SIZE_32, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD|(3<<29),(uint8*)buffRight);

}



void LoadMBump5Texture(char filename[],int num) 
{
	extern int BodenTexture[Ground_Count];
	extern int BodenTextureS[Ground_Count];
	extern int BodenTextureW[Ground_Count];
	extern int BodenTextureE[Ground_Count];
	extern int BodenPal[Ground_Count];
	extern u8 BodenSize[Ground_Count];

	u8* buffer8;
	u16 pal[256];
	int width=0;
	int height=0;
		
	FILE *bmp = fopen(filename,"r") ;
	if (bmp==NULL) {
		PrintOUT(filename,5,5,false,strlen(filename));
		PrintOUT("wasnt found",5,15,false,strlen("wasnt found"));
		while(!(keysDown() & KEY_A))scanKeys();
		ScreenModeLOADING();
		return;
	}
	
	fseek(bmp,18,SEEK_SET) ;

	fread(&width,4,1,bmp) ;
	fread(&height,4,1,bmp) ;
	
	if(width!=height*2 || (height!=32 && height!=64 )){
		char bug[40];
		PrintOUT(filename,5,5,false,strlen(filename));
		PrintOUT("Texture isnt 64x32 or 128x64",5,15,false,strlen("Texture isnt 64x32 or 128x64"));
		sprintf(bug,"its %dx%d",width,height);
		PrintOUT(bug,5,25,false,strlen(bug));	
		while(!(keysDown() & KEY_A))scanKeys();
		ScreenModeLOADING();
		return;	
	}
	
	unsigned long colorCoding ;
	fread(&colorCoding,4,1,bmp) ;

	if(((colorCoding & 0xFFFF0000) >> 16)!=8){
		char bug[40];
		PrintOUT(filename,5,5,false,strlen(filename));
		PrintOUT("Texture isnt 8bit...",5,15,false,strlen("Texture isnt 8bit..."));
		sprintf(bug,"its %dBit",int((colorCoding & 0xFFFF0000) >> 16));
		PrintOUT(bug,5,25,false,strlen(bug));	
		while(!(keysDown() & KEY_A))scanKeys();
		ScreenModeLOADING();
		return;		
	}
	
	fseek(bmp,34,SEEK_SET) ;
	unsigned long dataLength ;
	fread(&dataLength,4,1,bmp) ;

	fseek(bmp,46,SEEK_SET) ;
	unsigned long impcol ;
	fread(&impcol,4,1,bmp) ;

	int i,q ;
		
	switch ((colorCoding & 0xFFFF0000) >> 16) {
		
		case 8:
			//First read the pal
			fseek(bmp,54,SEEK_SET) ;
			for (i=0;i<256;i++){
					unsigned char r,g,b ;
					unsigned long color ;
					fread(&color,4,1,bmp) ;
					b = (color & 0x0FF) ;
					g = ((color >> 8) & 0x0FF) ;
					r = ((color >> 16) & 0x0FF) ;				
					pal[i]=RGB15(r>>3,g>>3,b>>3)| BIT(15);
			}
			buffer8 = new u8[width*height] ;
			for (i=0;i<height;i++){
				for (q=0;q<width;q++){
					u8 color;
					fread(&color,1,1,bmp) ;
					buffer8[q+(((height-1)-i)*width)]=color;
				}
			}
		break ;
	} ;
	
	fclose(bmp) ;
 
 	u8* buffNorm= new u8[height*height];
 	u8* buffLeft= new u8[height*height];
	u8* buffRight= new u8[height*height];
	u8* buffDown= new u8[height*height];

	int x,y;
	//first we get the colors
	u16 red=RGB15(255>>3,0>>3,0>>3)| BIT(15);
	u16 blue=RGB15(0>>3,0>>3,255>>3)| BIT(15);
	u16 green=RGB15(0>>3,255>>3,0>>3)| BIT(15);
	
	//now we create 5 textures
	for(x=0;x<height;x++){
		for(y=0;y<height;y++){
		
			buffNorm[x+(y*height)]=buffer8[(x)+(y*width)];
			buffLeft[x+(y*height)]=0;
			buffRight[x+(y*height)]=0;
			buffDown[x+(y*height)]=0;
		
			if(pal[buffer8[(x+height)+(y*width)]]==red){
				buffRight[x+(y*height)]=buffer8[(x)+(y*width)];
			}

			if(pal[buffer8[(x+height)+(y*width)]]==blue){
				buffLeft[x+(y*height)]=buffer8[(x)+(y*width)];
			}

			if(pal[buffer8[(x+height)+(y*width)]]==green){
				buffDown[x+(y*height)]=buffer8[(x)+(y*width)];
			}

		}
	}
	
	free(buffer8);	
	
	BodenSize[num]=height;
	
	WaitForFreeVblank();
    // TODO: Uncomment
	//BodenPal[num]= gluTexLoadPal( pal, 256, GL_RGB256 );
		
	glBindTexture (0, BodenTexture[num]);
	if(height==32)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0, TEXGEN_TEXCOORD|(3<<29),(uint8*)buffNorm);
	if(height==64)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD|(3<<29),(uint8*)buffNorm);

	glBindTexture (0, BodenTextureS[num]);
	if(height==32)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0, TEXGEN_TEXCOORD|(3<<29),(uint8*)buffDown);
	if(height==64)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD|(3<<29),(uint8*)buffDown);

	glBindTexture (0, BodenTextureW[num]);
	if(height==32)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0, TEXGEN_TEXCOORD|(3<<29),(uint8*)buffLeft);
	if(height==64)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD|(3<<29),(uint8*)buffLeft);

	glBindTexture (0, BodenTextureE[num]);
	if(height==32)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0, TEXGEN_TEXCOORD|(3<<29),(uint8*)buffRight);
	if(height==64)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, TEXTURE_SIZE_64, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD|(3<<29),(uint8*)buffRight);

}
