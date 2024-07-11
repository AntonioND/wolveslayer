#include "3D.h"

// TODO: Remove this
#include <nds/registers_alt.h>

void vblank_handler (void);
void SetMainBg(unsigned short int *pic);
void E3D_Init(void);
void E3D_StartRender(int mode);

//*******************Globale variablen
t16 BodenTextPosStart[4];
t16 BodenTextPosEnd[4];
v16 BodenVertex1=floattov16(.508);
v16 BodenVertex2=floattov16(-.508);

int BodenTexture[Ground_Count];
//int BodenTextureN[Ground_Count];
int BodenTextureS[Ground_Count];
int BodenTextureW[Ground_Count];
int BodenTextureE[Ground_Count];

int BodenPal[Ground_Count];
bool BodenEnable[Ground_Count];
u32 BodenColorKey[Ground_Count];
bool BodenTransEnable[Ground_Count];
bool BodenBumpEnable[Ground_Count];
u8 BodenSize[Ground_Count];

int AutotileTextur[4][3];
int AutotilePal[4];
u32 AutotileColorKeyMaster[4];
u32 AutotileIgnorecolors[10][4];
int AutotileIgnorecolorsNum[4];

int Wasser[1];
int WasserPal[1];
u32 WasserKey;

int Ufer[10];
int UferB[10];
int UferC[10];
int UferPal[10];
bool UferBump[10];

int Figuren[10];
int FigurenPal[10];

//26 textureslots for objects
int ObjektTex[Object_Count];//Standard slot for objects
int ObjektTexB[Object_Count];//Advanced slot for objects(bumpmapping)
int ObjektTexC[Object_Count];//Advanced slot for objects(bumpmapping)

int ObjectTextureID[Object_Count];

//and 26 objects...but u can map textures twice....
char ObjektTyp[Object_Count][13];
u32 ObjektColorKey[Object_Count];
int ObjektPal[Object_Count];
float ObjectRadius[Object_Count];
bool ObjectMirrow[Object_Count];
bool ObjectIllumination[Object_Count];
bool ObjectNoGround[Object_Count];
bool ObjectCulling[Object_Count];

int Door[1],DoorPal[1];

int Waffe[1],WaffePal[1];

u16 scrL_bin[256*192];
u16 touch_bin[256*192];
u16 textbox_pal[256];
u8 Font1[668*11];

void LoadBmptoBuffer(char* filename,u16* picbuff){
	u8* buffer8;
	u16 pal[256];
	int width=0;
	int height=0;
		
	FILE *bmp = fopen(filename,"r") ;
	if (bmp==NULL) {
		fprintf(stderr,"%s not found \n",filename);
		while(1){} ;
	} ;
	
	fseek(bmp,18,SEEK_SET) ;

	fread(&width,4,1,bmp) ;
	fread(&height,4,1,bmp) ;
	
	unsigned long colorCoding ;
	fread(&colorCoding,4,1,bmp) ;
	
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
			buffer8 = new u8[width*height] ;
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
	} ;
	
	fclose(bmp) ;

	swiWaitForVBlank();
	for(int i = 0; i < width*height; i++)
	picbuff[i] = pal[buffer8[i]]; 
}

void LoadBmptoBuffer8(char* filename,u8* picbuff,u16* palbuff){
	u8* buffer8;
	u16 pal[256];
	int width=0;
	int height=0;
		
	FILE *bmp = fopen(filename,"r") ;
	if (bmp==NULL) {
		fprintf(stderr,"%s not found \n",filename);
		while(1){} ;
	} ;
	
	fseek(bmp,18,SEEK_SET) ;

	fread(&width,4,1,bmp) ;
	fread(&height,4,1,bmp) ;
	
	unsigned long colorCoding ;
	fread(&colorCoding,4,1,bmp) ;
	
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
			buffer8 = new u8[width*height] ;
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
	} ;
	
	fclose(bmp) ;

	swiWaitForVBlank();
	for(i = 0; i < width*height; i++)picbuff[i] = buffer8[i]; 
	for(i = 0; i < 256; i++)palbuff[i]=pal[i];
}


