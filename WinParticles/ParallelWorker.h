#pragma once
#include <vector>
#include "stdafx.h"

template <typename TItem>
class CParallelWorker
{
private:
	int threadCount;
	HANDLE *threads;
	std::vector<TItem> *vector;
	int itemCount;
	int currentItem;
	int exitedThreads; // -1 means do not start exiting

	static DWORD WINAPI ThreadProc(LPVOID lpParameter)
	{
		CParallelWorker<TItem> *owner = (CParallelWorker<TItem>*)lpParameter;
		while (owner->exitedThreads < 0) {
			if (owner->currentItem >= owner->itemCount) {
				SwitchToThread();
			} else {
				int myCurrentItem;
				while ((myCurrentItem = owner->currentItem) < owner->itemCount) {
					TItem &item = owner->vector->at(myCurrentItem);
					owner->currentItem++;
					owner->ProcessOneItem(item);
				}
			}
		}
		owner->exitedThreads++;
		ExitThread(0);
	}

protected:
	virtual void ProcessOneItem(TItem &item) = 0;

public:
	CParallelWorker(int threadCount)
	{
		exitedThreads = -1;
		itemCount = currentItem = 0;
		this->threadCount = threadCount;
		threads = new HANDLE[threadCount];

		for (int i = 0; i < threadCount; i++) {
			threads[i] = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
		}
	}

	~CParallelWorker()
	{
		exitedThreads = 0;
		while (exitedThreads < threadCount) SwitchToThread();
		delete threads;
	}

	void ProcessVector(std::vector<TItem> &items)
	{
		vector = &items;
		itemCount = items.size();
		currentItem = 0;
		while (currentItem < itemCount) SwitchToThread();
	}
};

