#include <iostream>

class Process{
private:
	int processID;
	int initialArrivalTime;
	int totalCpuBurstTime;
	int totalNumBursts;
	int ioTime;

	public Process(int _processID, int _initialArrivalTime, int _totalCpuBurstTime int _totalNumBursts, int _ioTime){
		processID = _processID;
		initialArrivalTime = _initialArrivalTime;
		totalCpuBurstTime = _totalCpuBurstTime;
		totalNumBursts = _totalNumBursts;
		ioTime = _ioTime;
	}
};

int main(int argc, char* argv[]){
	
	std::cout << "hello world" << std::endl;



	return true;
}