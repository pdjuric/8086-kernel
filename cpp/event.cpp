#include "kernel.h"


Event::Event(IVTNo ivtNo) {
    noInterrupts();
    alloc(myImpl, KernelEv(ivtNo));
    if (!myImpl) {
   		print("Memory allocation error occured, couldn't allocate KernelEv. \n");
   	}
    okInterrupts();
}

Event::~Event() {
    noInterrupts();
    if (myImpl) dealloc(myImpl);
    okInterrupts();
}

void Event::wait() {
	if (!myImpl) {
		print("Memory allocation error occured, couldn't wait on Event. \n");
		return;
	}
    lock();
    myImpl->wait();
    unlock();
}

void Event::signal() {
	if (!myImpl) {
		print("Memory allocation error occured, couldn't signal on Event. \n");
		return;
	}
    lock();
    myImpl->signal();
    unlock();
}
