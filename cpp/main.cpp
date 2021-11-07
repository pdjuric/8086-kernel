#include "kernel.h"


PCBList_t* PCBKernelList = new PCBList_t();
SemList_t* SemKernelList = new SemList_t();

volatile PCB* PCB::running = 0;
PCB* PCB::Main = 0;
PCB* PCB::Idle = 0;
PCB* PCB::UserMain = 0;
volatile IVTEntry* IVT[256] = {0};
int flag = 0;

int main(int argC, char* argV[]) {
	if (!PCBKernelList || !SemKernelList) {cout <<"ERROR: LISTS AREN'T ALLOCATED!"; while(1); }

	MainThread();
	new IdleThread();
	new UserMainThread(argC, argV);
	if (!PCB::UserMain->myThread || !PCB::Idle->myThread) {cout <<"ERROR: COULDN'T ALLOCATE MEMORY!"; while(1); }

	initTimer();

	PCB::UserMain->myThread->start();
	PCB::UserMain->myThread->waitToComplete();

	int ret = ((UserMainThread*) PCB::UserMain->myThread)->getRet();
	for (int i = 0; i < 256; i++) if (IVT[i]) dealloc(IVT[i]);

	restoreTimer();

	Node<PCB>* t = PCBKernelList->head;
	int undestructed = 0;
	while (t) {
		if (t->info != PCB::UserMain) undestructed++;
		delete t->info;
		t = t->next;
	}

	if (undestructed) cout << undestructed << " thread" << (undestructed != 1 ? "s" : "") << " remain undestructed\n";

	Node<KernelSem>* s = SemKernelList->head;

	while (s) {
		delete s->info;
		s = s->next;
	}


	delete PCB::UserMain;
	delete PCB::Main;
	delete PCB::Idle;
	delete PCBKernelList;
	delete SemKernelList;
	return ret;
}
