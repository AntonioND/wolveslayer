#include "../3D.h"

extern u8 EnvR,EnvG,EnvB;
extern int Blend;

extern int WallX,WallY;
extern bool walltrans;

extern int Door[1],DoorPal[1];

extern t16 walltex1;
extern t16 walltex1e;

extern t16 walltex2;
extern t16 walltex2e;

extern v16 mode0vertex;
extern v16 mode1vertex;
extern v16 mode0vertex2;
extern v16 mode1vertex2;


t16 overdoor=inttot16(17);
t16 ldoor=inttot16(13);
t16 rdoor=inttot16(21);

int newcol1[3];//for interpolated lights...
int newcol2[3];//stored to recycle

char DoorSpeech[25][10][256];//to hold 10 textes for each ddor with 256 chars
int DoorSpeechCount[25];//Holds the Number of speeches a Door has...

void AddTexttoDoor(char txt[256]){
	extern int MapChangeCounter;
	if(DoorSpeechCount[MapChangeCounter-1]<10){
		strcpy(DoorSpeech[MapChangeCounter-1][DoorSpeechCount[MapChangeCounter-1]],txt);
		DoorSpeechCount[MapChangeCounter-1]++;
	}
}


void LoadDoorTexture(char filename[]) 
{
	extern int Door[1],DoorPal[1];
	u8* buffer8 = NULL;
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

	if(width!=32 || height!=64){
		char bug[40];
		PrintOUT(filename,5,5,false,strlen(filename));
		PrintOUT("Texture isnt 32x64...",5,15,false,strlen("Texture isnt 32x64..."));
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
 
    // TODO: Uncomment
	//DoorPal[0]= gluTexLoadPal( pal, 256, GL_RGB256 );
	WaitForFreeVblank();
	glBindTexture (0, Door[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, TEXTURE_SIZE_32, TEXTURE_SIZE_64, 0, TEXGEN_TEXCOORD|(3<<29),(uint8*)buffer8);
	free(buffer8);
}

void RenderDoorOutside(int mode,float angle,f32 x, f32 y, f32 z,bool trans){
	if(mode!=0 && mode!=1)return;

	if(trans)glPolyFmt(POLY_ALPHA(7) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0|POLY_ID(3));
	else glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0|POLY_ID(3));

	glBindTexture (GL_TEXTURE_2D, Door[0]);
    // TODO: Uncomment
	//glColorTable(GL_RGB256, DoorPal[0]);		
	int alpha=7;
	
	if(!walltrans || mode==1)alpha=31;
	v16 vert,vert2;
	if(mode==0){vert=mode0vertex;vert2=floattov16(.25);glPolyFmt(POLY_ALPHA(alpha) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 |POLY_ID(1));}
	if(mode==1){vert=mode1vertex;vert2=floattov16(-.25);glPolyFmt(POLY_ALPHA(alpha) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0| POLY_ID(0));}

	glPushMatrix ();
	glTranslatef32 (x, y, z); 
	glRotatef(angle,0,1,0);
	u8 r=EnvR,g=EnvG,b=EnvB;
	if(Blend!=8){
		r=EnvR/8*Blend;
		g=EnvG/8*Blend;
		b=EnvB/8*Blend;
	}
	glColor3b(r,g,b);
	

		//Wand links
		glBegin (GL_QUADS); 
			// Bottom left
			glColor3b((u8)newcol1[0],(u8)newcol1[1],(u8)newcol1[2]);
			glTexCoord2t16 (walltex1,walltex2e);
			glVertex3v16 (floattov16(0), vert, floattov16(0));
			// Top left
			glTexCoord2t16 (walltex1,walltex1);
			glVertex3v16 (floattov16(0), vert2, floattov16(0));
			// Top right
			glColor3b((u8)newcol2[0],(u8)newcol2[1],(u8)newcol2[2]);
			glTexCoord2t16 (walltex1e,walltex1);
			glVertex3v16 (floattov16(.625), vert2, floattov16(0));
			// Bottom right
			glTexCoord2t16 (walltex1e,walltex2e);
			glVertex3v16 (floattov16(.625), vert, floattov16(0));
		glEnd ();


	glPopMatrix(1);

}
//____________________________________________________________________________________________

void RenderHouseBorderDoor (int mode,int doorangle,int dirrection ,f32 x, f32 y, f32 z)
{	
	if(mode!=0 && mode!=1)return;
	bool trans=true;
	
	if(walltrans && mode!=1)glPolyFmt(POLY_ALPHA(7) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0|POLY_ID(3));
	else {glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0|POLY_ID(3));trans=false;}

	v16 vert,vert2,vert3;
	if(mode==0){vert=mode0vertex;vert2=mode0vertex2;vert3=floattov16(.25);}
	if(mode==1){vert=mode1vertex;vert2=mode1vertex2;vert3=floattov16(-.25);}

	glPushMatrix ();

	glTranslatef32 (x, y, z); 
	glRotateYi(dirrection*-128);
	u8 r=EnvR,g=EnvG,b=EnvB;

	int LightX1=0,LightY1=0;
	int LightX2=0,LightY2=0;

	u8 col1[3];
	u8 col2[3];

	if(dirrection==0){
		LightX1=WallX;LightY1=WallY+1;
		LightX2=WallX+1;LightY2=WallY+1;
	}
	
	if(dirrection==1){
		LightX1=WallX;LightY1=WallY;
		LightX2=WallX;LightY2=WallY+1;
	}	
	
	if(dirrection==2){
		LightX1=WallX+1;LightY1=WallY;
		LightX2=WallX;LightY2=WallY;
	}
	
	if(dirrection==3){
		LightX1=WallX+1;LightY1=WallY+1;
		LightX2=WallX+1;LightY2=WallY;
	}
	
	//here comes that interpolation
	
	GiveLight(LightX1,LightY1,col1);
	GiveLight(LightX2,LightY2,col2);
	
	newcol1[0]=f32toint(mulf32(floattov16(.625),inttof32(col1[0]))+mulf32(floattov16(1-.625),inttof32(col2[0])));
	newcol1[1]=f32toint(mulf32(floattov16(.625),inttof32(col1[1]))+mulf32(floattov16(1-.625),inttof32(col2[1])));
	newcol1[2]=f32toint(mulf32(floattov16(.625),inttof32(col1[2]))+mulf32(floattov16(1-.625),inttof32(col2[2])));
	if(newcol1[0]>255)newcol1[0]=255;
	if(newcol1[1]>255)newcol1[1]=255;
	if(newcol1[2]>255)newcol1[2]=255;
	newcol2[0]=f32toint(mulf32(floattov16(.625),inttof32(col2[0]))+mulf32(floattov16(1-.625),inttof32(col1[0])));
	newcol2[1]=f32toint(mulf32(floattov16(.625),inttof32(col2[1]))+mulf32(floattov16(1-.625),inttof32(col1[1])));
	newcol2[2]=f32toint(mulf32(floattov16(.625),inttof32(col2[2]))+mulf32(floattov16(1-.625),inttof32(col1[2])));
	if(newcol2[0]>255)newcol2[0]=255;
	if(newcol2[1]>255)newcol2[1]=255;
	if(newcol2[2]>255)newcol2[2]=255;	
	
		//Wand Oben
		glBegin (GL_QUADS); 
			// Bottom left
			GrapLight(LightX1,LightY1);
			glTexCoord2t16 (overdoor, walltex1);
			glVertex3v16 (floattov16(-.5), vert3, floattov16(.5));
			// Top left
			glTexCoord2t16 (walltex1, walltex1);
			glVertex3v16 (floattov16(-.5), vert2, floattov16(.5));
			// Top right
			GrapLight(LightX2,LightY2);
			glTexCoord2t16 (walltex1, walltex1e);
			glVertex3v16 (floattov16(.5), vert2, floattov16(.5));
			// Bottom right
			glTexCoord2t16 (overdoor, walltex1e);
			glVertex3v16 (floattov16(.5), vert3, floattov16(.5));
		glEnd ();
	
		//Wand links
		glBegin (GL_QUADS); 
			// Bottom left
			GrapLight(LightX1,LightY1);
			glTexCoord2t16 (walltex2e, walltex1);
			glVertex3v16 (floattov16(-.5), vert, floattov16(.5));
			// Top left
			glTexCoord2t16 (overdoor, walltex1);
			glVertex3v16 (floattov16(-.5), vert3, floattov16(.5));
			// Top right
			glColor3b((u8)newcol1[0],(u8)newcol1[1],(u8)newcol1[2]);
			glTexCoord2t16 (overdoor, ldoor);
			glVertex3v16 (floattov16(-.3125), vert3, floattov16(.5));
			// Bottom right
			glTexCoord2t16 (walltex2e, ldoor);
			glVertex3v16 (floattov16(-.3125), vert, floattov16(.5));
		glEnd ();

		//Wand rechts
		glBegin (GL_QUADS); 
			// Bottom left
			glColor3b((u8)newcol2[0],(u8)newcol2[1],(u8)newcol2[2]);
			glTexCoord2t16 (walltex2e, rdoor);
			glVertex3v16 (floattov16(.3125), vert, floattov16(.5));
			// Top left
			glTexCoord2t16 (overdoor, rdoor);
			glVertex3v16 (floattov16(.3125), vert3, floattov16(.5));
			// Top right
			GrapLight(LightX2,LightY2);
			glTexCoord2t16 (overdoor, walltex1e);
			glVertex3v16 (floattov16(.5), vert3, floattov16(.5));
			// Bottom right
			glTexCoord2t16 (walltex2e, walltex1e);
			glVertex3v16 (floattov16(.5), vert, floattov16(.5));
		glEnd ();
		
		//Dach Unten
	if(mode==0){
		if(!walltrans){
			if(Blend!=8){
				r=EnvR/8*Blend;
				g=EnvG/8*Blend;
				b=EnvB/8*Blend;
			}
			glColor3b(r,g,b);
			glBegin (GL_QUADS); 
					// Bottom left
					glTexCoord2t16 (walltex1, walltex2);
					glVertex3v16 (floattov16(-.5), floattov16(1.3), floattov16(-.5));
			
					// Top left
					glTexCoord2t16 (walltex2, walltex2);
					glVertex3v16 (floattov16(-.5), floattov16(.8), floattov16(.5));
			
					// Top right
					glTexCoord2t16 (walltex2, walltex2e);
					glVertex3v16 (floattov16(.5), floattov16(.8), floattov16(.5));
			
					// Bottom right
					glTexCoord2t16 (walltex1, walltex2e);
					glVertex3v16 (floattov16(.5), floattov16(1.3), floattov16(-.5));
			glEnd ();
		}
	}

	glPopMatrix(1);
	if(dirrection==0)RenderDoorOutside(mode,doorangle*-1,x-floattov16(.3125),y,z+floattov16(.5),trans);
	if(dirrection==2)RenderDoorOutside(mode,180+doorangle*-1,x+floattov16(.3125),y,z-floattov16(.5),trans);
	if(dirrection==1)RenderDoorOutside(mode,90+doorangle*-1,x-floattov16(.5),y,z-floattov16(.3125),trans);
	if(dirrection==3)RenderDoorOutside(mode,-90+doorangle*-1,x+floattov16(.5),y,z+floattov16(.3125),trans);
}

