#ifndef EVENT_EVENT_H_
#define EVENT_EVENT_H_

typedef unsigned char IVTNo;
typedef void interrupt (*pInterrupt)(...);
class KernelEv;


class IVTEntry{
public:
	IVTEntry(IVTNo ivtNo, pInterrupt newRoutine);
	~IVTEntry();

	void callOldRoutine();
	void signal();

private:
	IVTNo ivtNo;
	pInterrupt oldRoutine, newRoutine;

	void setEvent(KernelEv*);
	void clearEvent();

	KernelEv* myEvent;
	friend class KernelEv;
};

class Event{
public:
	Event(IVTNo ivtNo);
	~Event();

	void wait();

protected:
	friend class KerelEv;
	void signal();

private:
	KernelEv* myImpl;
};


#define PREPAREENTRY(entryNo, flag) \
void interrupt newRoutine##entryNo(...); \
IVTEntry newEntry##entryNo(entryNo, newRoutine##entryNo); \
void interrupt newRoutine##entryNo(...) { \
	if (flag == 1) \
		newEntry##entryNo.callOldRoutine(); \
	newEntry##entryNo.signal(); \
	dispatch(); \
}

#endif /* EVENT_EVENT_H_ */
