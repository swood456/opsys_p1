#include "Process.h"

Process::Process() {
	processID = "";
	initialArrivalTime = -1;
	totalCpuBurstTime = -1;
	totalNumBursts = -1;
	ioTime = -1;

	curNumBursts = -1;
	curIOTime = -1;
	curProcessTime = -1;

	running = false;
}

Process::Process(std::string _processID, int _initialArrivalTime, int _totalCpuBurstTime, int _totalNumBursts, int _ioTime){
	processID = _processID;
	initialArrivalTime = _initialArrivalTime;
	totalCpuBurstTime = _totalCpuBurstTime;
	totalNumBursts = _totalNumBursts;
	ioTime = _ioTime;

	curNumBursts = totalNumBursts;
	curIOTime = ioTime;
	curProcessTime = totalCpuBurstTime;

	running = false;
}

void Process::print(){
	std::cout << processID << " " << initialArrivalTime << " " << totalCpuBurstTime << " " << totalNumBursts << " " << ioTime << std::endl;
}

int Process::run(int duration){
	curProcessTime -= duration;

	if(curProcessTime <= 0){
		curNumBursts--;
		curProcessTime = totalCpuBurstTime;
		return 1;
	}
	return 0;
}

int Process::runIO(int duration){
	curIOTime -= duration;

	if(curIOTime <= 0){
		curIOTime = ioTime;
		return 1;
	}
	return 0;
}
