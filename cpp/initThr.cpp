#include "kernel.h"

extern int userMain(int argc, char** argv);

void IdleThread::run(){
	while (1);
}

IdleThread::IdleThread() : Thread(0, 0, 0) {
	alloc(myPCB, PCB());
	myPCB->timeSlice = 1;
	myPCB->status = idle;
	myPCB->init(minStackSize);
	myPCB->myThread = this;
	PCB::Idle = this->myPCB;
}

void MainThread(){
	alloc(PCB::Main, PCB());
	PCB::Main->timeSlice = 1;
	PCB::Main->status = runnable;
	PCB::running = PCB::Main;
}

void UserMainThread::run() {
	ret = userMain(argC, argV);
}

UserMainThread::UserMainThread(int argC, char* argV[]) : Thread(0, 0, 0) {
	alloc(myPCB, PCB());
	myPCB->status = notStarted;
	myPCB->Id = PCBKernelList->pushBack(myPCB);
	myPCB->init(maxStackSize);
	myPCB->myThread = this;
	this->argC = argC;
	this->argV = argV;
	ret = 0;
	PCB::UserMain = myPCB;
}
