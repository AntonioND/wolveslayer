#include <nds.h>
#include "Sound7.h"

void handleVBlank() 
{
//	static int heartbeat = 0;
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0, batt=0, aux=0;
	int t1=0, t2=0;
	uint32 temp=0;
//	uint8 ct[sizeof(IPC->curtime)];
		
//	++heartbeat;
		
	but = REG_KEYXY;
	if (~but & 0x40) {
			// Read the touch screen
		x = touchReadXY().x;
		y = touchReadXY().y;
		xpx = touchReadXY().px;
		ypx =touchReadXY().py;
		z1 = touchRead(TSC_MEASURE_Z1);
		z2 = touchRead(TSC_MEASURE_Z2);
	}
		
	batt = touchRead(TSC_MEASURE_BATTERY);
	aux  = touchRead(TSC_MEASURE_AUX);
		
//    rtcGetTime((uint8 *)ct);
  //  BCDToInteger((uint8 *)&(ct[1]), 7);
		
	temp = touchReadTemperature(&t1, &t2);
		
	IPC->buttons   = but;
	IPC->touchX    = x;
	IPC->touchY    = y;
	IPC->touchXpx  = xpx;
	IPC->touchYpx  = ypx;
	IPC->touchZ1   = z1;
	IPC->touchZ2   = z2;
	IPC->battery   = batt;
	IPC->aux       = aux;
		
//	for(u32 i=0; i<sizeof(ct); i++) {
//		IPC->curtime[i] = ct[i];
//	}
		
	IPC->temperature = temp;
	IPC->tdiode1 = t1;
	IPC->tdiode2 = t2;
		
	SndVblIrq();
}

int main(int argc, char ** argv)
{
	rtcReset();
		
	irqInit();
	irqSet(IRQ_VBLANK, handleVBlank);
	irqEnable(IRQ_VBLANK);
	irqSet(IRQ_TIMER0, SndTimerIrq);
	irqEnable(IRQ_TIMER0);
		
	SndInit7();
		
	while (1) {
			
			
		swiWaitForVBlank();
	}
		
	return 0;
}
