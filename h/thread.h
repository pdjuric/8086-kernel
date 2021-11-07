#ifndef THREAD_H_
#define THREAD_H_

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
const StackSize maxStackSize = 65535;
const StackSize minStackSize = 64;
typedef unsigned int Time;
const Time defaultTimeSlice = 2;
typedef int ID;

class PCB;

class Thread {				// DEFINED IN: thread.cpp
public:
	void start();
	void waitToComplete();
	virtual ~Thread();

	ID getId();
	static ID getRunningId();
	static Thread* getThreadById(ID Id);

protected:
	friend class PCB;
	Thread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	virtual void run() {};

private:
	PCB* myPCB;

public:						// DEFINED IN: fork.cpp
	static ID fork();
	static void exit();
	static void waitForForkChildren();

	virtual Thread* clone() const;

	//added:
private:
	Thread(int, int, int) : myPCB(0) {};
	friend class IdleThread;
	friend class UserMainThread;
	friend class MainThread;
};

void dispatch();			// DEFINED IN: context.cpp

#endif /* THREAD_H_ */
