#include "kernel.h"

PCB *newPCB = 0, *oldPCB = 0;
StackSize stackSize, __offset, __newOffset;

unsigned oldRegister;
unsigned* newStack = 0, *oldStack = 0, *oldStackTopAddress, *oldStackBottomAddress;


Thread* Thread::clone() const{
	return new Thread(this->myPCB->stackSize * sizeof(unsigned), this->myPCB->timeSlice);
}


void interrupt PCBClone() {

		newStack =  newPCB->stack;
		oldStack =  oldPCB->stack;

		stackSize = oldPCB->stackSize;

		oldStackTopAddress = (unsigned*)MK_FP(_SS, _BP);
		oldStackBottomAddress = &oldStack[stackSize - 1];

		for (__offset = 1; 1; __offset++) {
			newStack[stackSize - __offset] = oldStack[stackSize - __offset];
			if (&oldStack[stackSize - __offset] == oldStackTopAddress) break;
		}

		newPCB->ss = FP_SEG(&newPCB->stack[stackSize - __offset]);
		newPCB->sp = FP_OFF(&newPCB->stack[stackSize - __offset]);
		newPCB->bp = newPCB->sp;


	while ((oldRegister = newPCB->stack[stackSize - __offset]) != 0){
		__newOffset = (StackSize)(newPCB->stack - (unsigned*) MK_FP(_SS, oldRegister) + stackSize) ;
		newPCB->stack[stackSize - __offset] = FP_OFF(newPCB->stack + stackSize - __newOffset);
		__offset = __newOffset;
	}
}

StackSize j, initOffset;

void interrupt PCBClonee() {

	oldStack =  oldPCB->stack;
	stackSize =oldPCB->stackSize;
	initOffset = __offset = (StackSize)(oldPCB->stack - (unsigned*) MK_FP(_SS, _BP) + stackSize);

	newPCB->ss = FP_SEG(&newPCB->stack[stackSize - __offset]);
	newPCB->sp = FP_OFF(&newPCB->stack[stackSize - __offset]);
	newPCB->bp = newPCB->sp;

	noInterrupts();
	for (j = 1 ; j<=initOffset; j++) newPCB->stack[stackSize - j] = oldPCB->stack[stackSize - j];	okInterrupts();

	while ((oldRegister = newPCB->stack[stackSize - __offset]) != 0){
		__newOffset = (StackSize)(newPCB->stack - (unsigned*) MK_FP(_SS, oldRegister) + stackSize) ;
		newPCB->stack[stackSize - __offset] = FP_OFF(newPCB->stack + stackSize - __newOffset);
		__offset = __newOffset;
	}
}


ID Thread::fork(){
	lock();
	oldPCB = (PCB*) PCB::running;
	Thread* t = 0;

	oldStack =  oldPCB->stack;
	stackSize = oldPCB->stackSize;
	oldStackTopAddress = (unsigned*) MK_FP(_SS, _SP);

	for (__offset = 1; &oldPCB->stack[stackSize - __offset] != oldStackTopAddress; __offset++);

	if (stackSize > 12 + __offset)  t = oldPCB->myThread->clone();	//stek je dovoljne velicine za interrupt

	if (t == 0 || t->myPCB == 0 || t->myPCB->stack == 0) {
		dealloc(t);
		oldPCB = 0;

		unlock();
		return -1;
	}
	newPCB = t->myPCB;
	newPCB->timeToWait = runnable;
	newPCB->status = oldPCB->status;
	PCBClone();

	if (newPCB && oldPCB &&  newPCB->stack == 0) {
		dealloc(t);
		oldPCB = 0;
		newPCB = 0;

		unlock();
		return -1;
	}
	if (((PCB*)PCB::running)->Id == t->getId()) return 0;

	oldPCB->forkChildrenCount++;
	newPCB->forkParent = oldPCB;
	oldPCB = 0;
	Scheduler::put(newPCB);
	newPCB = 0;

	unlock();
	return  t->getId();
}

void Thread::exit(){
	lock();
	PCB::running->status = finished;
	while (((PCB*)PCB::running)->blocked.peekFront() != 0) {
		PCB* t = ((PCB*)PCB::running)->blocked.popFront();
		t->status = runnable;
		Scheduler::put(t);
	}

	PCB* parent = ((PCB*) PCB::running)->forkParent;
	if (parent) {
		parent->forkChildrenCount--;
		if (parent->forkChildrenCount == 0 && parent->status == waitingForChildren) {
			parent->status = runnable;
			Scheduler::put(parent);
		}
	}
	unlock();
	dispatch();

}
void Thread::waitForForkChildren(){
	lock();
	if (((PCB*)PCB::running)->forkChildrenCount == 0) {
		unlock();
		return;
	}
	((PCB*)PCB::running)->status = waitingForChildren;
	unlock();
	dispatch();
}
s