void Splash(void){
	srand(0xDEADBEEF);

	InitSound();
	StartSong("/wolveslayer/bgfx/Title.mod");

 	vramSetBankA (VRAM_A_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);
	
	videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
	lcdSwap();
	BLEND_CR = BLEND_FADE_BLACK | BLEND_SRC_BG3 | BLEND_DST_BACKDROP; 
	BG3_CR = BG_BMP16_256x256;
    BG3_XDX = 1 << 8;
    BG3_XDY = 0;
    BG3_YDX = 0;
    BG3_YDY = 1 << 8;
    BG3_CX = 0;
    BG3_CY = 0 << 8;

	videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);

	SUB_BLEND_CR = BLEND_FADE_BLACK | BLEND_SRC_BG3 | BLEND_DST_BACKDROP; 	
	SUB_BG3_CR = BG_BMP16_256x256;
    SUB_BG3_XDX = 1 << 8;
    SUB_BG3_XDY = 0;
    SUB_BG3_YDX = 0;
    SUB_BG3_YDY = 1 << 8;
    SUB_BG3_CX = 0;
    SUB_BG3_CY = 0 << 8;

	BLEND_Y = 15;
	SUB_BLEND_Y = 15;
	LoadBmptoBuffer("/wolveslayer/pic/scr1.bmp",BG_GFX);
	LoadBmptoBuffer("/wolveslayer/pic/scr2.bmp",BG_GFX_SUB);


	for(int u=15;u>=0;u--){	
		BLEND_Y = u;
		SUB_BLEND_Y = u;
		swiWaitForVBlank();
	}
	InitCircles();
	
	LoadBmptoBuffer("/wolveslayer/pic/loading.bmp",scrL_bin);
	LoadBmptoBuffer("/wolveslayer/pic/touch.bmp",touch_bin);
	LoadBmptoBuffer8("/wolveslayer/pic/font1.bmp",Font1,textbox_pal);

//	LoadBmptoBuffer8("/rd/pic/box.bmp",textbox_bin,textbox_pal);
	
	int pressed;
	for(int u=0;u<360;u++){
		scanKeys();
		pressed = keysDown();
		swiWaitForVBlank();
		if((pressed & KEY_TOUCH)||(pressed & KEY_START))u=360;
	}
	
	for(int u=15;u>=0;u--){	
		BLEND_Y = 15-u;
		SUB_BLEND_Y = 15-u;
		swiWaitForVBlank();
	}
	
	LoadBmptoBuffer("/wolveslayer/pic/scr3.bmp",BG_GFX);
	LoadBmptoBuffer("/wolveslayer/pic/scr4.bmp",BG_GFX_SUB);

	for(int u=15;u>=0;u--){	
		BLEND_Y = u;
		SUB_BLEND_Y = u;
		swiWaitForVBlank();
	}
	
	scanKeys();
	pressed = keysDown();

	while( !((pressed & KEY_START) || (pressed & KEY_TOUCH)) ){
		swiWaitForVBlank();
		scanKeys();
		pressed = keysDown();
	}
}

int frameCounter=0; 
int loopCounter=0; 
int elapsedFrames=0;
int frameold=0;
int polycount=0;
int capcount=0;

void vBlank(void){
	extern void DisplCapture(void);

	extern int screenmode;
	extern int frcapture;

	++elapsedFrames; 
	++frameCounter;
	
	if(frameCounter%2==1)tackt();
	
	if (frameCounter>=60){ 
		frameold=loopCounter;
		frameCounter=0; 
		loopCounter=0;
	}
	
	if(screenmode==2 && frameCounter%2==1 && (keysHeld() & KEY_A))TextBoxmodeHandler();
	if(screenmode==2 && frameCounter%6==1 && !(keysHeld() & KEY_A))TextBoxmodeHandler();
		
	if(screenmode!=2 && frameCounter%59==0 && (keysHeld() & KEY_R)){
		extern float PlPosX;
		extern float PlPosY;
		int px,py;
		px=PlPosX;
		py=PlPosY;
		
		char Tmp[20];
		
		#ifdef ShowPosition
		sprintf(Tmp,"PX: %d  PY: %d  ",px+6,py+8);
		Print(Tmp,80,105);
		#endif
		
		sprintf(Tmp,"Frames   : %d  ",frameold);//we print it all 15 vblanks so 4 times per second
		Print(Tmp,80,115);
		
		#ifdef ShowPolyCount
		sprintf(Tmp,"Polys   : %d  ",polycount);//we print it all 15 vblanks so 4 times per second
		Print(Tmp,80,125);
		#endif
	}

/*
	if((keysHeld() & KEY_L))DisplCapture();

	if(frcapture!=-1 && frameCounter==frcapture){
		frcapture=-1;
		vramSetBankC(VRAM_C_SUB_BG);
		capcount++;
		
		char filename[20];
		sprintf(filename,"/%d.raw",capcount);
		FILE *bmp = fopen(filename,"w") ;
		//if (bmp==NULL) {
		//	return;
		//} ;
		fwrite(BG_GFX_SUB,256*256*2,1,bmp) ;
		fclose(bmp) ;
	}
*/
}

