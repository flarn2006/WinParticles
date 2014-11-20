#pragma once
#include <vector>
#include "stdafx.h"

template <typename TItem>
class CParallelWorker
{
private:
	int threadCount;
	HANDLE *threads;
	std::vector<TItem&> itemStorage;

	static DWORD WINAPI ThreadProc(LPVOID lpParameter)
	{
		CParallelWorker<TItem> *owner = (CParallelWorker<TItem>*)lpParameter;
		for (;;) {
			if (owner->itemStorage->empty()) {
				SwitchToThread();
			} else {
				while (!owner->itemStorage->empty()) {
					TItem &item = owner->itemStorage->back();
					owner->itemStorage->pop_back();
					owner->ProcessOneItem(item);
				}
			}
		}
	}

protected:
	virtual void ProcessOneItem(TItem &item) = 0;

public:
	CParallelWorker(int threadCount)
	{
		this->threadCount = threadCount;
		threads = new HANDLE[threadCount];

		for (int i = 0; i < threadCount; i++) {
			threads[i] = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
		}
	}

	~CParallelWorker()
	{
		for (int i = 0; i < threadCount; i++) {
			TerminateThread(threads[i], 0);
		}
		delete threads;
	}

	void ProcessVector(std::vector<TItem> &items)
	{
		for (std::vector<TItem&>::iterator i = items.begin(); i != items.end(); i++) {
			itemStorage.push_back(*i);
		}

		while (!itemStorage->empty()) SwitchToThread();
	}
};

