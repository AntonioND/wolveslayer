#include "../3D.h"

typedef struct{
	v16 v[4];
	u8 sidewalls;
}v16x4;

//extern u16 BodenArray[128*256];
extern int BodenX,BodenY;

void LoadAutotileTexture(char filename[],int num) 
{
	extern int AutotileTextur[4][3];
	u8* buffer8 = NULL;
	u8* part;
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

	if(width!=384 || height!=128){
		char bug[40];
		PrintOUT(filename,5,5,false,strlen(filename));
		PrintOUT("Texture isnt 384x128...",5,15,false,strlen("Texture isnt 384x128..."));
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
		sprintf(bug,"its %luBit",((colorCoding & 0xFFFF0000) >> 16));
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
					//BG_GFX[i]= RGB15(r>>3,g>>3,b>>3)| BIT(15);
					//BG_GFX[i+256]= RGB15(r>>3,g>>3,b>>3)| BIT(15);
					pal[i]=RGB15(r>>3,g>>3,b>>3)| BIT(15);
			}
			buffer8 = (u8*)malloc(width*height);
			for (i=0;i<height;i++){
				for (q=0;q<width;q++){
					u8 color;
					fread(&color,1,1,bmp) ;
					//BG_GFX[q+(((height-1)-i)*256)]=pal[color];
					buffer8[q+(((height-1)-i)*width)]=color;
				}
				unsigned long abuf ;
				if ((width) & 1)
					fread(&abuf,4-((width) & 1),1,bmp) ;
			}
		break ;
		default:
			fprintf(stderr, "Invalid BMP format: %s", filename);
			while (1);
		break;
	} ;
	
	fclose(bmp) ;
	swiWaitForVBlank();
	

	part = (u8*)malloc(128*129);
	int ax,bx,by;

	int palTextureName = -1;

	//lets load 16 first 
	for (ax=0;ax<3;ax++){//all 16 parts of 128x128 tile thing
		for (bx=0;bx<128;bx++)for(by=0;by<128;by++)//each pixel
			part[bx+(by*128)]=buffer8[(bx+(ax*128))+(by*384)];

		glGenTextures(0, &AutotileTextur[num][ax]);
		glBindTexture(0, AutotileTextur[num][ax]);

		WaitForFreeVblank();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, 128, 128, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, part);

		if (palTextureName == -1)
		{
			//OK here we now have loaded pal and we got 36 fields a 32x32
			glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);
			palTextureName = AutotileTextur[num][ax];
		}
		else
		{
			glAssignColorTable(0, palTextureName);
		}
	}

	free(buffer8);
	free(part);	
}//The way a Bodentexture should be loaded

void AddAutIgnore(int Tilenum, u32 Col){
	extern u32 AutotileIgnorecolors[10][4];
	extern int AutotileIgnorecolorsNum[4];

	AutotileIgnorecolorsNum[Tilenum]++;
	AutotileIgnorecolors[AutotileIgnorecolorsNum[Tilenum]][Tilenum]=Col;
}

