#pragma once
#include "../Common/shared.h"
#include "../Common/structures.h"


DWORD WINAPI WorkerRegistry(LPVOID param) {
	Worker* worker = (Worker*)param;
	if (worker == NULL) {
		print("Cant thread worker - worker null");
		return 0;
	}
	WorkerData data = (worker->data);
	AddWorkerSafe(data, L1);

}

DWORD WINAPI WorkerBusy(LPVOID param) {
	Worker* worker = (Worker*)param;
	if (worker == NULL) {
		print("Cant thread worker - worker null");
		return 0;
	}
	WorkerData data = (worker->data);
	AddWorkerSafe(data, L2);

}
