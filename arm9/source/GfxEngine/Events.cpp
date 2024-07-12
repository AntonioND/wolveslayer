#include "3D.h"

char EventSpeech[10][256];//to hold 10 textes for event with 256 chars
int EventSpeechCount;//Holds the Number of speeches a event has...

void ResetEvent(void){
	for(int b=0;b<10;b++)strcpy(EventSpeech[b],"");
	EventSpeechCount=0;
}

void AddTexttoEvent(char txt[256]){
	if(EventSpeechCount<10){
		strcpy(EventSpeech[EventSpeechCount],txt);
		EventSpeechCount++;
	}
}

bool Gameended=false;
void CheckEndGame(void){
	extern int PlHP;
	extern int GegnerHP[10];
	extern bool GegnerIsBoss[10];
	extern int GegnerCount;
	extern int PlStatus;
	extern int PlFrame;
	extern void Splash(void);
	extern int Blend;

	int a;

	if(Gameended==true && (keysDown() & KEY_START)){
		Gameended=false;
		PlStatus=0;
		PlFrame=0;
		PlHP=100;

		for(int b=8;b>=0;b--){
				Blend=b;
				RunTime();
				E3D_StartRender();
				UpdateDynamic();
				Refresh3D();
				glPopMatrix (1);
				glFlush(2);
		}

		ReadSettings();	
		
		for(int b=0;b<=8;b++){
				Blend=b;
				RunTime();
				E3D_StartRender();
				UpdateDynamic();
				Refresh3D();
				glPopMatrix (1);
				glFlush(2);
		}
			
	}
	
	if(Gameended==false){
		//check if player died
		if(PlHP==0){
			Gameended=true;
			StartSong("/wolveslayer/bgfx/game-over.mod");
			Print("YOU SUCKED!!!",80,75);
			Print(" Restart with",80,85);
			Print("  Start-Key  ",80,95);			
		}
		
		//check if endboss died
		for(a=0;a<=GegnerCount;a++){
			if(GegnerIsBoss[a]==true && GegnerHP[a]<=0){
				Gameended=true;
				StartSong("/wolveslayer/bgfx/game-finished.mod");
				PlStatus=-1;
				PlFrame=0;
				Print(" YOU OWNED!!!",80,75);
				Print(" Restart with",80,85);
				Print("  Start-Key  ",80,95);			
			}
		}
	}
}
