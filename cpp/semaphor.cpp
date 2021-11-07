#include "kernel.h"

Semaphore::Semaphore (int init) {
	alloc(myImpl, KernelSem(init));
}

Semaphore::~Semaphore(){
	dealloc(myImpl);
}

int Semaphore::wait (Time maxTimeToWait){
	lock();
	PCB::running->timeToWait = 1;
	if (--myImpl->val < 0) myImpl->block(maxTimeToWait);
	unlock();
	return PCB::running->timeToWait;
}

void Semaphore::signal (){
	lock();
	if (++myImpl->val <= 0) myImpl->unblock();
	unlock();
}

int Semaphore::val() const { return myImpl->val;}
