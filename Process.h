#ifndef PROCESS_H
#define PROCESS_H

// TODO remove unnecessary include statements
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <queue>

class Process {
// TODO Make these private
public:
	std::string processID;		// the name of the process
	int initialArrivalTime;		// when the process first arrives
	int totalCpuBurstTime;		// initial cpu burst time
	int totalNumBursts;			// initial number of cpu bursts
	int ioTime;					// initial io time
	int curNumBursts;			// current number of cpu bursts remaining
	int curIOTime;				// current IO time remaining
	int curProcessTime;			// current CPU time remaining
	int burstArrivalTime;		// time current burst arrives in ready queue

public:
	Process();
	Process(std::string PID, int initialArrivalTime, int totalCpuBurstTime, int totalNumBursts, int ioTime);
	void print();
	int run(int duration);
	int runIO(int duration);
};

struct FirstComeLessThan{
	bool operator()(const Process& lhs, const Process& rhs) const{
		if(lhs.initialArrivalTime == rhs.initialArrivalTime){
			return lhs.processID > rhs.processID;
		}
		return lhs.initialArrivalTime > rhs.initialArrivalTime;
	}
};

#endif
