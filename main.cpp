#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include "Process.h"

const int m = 1;		//number of processors available
const int t_cs = 8;		//time it takes to complete a context switch
const int t_slice = 84;	//time slice value

void fcfs(std::vector<Process>, int);
std::string printQueue(std::queue<Process *> q) {
	if(q.empty()) {
		return "[Q empty]";
	}
	std::string output = "[Q";
	while (!q.empty()) {
		output += " " + q.front()->processID;
		q.pop();
	}

	return output + "]";
}

int main(int argc, char* argv[]){

	//check arguments
	if(argc != 3){
		fprintf(stderr, "ERROR: Invalid arguments\nUSAGE: ./a.out <input-file> <stats-output-file>\n" );
		return EXIT_FAILURE;
	}

	//open argv[1] to read file
	std::ifstream inFile;
	inFile.open(argv[1]);

	std::string line;
	std::vector<Process> processes;


	//check to see if the file is able to be opened
	if(!inFile){
		fprintf(stderr, "ERROR: Invalid input file format\n" );
		return EXIT_FAILURE;
	}

	int numBursts = 0;

	//go through the file line by line
	while(std::getline(inFile, line)){
		if(line[0] != '#'){

			std::cout << "debug: got line " << line << std::endl;

			//parse the input line correctly
			std::string processID;
			int processInfo [4];

			std::string delimiter = "|";

			size_t pos = 0;
			std::string token;

			pos = line.find(delimiter);
			processID = line.substr(0, pos);
			line.erase(0, pos + delimiter.length());

			for(int i = 0; i < 4; i++){


				pos = line.find(delimiter);

				token = line.substr(0, pos);

				processInfo[i] = atoi(token.c_str());

			    line.erase(0, pos + delimiter.length());

			}

			//push the new object onto the vector
			processes.push_back(Process(processID, processInfo[0], processInfo[1], processInfo[2], processInfo[3]));
			numBursts += processInfo[2];
		}
	}


	inFile.close();

	for(unsigned int i = 0; i < processes.size(); i++){
		processes[i].print();
	}

	fcfs(processes, numBursts);

	// TODO Shortest Job First

	// TODO Round Robin


	//std::cout << "hello world" << std::endl;



	return EXIT_SUCCESS;
}

void fcfs(std::vector<Process> fcfsAdding, int numBursts) {

	std::cout<<"time 0ms: Simulator started for FCFS [Q empty]"<<std::endl;

	unsigned int numProcesses = fcfsAdding.size();
	std::vector<Process *> cpu(m);	// stores all the running processes
	std::vector<int> cpuCS(m);		// countdown timers for context switches for each core

	double	avgWaitTime = 0;
	double	avgTurnTime = 0;
	int		numContextSwitches = 0;

	//////////////////////////
	//First-Come-First-Serve//
	//////////////////////////

	/*
	1. remove finished processes from the cpu
		- check if cpuCS does not equal zero
			- check if process is finished
				- check if process is done
					- TRUE then remove process completely
					- FALSE then place on IO list
		- else decrement cpuCS
	2. remove processes from IO list and place them on ready queue
	3. add new processes to ready queue
	4. remove processes from ready queue and place on cpu

	*/


	//sort the vector by process ID?
	std::queue<Process *> fcfsQueue;
	std::vector<Process *> fcfsIOList;
	std::vector<Process *>::iterator IOitr;
	std::vector<Process *> IOtmp;

	std::vector<Process>::iterator addingItr;
	int time = 0;
	while(numProcesses > 0){

		for (int i = 0; i < m; ++i) {
			if (cpuCS[i] == 0) {
				if (cpu[i] != NULL && cpu[i]->run(1) == 1) {
					// either put this process in the io queue if it's not finished
					// or remove it completely
					if (cpu[i]->curNumBursts <= 0) {
						std::cout<<"time "<<time<<"ms: Process "<<cpu[i]->processID<<" terminated "
							<<printQueue(fcfsQueue)<<std::endl;

						numProcesses--;
					} else {
						std::cout<<"time "<<time<<"ms: Process "<<cpu[i]->processID<<" completed a CPU burst; "
							<<cpu[i]->curNumBursts<<" to go "<<printQueue(fcfsQueue)<<std::endl;

						std::cout<<"time "<<time<<"ms: Process "<<cpu[i]->processID<<" blocked on I/O until time "
							<<time+cpu[i]->ioTime<<"ms "<<printQueue(fcfsQueue)<<std::endl;

						IOtmp.push_back(cpu[i]);
					}
					avgTurnTime += time - cpu[i]->burstArrivalTime;

					cpu[i] = NULL;

					cpuCS[i] = t_cs/2;
				}
			} else {
				cpuCS[i]--;

				if(cpuCS[i] == 0 && cpu[i] != NULL){
					std::cout<<"time "<<time<<"ms: Process "<<cpu[i]->processID<<" started using the CPU "
						<<printQueue(fcfsQueue)<<std::endl;
				}
			}
		}

		IOitr = fcfsIOList.begin();
		while(IOitr != fcfsIOList.end()){
			//update IO for this process
			if((*IOitr)->runIO(1) == 1){
				(*IOitr)->burstArrivalTime = time;

				//add the process back into the queue
				fcfsQueue.push(*IOitr);

				std::cout<<"time "<<time<<"ms: Process "<< (*IOitr)->processID
					<<" completed I/O "<<printQueue(fcfsQueue)<<std::endl;

				//remove the process from the IO list
				fcfsIOList.erase(IOitr);

			}
			else{
				IOitr++;
			}
		}

		fcfsIOList.insert(fcfsIOList.end(), IOtmp.begin(), IOtmp.end());
		IOtmp.clear();

		//look to see if any processes arrive
		addingItr = fcfsAdding.begin();
		while(addingItr != fcfsAdding.end()){

			if(addingItr->initialArrivalTime == time){
				fcfsQueue.push(&(*addingItr));

				std::cout<<"time "<<time<<"ms: Process "<<addingItr->processID
					<<" arrived "<<printQueue(fcfsQueue)<<std::endl;

				//fcfsAdding.erase(addingItr);
				addingItr++;

			} else{
				addingItr++;
			}
		}

		// add stuff to cpu
		for (int i = 0; i < m; ++i) {
			if (cpu[i] == NULL && !fcfsQueue.empty()) {
				numContextSwitches++;
				avgWaitTime += time - fcfsQueue.front()->burstArrivalTime;
				//avgTurnTime += time + fcfsQueue.front()->totalCpuBurstTime - fcfsQueue.front()->burstArrivalTime;

				cpu[i] = fcfsQueue.front();
				fcfsQueue.pop();

				cpuCS[i] += t_cs/2;
			}
		}

		time++;
	}

	std::cout<<"time "<<time+ t_cs/2 - 1<<"ms: Simulator ended for FCFS"<<std::endl<<std::endl;

	std::cout<<"AVG WAIT = "<<avgWaitTime/numBursts<<std::endl;
	std::cout<<"AVG TURN = "<<(avgTurnTime)/numBursts<<std::endl;
	std::cout<<"CSWITCH = "<<numContextSwitches<<std::endl;
}
