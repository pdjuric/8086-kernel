#include "kernel.h"

extern void tick();

volatile int lockFlag = 0;
volatile int contextSwitchDemand = 0;
volatile Time timeCounter = 1;

unsigned oldTimerOFF, oldTimerSEG;
volatile unsigned tsp;
volatile unsigned tss;
volatile unsigned tbp;

void interrupt tickTock(...){
	if (!contextSwitchDemand){
		asm int 60h;
		tick();
		if (((PCB*)PCB::running)->timeSlice != 0 && timeCounter > 0) timeCounter--;
		SemKernelList->releaseAll();
	}

	if ((!timeCounter && ((PCB*)PCB::running)->timeSlice != 0) || contextSwitchDemand){
		if (lockFlag == 0) {
			contextSwitchDemand = 0;

			PCB::running->sp = _SP;
			PCB::running->ss = _SS;
			PCB::running->bp = _BP;

			if (((PCB*)PCB::running)->status == runnable) Scheduler::put((PCB*)PCB::running);
			PCB::running = Scheduler::get();
			if (PCB::running == 0) PCB::running = PCB::Idle;

			_SP = PCB::running->sp;
			_SS = PCB::running->ss;
			_BP = PCB::running->bp;

			timeCounter = ((PCB*)PCB::running)->timeSlice;
			if (!timeCounter) timeCounter = 1;

		} else contextSwitchDemand = 1;
	}
}

void dispatch() {
	noInterrupts();
	contextSwitchDemand = 1;
	tickTock();
	okInterrupts();
}

void initTimer(){
	noInterrupts();
	pInterrupt oldRoutine = getvect(0x08);
	setvect(0x08, tickTock);
	setvect(0x60, oldRoutine);
	okInterrupts();
}

void restoreTimer(){
	noInterrupts();
	pInterrupt oldRoutine = getvect(0x60);
	setvect(0x08, oldRoutine);
	okInterrupts();

}