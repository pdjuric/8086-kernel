#include "kernel.h"


extern SemList_t* SemKernelList;

KernelSem::KernelSem(int v) : val(v > 0 ? v : 0), currId(0) {
	semLockFlag = 0;
	releaseFlag = 0;
	lockSem();
	SemKernelList->pushBack(this);
	unlockSem();
}

void KernelSem::block(Time maxTimeToWait){
	PCB::running->status = blocked;
	PCB::running->timeToWait = maxTimeToWait;
	if (maxTimeToWait == 0){
		unlimited.currId = currId++;
		unlimited.pushBack((PCB*)PCB::running);
	} else {
		timed.currId = currId++;
		lockSem();
		timed.insertByTime((PCB*)PCB::running, firstTime);
		unlockSem();
	}
	dispatch();
}

void KernelSem::unblock(){
	PCB* unblocked = 0;
	int existsUnlimited = (unlimited.peekFront() != 0);
	int existsTimed = (timed.peekFront() != 0);
	if (existsUnlimited && (!existsTimed || timed.frontId() > unlimited.frontId())){
		unblocked = unlimited.popFront();
	} else {
		lockSem();
		unblocked = timed.popFront();
		if (timed.peekFront()){
			firstTime += timed.peekFront()->timeToWait;
			timed.peekFront()->timeToWait = firstTime;
		} else firstTime = 0;
		unlockSem();
	}
	if (unblocked == 0) cout << "greska na semaforu, odblokirava se a nema ni jedne niti" << endl;
	unblocked->status = runnable;
	unblocked->timeToWait = 1;
	Scheduler::put(unblocked);
}

void KernelSem::release() {
	lockSem();
	PCB* pcb = 0;
	do {
		pcb = timed.popFront();
		if (!pcb) break;
		pcb->status = runnable;
		pcb->timeToWait = 0;
		val++;
		Scheduler::put(pcb);
		firstTime = (timed.peekFront() != 0 ? timed.peekFront()->timeToWait : 0);
	} while (timed.peekFront() && !firstTime );
	unlockSem();

}

KernelSem::~KernelSem(){
	PCB* t = 0;
	lockSem();
	while ((t = timed.popFront())) {
		t->status = runnable;
		t->timeToWait = 0;
		Scheduler::put(t);
	}
	unlockSem();
	while ((t = unlimited.popFront())) {
			t->status = runnable;
			t->timeToWait = 0;
			Scheduler::put(t);
		}
	SemKernelList->remove(this);
}


void SemList_t::releaseAll() {
		for (Node<KernelSem>* t = head; t; t = t->next){
			if (t->info->val < 0 && --t->info->firstTime == 0) {
				if (t->info->semLockFlag == 0) t->info->release();
				else t->info->releaseFlag ++;
			}
		}
	}
