#include "kernel.h"

extern PCBList_t* PCBKernelList;

PCB::PCB(Thread* myThread, Time timeSlice) : bp(0), sp(0), ss(0), timeToWait(0), stackSize(0), forkParent(0), forkChildrenCount(0) {
	Id = PCBKernelList->pushBack(this);
	if (Id == -1) {
		print("Memory allocation error occured while adding PCB to the global list. \n");
		return;
	}
	this->status = notStarted;
	this->timeSlice = timeSlice;
	this->stack = 0;
	this->myThread = myThread;
}

PCB::PCB() : bp(0), sp(0), ss(0), timeToWait(0),  myThread(0), status(notStarted), Id(0), timeSlice(0), stack(0), stackSize(0),
		forkParent(0), forkChildrenCount(0){}

void PCB::init(StackSize stackSize){
	stackSize /= sizeof(unsigned);
	this->stackSize = stackSize;

	alloc(this->stack, unsigned[stackSize]);
	if (!this->stack) {
		print("Memory allocation error occured while initialising stack. \n");
		return;
	}

	this->stack[stackSize - 1] = 0x200;
	this->stack[stackSize - 2] = FP_SEG(PCB::wrapper);
	this->stack[stackSize - 3] = FP_OFF(PCB::wrapper);
	//-4..-11 registerss
	this->stack[stackSize - 12] = 0;

	this->ss = FP_SEG(&stack[stackSize - 12]);
	this->sp = FP_OFF(&stack[stackSize - 12]);
	this->bp = this->sp;

}

PCB::~PCB(){
	if (stack != 0) deallocArray(this->stack);	//jedino za main ne treba da se brise
	PCBKernelList->remove(this);
}

void PCB::wrapper(){
	PCB::running->myThread->run();
	Thread::exit();
}

int PCBList_t::insertByTime(PCB* i,volatile Time& firstTime){
	if (head) head->info->timeToWait = firstTime;
	Node<PCB>* t = head;
	for (; t && t->info->timeToWait <= i->timeToWait;  i->timeToWait -= t->info->timeToWait, t = t->next);
	Node<PCB>* novi;
	alloc(novi, Node<PCB>(i, t ? t->prev : tail, t ? t : 0, currId));
	if (!novi) {
		print("Memory allocation error occured while adding to the InsertByTime \n");
		return -1;
	}
	if (t) t->info->timeToWait -= i->timeToWait;
	if (novi->prev == 0) {
		head = novi;
		firstTime = novi->info->timeToWait;
	}
	if (novi->next == 0) tail = novi;
	return 0;
}


int print(const char *format, ...) {
	int res;
	va_list args;
	lock();
		va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	unlock();
		return res;
}
