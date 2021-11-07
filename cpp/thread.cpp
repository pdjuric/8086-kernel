#include "kernel.h"

Thread::Thread(StackSize stackSize, Time timeSlice){
	if (stackSize > maxStackSize) stackSize = maxStackSize;
	if (stackSize < minStackSize) stackSize = minStackSize;
	lock();
	alloc(myPCB, PCB(this, timeSlice));
	if (!myPCB || myPCB->Id == -1) return;
	else myPCB->init(stackSize);
	unlock();
}

ID Thread::getId() {return this->myPCB->Id;}

ID Thread::getRunningId() {
	return PCB::running->Id;
}

void Thread::start() {
	if (myPCB->status != notStarted) return;
	lock();
	myPCB->status = runnable;
	Scheduler::put(myPCB);
	unlock();
}

Thread* Thread::getThreadById(ID Id) {
	lock();
	PCB* t = PCBKernelList->find(Id);
	unlock();
	if (!t) return 0;
	return t->myThread;
}

Thread::~Thread(){
	dealloc(myPCB);
}

void Thread::waitToComplete(){
	lock();
	if (myPCB->status == finished || myPCB->status == notStarted || PCB::running == myPCB ) {unlock(); return;}
	myPCB->blocked.pushFront((PCB*)PCB::running);
	PCB::running->status = blocked;
	dispatch();
	unlock();

}