void WaitForFreeVblank(void){
	swiWaitForVBlank();	
	if(frameCounter%59==0)swiWaitForVBlank();
}//important for multitask buffering and loading to vram 

void E3D_Init(void){
 	powerOn (POWER_ALL);
	InitFS();	
	Splash();


	// Set mode 0 and set it to 3D
	videoSetMode (MODE_0_3D);

	// Texture setup
	vramSetBankA (VRAM_A_TEXTURE);
	vramSetBankB (VRAM_B_TEXTURE);
	vramSetBankC(VRAM_C_SUB_BG);
	vramSetBankD (VRAM_D_TEXTURE);

	lcdSwap();	
	videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);
	
	SUB_BG3_CR = BG_BMP16_256x256;
    SUB_BG3_XDX = 1 << 8;
    SUB_BG3_XDY = 0;
    SUB_BG3_YDX = 0;
    SUB_BG3_YDY = 1 << 8;
    SUB_BG3_CX = 0;
    SUB_BG3_CY = 0 << 8;	
	
	ScreenMode();
   
	// OpenGL init
	irqSet(IRQ_VBLANK,vBlank);
	glInit();

	glViewport (0, 0, 255, 191);
	glClearColor(0,0,0,31); // BG must be opaque for AA to work
	glClearPolyID(63); // BG must have a unique polygon ID for AA to work
	glClearDepth(0x7FFF);

	glEnable(GL_TEXTURE);
	glEnable(GL_ANTIALIAS);
	glEnable(GL_BLEND);
	glAlphaFunc(BLEND_ALPHA); //could that blend out colors from palletes which doesnt have alphabit?
	
	//texturen Platzhalter
	glGenTextures (Ground_Count, &BodenTexture[0]);//Regular ground
	//glGenTextures (Ground_Count, &BodenTextureN[0]);//bump for northside of a ground
	glGenTextures (Ground_Count, &BodenTextureS[0]);//bump for southside of a ground
	glGenTextures (Ground_Count, &BodenTextureW[0]);//bump for westside(2PAC) of a ground
	glGenTextures (Ground_Count, &BodenTextureE[0]);//bump for eastside of a ground
	glGenTextures (10, &Figuren[0]);
	glGenTextures (Object_Count, &ObjektTex[0]);//Regular Obj
	glGenTextures (Object_Count, &ObjektTexB[0]);//Bumpmap for one side
	glGenTextures (Object_Count, &ObjektTexC[0]);//and the other one
	glGenTextures (1, &Wasser[0]);
	glGenTextures (1, &Door[0]);
	glGenTextures (3, &AutotileTextur[0][0]);
	glGenTextures (3, &AutotileTextur[1][0]);
	glGenTextures (3, &AutotileTextur[2][0]);
	glGenTextures (3, &AutotileTextur[3][0]);
	glGenTextures (1, &Waffe[0]);
	glGenTextures (10, &Ufer[0]);
	glGenTextures (10, &UferB[0]);
	glGenTextures (10, &UferC[0]);

	//Speeds up a bit when thoose values are used (which are used often and are fixed)
	BodenTextPosStart[0]=inttot16(0);
	BodenTextPosStart[1]=inttot16(32);
	BodenTextPosStart[2]=inttot16(64);
	BodenTextPosStart[3]=inttot16(96);
	
	BodenTextPosEnd[0]=inttot16(32);
	BodenTextPosEnd[1]=inttot16(64);
	BodenTextPosEnd[2]=inttot16(96);
	BodenTextPosEnd[3]=inttot16(128);

	InitTableOfNormal();
	swiWaitForVBlank();
//	glReset ();
	glMatrixMode(GL_PROJECTION); glLoadIdentity(); 
 
	gluPerspective (25, 1, .1, 100);//<---LATER THATS RIGHT ONE


}


void E3D_StartRender(){
	extern float CamPosSX,CamPosSY;
	extern float PlHeight;

	gluLookAt(CamPosSX, (PlHeight*1.25)+3, 3.4+CamPosSY-(PlHeight*.28),// Camera possition 
		CamPosSX, .4+(PlHeight*.67), CamPosSY+.1-(PlHeight*.28),// Look at
			0, 1, 0);// Up

				
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
			
	glMatrixMode(GL_MODELVIEW);
	// Move away from the camera
	glTranslate3f32 (0, 0, floattov16(-0.1));
					
	glPushMatrix();	
}

void glTranslatef32(f32 x, f32 y, f32 z) {
//---------------------------------------------------------------------------------
  MATRIX_TRANSLATE = x;
  MATRIX_TRANSLATE = y;
  MATRIX_TRANSLATE = z;
}
