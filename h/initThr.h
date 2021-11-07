#ifndef KERNEL_INITTHR_H_
#define KERNEL_INITTHR_H_

#include "thread.h"

class IdleThread : public Thread{
private:
	virtual void run();
	IdleThread();

	friend int main(int argC, char* argV[]);
};


void MainThread();

class UserMainThread: public Thread{
public:
	int getRet() {return ret;}
private:
	int argC;
	char** argV;
	int ret;
	virtual void run();
	UserMainThread(int argC, char* argV[]);
	~UserMainThread() {
		waitToComplete();
	}
	friend int main(int argC, char* argV[]);
};


#endif /* KERNEL_INITTHR_H_ */