void RenderWallBorderDoor (int mode,int doorangle,int dirrection ,f32 x, f32 y, f32 z)
{	
	if(mode!=0 && mode!=1)return;
	
	bool trans=true;
	
	if(walltrans && dirrection==2 && mode!=1)glPolyFmt(POLY_ALPHA(7) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0|POLY_ID(3));
	else {glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0|POLY_ID(3));trans=false;}

	v16 vert,vert2,vert3;
	if(mode==0){vert=mode0vertex;vert2=mode0vertex2;vert3=floattov16(.25);}
	if(mode==1){vert=mode1vertex;vert2=mode1vertex2;vert3=floattov16(-.25);}


	u8 col1[3];
	u8 col2[3];

	int LightX1=0,LightY1=0;
	int LightX2=0,LightY2=0;
	
	if(dirrection==0){
		LightX1=WallX;LightY1=WallY+1;
		LightX2=WallX+1;LightY2=WallY+1;
	}
	
	if(dirrection==1){
		LightX1=WallX;LightY1=WallY;
		LightX2=WallX;LightY2=WallY+1;
	}	

	if(dirrection==2){
		LightX1=WallX+1;LightY1=WallY;
		LightX2=WallX;LightY2=WallY;
	}
		
	if(dirrection==3){
		LightX1=WallX+1;LightY1=WallY+1;
		LightX2=WallX+1;LightY2=WallY;
	}

		GiveLight(LightX1,LightY1,col1);
		GiveLight(LightX2,LightY2,col2);
		
		newcol1[0]=f32toint(mulf32(floattov16(.625),inttof32(col1[0]))+mulf32(floattov16(1-.625),inttof32(col2[0])));
		newcol1[1]=f32toint(mulf32(floattov16(.625),inttof32(col1[1]))+mulf32(floattov16(1-.625),inttof32(col2[1])));
		newcol1[2]=f32toint(mulf32(floattov16(.625),inttof32(col1[2]))+mulf32(floattov16(1-.625),inttof32(col2[2])));
		if(newcol1[0]>255)newcol1[0]=255;
		if(newcol1[1]>255)newcol1[1]=255;
		if(newcol1[2]>255)newcol1[2]=255;
		newcol2[0]=f32toint(mulf32(floattov16(.625),inttof32(col2[0]))+mulf32(floattov16(1-.625),inttof32(col1[0])));
		newcol2[1]=f32toint(mulf32(floattov16(.625),inttof32(col2[1]))+mulf32(floattov16(1-.625),inttof32(col1[1])));
		newcol2[2]=f32toint(mulf32(floattov16(.625),inttof32(col2[2]))+mulf32(floattov16(1-.625),inttof32(col1[2])));
		if(newcol2[0]>255)newcol2[0]=255;
		if(newcol2[1]>255)newcol2[1]=255;
		if(newcol2[2]>255)newcol2[2]=255;	

	if(dirrection!=2){	
	
		glPushMatrix ();

		glTranslatef32 (x, y, z); 
		glRotateYi(dirrection*-128);	
			//Wand Oben
			glBegin (GL_QUADS); 
				// Bottom left
				GrapLight(LightX1,LightY1);
				glTexCoord2t16 (overdoor, walltex1);
				glVertex3v16 (floattov16(-.5), vert3, floattov16(.5));
				// Top left
				glTexCoord2t16 (walltex1, walltex1);
				glVertex3v16 (floattov16(-.5), vert2, floattov16(.5));
				// Top right
				GrapLight(LightX2,LightY2);
				glTexCoord2t16 (walltex1, walltex1e);
				glVertex3v16 (floattov16(.5), vert2, floattov16(.5));
				// Bottom right
				glTexCoord2t16 (overdoor, walltex1e);
				glVertex3v16 (floattov16(.5), vert3, floattov16(.5));
			glEnd ();
		
			//Wand links
			glBegin (GL_QUADS); 
				// Bottom left
				GrapLight(LightX1,LightY1);
				glTexCoord2t16 (walltex2e, walltex1);
				glVertex3v16 (floattov16(-.5), vert, floattov16(.5));
				// Top left
				glTexCoord2t16 (overdoor, walltex1);
				glVertex3v16 (floattov16(-.5), vert3, floattov16(.5));
				// Top right
				glColor3b((u8)newcol1[0],(u8)newcol1[1],(u8)newcol1[2]);
				glTexCoord2t16 (overdoor, ldoor);
				glVertex3v16 (floattov16(-.3125), vert3, floattov16(.5));
				// Bottom right
				glTexCoord2t16 (walltex2e, ldoor);
				glVertex3v16 (floattov16(-.3125), vert, floattov16(.5));
			glEnd ();
	
			//Wand rechts
			glBegin (GL_QUADS); 
				// Bottom left
				glColor3b((u8)newcol2[0],(u8)newcol2[1],(u8)newcol2[2]);
				glTexCoord2t16 (walltex2e, rdoor);
				glVertex3v16 (floattov16(.3125), vert, floattov16(.5));
				// Top left
				glTexCoord2t16 (overdoor, rdoor);
				glVertex3v16 (floattov16(.3125), vert3, floattov16(.5));
				// Top right
				GrapLight(LightX2,LightY2);
				glTexCoord2t16 (overdoor, walltex1e);
				glVertex3v16 (floattov16(.5), vert3, floattov16(.5));
				// Bottom right
				glTexCoord2t16 (walltex2e, walltex1e);
				glVertex3v16 (floattov16(.5), vert, floattov16(.5));
			glEnd ();
		
		glPopMatrix(1);
	}

	if(dirrection==0)RenderDoorOutside(mode,doorangle*-1,x-floattov16(.3125),y,z+floattov16(.5),trans);
	if(dirrection==2)RenderDoorOutside(mode,180+doorangle*-1,x+floattov16(.3125),y,z-floattov16(.5),trans);
	if(dirrection==1)RenderDoorOutside(mode,90+doorangle*-1,x-floattov16(.5),y,z-floattov16(.3125),trans);
	if(dirrection==3)RenderDoorOutside(mode,-90+doorangle*-1,x+floattov16(.5),y,z+floattov16(.3125),trans);
}



