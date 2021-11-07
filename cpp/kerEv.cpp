#include "kernel.h"

extern volatile IVTEntry* IVT[];

KernelEv::KernelEv(IVTNo ivtNo):  ivtNo(ivtNo),  blockFlag(0), signalFlag(0){
	myPCB = (PCB*)PCB::running;
	((IVTEntry*)IVT[ivtNo])->setEvent(this);
}

KernelEv::~KernelEv(){
	signal();
	((IVTEntry*)IVT[ivtNo])->clearEvent();
}

void KernelEv::wait(){
	if(PCB::running != myPCB) return;
	if (signalFlag) {
		blockFlag = 0;
		signalFlag--;
	} else {
		myPCB->status = blocked;
		blockFlag = 1;
		dispatch();
	}
}

void KernelEv::signal(){
	if (blockFlag){
		signalFlag = 0;
		myPCB->status = runnable;
		blockFlag = 0;
		Scheduler::put(myPCB);
	} else {
		signalFlag = 1;
	}
}

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt newRoutine): ivtNo(ivtNo), myEvent(0), newRoutine(newRoutine) {
	lock();
	IVT[ivtNo] = this;
	oldRoutine = getvect(ivtNo);
	unlock();
}

IVTEntry::~IVTEntry(){
	clearEvent();
}

void IVTEntry::signal() {
    lock();
	if (myEvent) myEvent->signal();
	unlock();
}

void IVTEntry::setEvent(KernelEv* myEvent) {
    lock();
	setvect(ivtNo, newRoutine);
    this->myEvent = myEvent;
    unlock();
}

void IVTEntry::clearEvent() {
	lock();
	IVT[ivtNo] = 0;
	myEvent = 0;
	setvect(ivtNo, oldRoutine);
	unlock();
}

void IVTEntry::callOldRoutine() {
    lock();
	if (oldRoutine) oldRoutine();
	unlock();
}
