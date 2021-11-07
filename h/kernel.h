#ifndef KERNEL_H_
#define KERNEL_H_

#include "list.h"
#include "schedule.h"
#include "initThr.h"
#include "semaphor.h"
#include "event.h"

enum Status {notStarted, runnable, blocked, finished, idle, waitingForChildren};

//---------LISTS--------------------------
class PCB;
class PCBList_t : public List<PCB>{
public:
	int insertByTime(PCB* i,volatile Time& firstTime);
};

class KernelSem;
class SemList_t : public List<KernelSem> {
public:
	void releaseAll();
};

//---------IMPORTED GLOBAL DATA-----------
extern volatile int lockFlag;
extern volatile int contextSwitchDemand;
extern PCBList_t* PCBKernelList;
extern SemList_t* SemKernelList;
extern volatile IVTEntry* IVT[];

//---------KERNEL DATA STRUCTURES---------

class PCB {					// DEFINED IN: kerPCB.cpp
public:
	unsigned bp;
	unsigned sp;
	unsigned ss;
	unsigned* stack;
	StackSize stackSize;
	void init(StackSize stackSize);

	Status status;
	Time timeSlice;
	ID Id;

	Thread* myThread;

	Time timeToWait;
	List<PCB> blocked;

	PCB* forkParent;
	unsigned forkChildrenCount;

	PCB(Thread* myThread, Time timeSlice);
	PCB();
	~PCB();
	static void wrapper();

	static volatile PCB* running;
	static PCB* Main;
	static PCB* Idle;
	static PCB* UserMain;

	friend int main(int argC, char* argV[]);

};


class KernelSem{			// DEFINED IN: kerSem.cpp
public:
	KernelSem(int v);
	~KernelSem();
	void block(Time maxTimeToWait);
	void unblock();
	void release();

	volatile int val;
	volatile Time firstTime;
	volatile int semLockFlag;
	volatile int releaseFlag;

	ID currId;

	PCBList_t timed;
	PCBList_t unlimited;
};


class KernelEv {				// DEFINED IN: kerEv.cpp
public:
	KernelEv (IVTNo ivtNo);
	~KernelEv();

	void wait();
	void signal();

	int blockFlag;
	int signalFlag;
	Event* myEvent;
	PCB* myPCB;
	IVTNo ivtNo;
};

//---------TIMER SETUP--------------------

							// DEFINED IN: context.cpp
void initTimer();
void restoreTimer();

#endif /* KERNEL_H_ */