void RenderAutoBoden( int x, int y, int z,int num)
{
	int cx=0,cy=0;
	extern v16x4 Terrain[128][128];
	v16x4 pointer=Terrain[BodenX][BodenY];

	extern t16 BodenTextPosStart[4];
	extern t16 BodenTextPosEnd[4];
	extern v16 BodenVertex1;
	extern v16 BodenVertex2;

	if(num==0){cx=0;cy=0;}
	if(num==1){cx=1;cy=0;}
	if(num==2){cx=2;cy=0;}
	if(num==3){cx=3;cy=0;}
	
	if(num==4){cx=0;cy=1;}
	if(num==5){cx=1;cy=1;}
	if(num==6){cx=2;cy=1;}
	if(num==7){cx=3;cy=1;}

	if(num==8){cx=0;cy=2;}
	if(num==9){cx=1;cy=2;}
	if(num==10){cx=2;cy=2;}
	if(num==11){cx=3;cy=2;}

	if(num==12){cx=0;cy=3;}
	if(num==13){cx=1;cy=3;}
	if(num==14){cx=2;cy=3;}
	if(num==15){cx=3;cy=3;}
	
	glPushMatrix ();
	glTranslatef32 (inttof32(x), inttof32(0), inttof32(z)); 


	int mode=(pointer.sidewalls>>6)& 3;
	
	if(mode==0){	
		glBegin (GL_QUADS); 
			// Bottom left
			GrapLight(BodenX,BodenY);
			glTexCoord2t16 ( BodenTextPosStart[cx],BodenTextPosStart[cy]);
			glVertex3v16 (BodenVertex2, pointer.v[0], BodenVertex2);
			
			// Top left
			GrapLight(BodenX,BodenY+1);
			glTexCoord2t16 ( BodenTextPosStart[cx],BodenTextPosEnd[cy]);
			glVertex3v16 (BodenVertex2, pointer.v[2], BodenVertex1);
	
			// Top right
			GrapLight(BodenX+1,BodenY+1);
			glTexCoord2t16 (BodenTextPosEnd[cx],BodenTextPosEnd[cy]);
			glVertex3v16 (BodenVertex1, pointer.v[3], BodenVertex1);
	
			// Bottom right
			GrapLight(BodenX+1,BodenY);
			glTexCoord2t16 (BodenTextPosEnd[cx],BodenTextPosStart[cy]);
			glVertex3v16 (BodenVertex1, pointer.v[1], BodenVertex2);
		glEnd ();
	}
	
	if(mode==1){
		glBegin (GL_TRIANGLES); 
			// Bottom left
			GrapLight(BodenX,BodenY);
			glTexCoord2t16 (BodenTextPosStart[cx],BodenTextPosStart[cy]);
			glVertex3v16 (BodenVertex2, pointer.v[0], BodenVertex2);
			
			// Top left
			GrapLight(BodenX,BodenY+1);
			glTexCoord2t16 (BodenTextPosStart[cx],BodenTextPosEnd[cy]);
			glVertex3v16 (BodenVertex2, pointer.v[2], BodenVertex1);
	
			// Top right
			GrapLight(BodenX+1,BodenY+1);
			glTexCoord2t16 (BodenTextPosEnd[cx],BodenTextPosEnd[cy]);
			glVertex3v16 (BodenVertex1, pointer.v[3], BodenVertex1);
	//***********************
			// Top right
			GrapLight(BodenX+1,BodenY+1);
			glTexCoord2t16 (BodenTextPosEnd[cx],BodenTextPosEnd[cy]);
			glVertex3v16 (BodenVertex1, pointer.v[3], BodenVertex1);
	
			// Bottom right
			GrapLight(BodenX+1,BodenY);
			glTexCoord2t16 (BodenTextPosEnd[cx],BodenTextPosStart[cy]);
			glVertex3v16 (BodenVertex1, pointer.v[1], BodenVertex2);

			// Bottom left
			GrapLight(BodenX,BodenY);
			glTexCoord2t16 (BodenTextPosStart[cx],BodenTextPosStart[cy]);
			glVertex3v16 (BodenVertex2, pointer.v[0], BodenVertex2);
		glEnd ();
	}

	if(mode==2){
		glBegin (GL_TRIANGLES); 
			// Bottom left
			GrapLight(BodenX,BodenY);
			glTexCoord2t16 (BodenTextPosStart[cx],BodenTextPosStart[cy]);
			glVertex3v16 (BodenVertex2, pointer.v[0], BodenVertex2);
			
			// Top left
			GrapLight(BodenX,BodenY+1);
			glTexCoord2t16 (BodenTextPosStart[cx],BodenTextPosEnd[cy]);
			glVertex3v16 (BodenVertex2, pointer.v[2], BodenVertex1);
	
			// Bottom right
			GrapLight(BodenX+1,BodenY);
			glTexCoord2t16 (BodenTextPosEnd[cx],BodenTextPosStart[cy]);
			glVertex3v16 (BodenVertex1, pointer.v[1], BodenVertex2);
	//***********************
			// Top right
			GrapLight(BodenX+1,BodenY+1);
			glTexCoord2t16 (BodenTextPosEnd[cx],BodenTextPosEnd[cy]);
			glVertex3v16 (BodenVertex1, pointer.v[3], BodenVertex1);
	
			// Bottom right
			GrapLight(BodenX+1,BodenY);
			glTexCoord2t16 (BodenTextPosEnd[cx],BodenTextPosStart[cy]);
			glVertex3v16 (BodenVertex1, pointer.v[1], BodenVertex2);

			// Top left
			GrapLight(BodenX,BodenY+1);
			glTexCoord2t16 (BodenTextPosStart[cx],BodenTextPosEnd[cy]);
			glVertex3v16 (BodenVertex2, pointer.v[2], BodenVertex1);
		glEnd ();
	}	
	
	glPopMatrix(1);
}//The way a bodentexture should be rendered

