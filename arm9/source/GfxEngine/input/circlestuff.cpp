#include "../3D.h"
#include <math.h>

typedef struct{
	short X;
	short Y;
	u8 Dirrection;
	u8 DirrectionDiagonal;
}circlepoint;

typedef struct{
	circlepoint *Pix;
	int count;
}circle;

circle CircleLUT[6];

//This precalulated circles maight be usefull for hitdetection :D
void makecircle(int rad){
	int X,Y;int ox=0,oy=0;int deg=0;
	int count=0;
	X=-1;Y=-1;
	
	//ok first we count the pixels which will be used...
	f32 radius=inttof32(rad+1);
	do {
		X=round(f32tofloat(mulf32(radius,cosLerp(deg*64))));
		Y=round(f32tofloat(mulf32(radius,sinLerp(deg*64))));
		
		if(ox!=X || oy!=Y){
			count++;
		}
		ox=X;oy=Y;
		deg+=1;
	}
	while (deg <= 511);
	
	//fine...now we can malloc the space for it and fill it with datas :D
	CircleLUT[rad].Pix = (circlepoint *)malloc (sizeof(circlepoint) * count);
	CircleLUT[rad].count=count;
	count=0;deg=0;ox=0,oy=0;X=-1;Y=-1;
	do {
		X=round(f32tofloat(mulf32(radius,cosLerp(deg*64))));
		Y=round(f32tofloat(mulf32(radius,sinLerp(deg*64))));
		
		if(ox!=X || oy!=Y){
			CircleLUT[rad].Pix[count].X=X;
			CircleLUT[rad].Pix[count].Y=Y;			
			//if you just press up,down,left or right
			if(deg>0)CircleLUT[rad].Pix[count].Dirrection=3;//down-right
			if(deg>128)CircleLUT[rad].Pix[count].Dirrection=5;//down-left
			if(deg>256)CircleLUT[rad].Pix[count].Dirrection=7;//up-left
			if(deg>384)CircleLUT[rad].Pix[count].Dirrection=1;//up-right
			
			//if you press 2 dirrections
			if(deg>448 || deg<64)CircleLUT[rad].Pix[count].DirrectionDiagonal=2;//right
			if(deg>64)CircleLUT[rad].Pix[count].DirrectionDiagonal=4;//down
			if(deg>192)CircleLUT[rad].Pix[count].DirrectionDiagonal=6;//left
			if(deg>320 && deg<448)CircleLUT[rad].Pix[count].DirrectionDiagonal=0;//up
			count++;
		}
		//if you just press up,down,left or right		
		if(deg==0)CircleLUT[rad].Pix[count-1].Dirrection=2;//right
		if(deg==128)CircleLUT[rad].Pix[count-1].Dirrection=4;//down
		if(deg==256)CircleLUT[rad].Pix[count-1].Dirrection=6;//left
		if(deg==384)CircleLUT[rad].Pix[count-1].Dirrection=0;//up
		//if you press 2 dirrections
		if(deg==64)CircleLUT[rad].Pix[count-1].DirrectionDiagonal=3;//down-right
		if(deg==192)CircleLUT[rad].Pix[count-1].DirrectionDiagonal=5;//down-left
		if(deg==320)CircleLUT[rad].Pix[count-1].DirrectionDiagonal=7;//up-left
		if(deg==448)CircleLUT[rad].Pix[count-1].DirrectionDiagonal=1;//up-right

		ox=X;oy=Y;
		deg+=1;
	}
	while (deg <= 511);
}

void InitCircles(void){
	for(int q=1;q<6;q++)makecircle(q);
}

void DrawCircle(int num){
	if(num<1 || num>5)return;
	
	int x,y;
	for(int q=0;q<CircleLUT[num].count;q++){
		x=CircleLUT[num].Pix[q].X;
		y=CircleLUT[num].Pix[q].Y;
		
		if(CircleLUT[num].Pix[q].Dirrection==0)
			BG_GFX_SUB[x+128+((y+96)*256)]=RGB15(31,31,31) | BIT(15);

		if(CircleLUT[num].Pix[q].Dirrection==1)
			BG_GFX_SUB[x+128+((y+96)*256)]=RGB15(0,31,31) | BIT(15);

		if(CircleLUT[num].Pix[q].Dirrection==2)
			BG_GFX_SUB[x+128+((y+96)*256)]=RGB15(31,0,31) | BIT(15);

		if(CircleLUT[num].Pix[q].Dirrection==3)
			BG_GFX_SUB[x+128+((y+96)*256)]=RGB15(31,31,0) | BIT(15);

		if(CircleLUT[num].Pix[q].Dirrection==4)
			BG_GFX_SUB[x+128+((y+96)*256)]=RGB15(0,31,0) | BIT(15);

		if(CircleLUT[num].Pix[q].Dirrection==5)
			BG_GFX_SUB[x+128+((y+96)*256)]=RGB15(0,0,31) | BIT(15);

		if(CircleLUT[num].Pix[q].Dirrection==6)
			BG_GFX_SUB[x+128+((y+96)*256)]=RGB15(31,0,0) | BIT(15);

		if(CircleLUT[num].Pix[q].Dirrection==7)
			BG_GFX_SUB[x+128+((y+96)*256)]=RGB15(0,0,0) | BIT(15);

	}
}

void Checkcolision(int x,int y,int sx,int sy,int rad,bool *values){
	int X,Y;
	for(int a=0;a<8;a++)values[a]=true;

	for(int q=0;q<CircleLUT[rad].count;q++){
		X=CircleLUT[rad].Pix[q].X;
		Y=CircleLUT[rad].Pix[q].Y;
		
		if(Passable(x,y,sx+X,sy+Y)==false)
			values[CircleLUT[rad].Pix[q].Dirrection]=false;
	}
}

void CheckcolisionDiagonal(int x,int y,int sx,int sy,int rad,bool *values){
	int X,Y;
	for(int a=0;a<8;a++)values[a]=true;

	for(int q=0;q<CircleLUT[rad].count;q++){
		X=CircleLUT[rad].Pix[q].X;
		Y=CircleLUT[rad].Pix[q].Y;
		
		if(Passable(x,y,sx+X,sy+Y)==false)
			values[CircleLUT[rad].Pix[q].DirrectionDiagonal]=false;
	}
}

