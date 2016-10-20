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

void fcfs(std::vector<Process>);
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

		}
	}


	inFile.close();

	for(unsigned int i = 0; i < processes.size(); i++){
		processes[i].print();
	}

	fcfs(processes);

	/*

	//this is an even worse way to do things
	std::priority_queue<Process, std::vector<Process>, FirstComeLessThan> fcfsPriorityQueue;

	//for(int i = 0; i < processes)
	for(unsigned int i = 0; i < processes.size(); i++){
		//processes[i].print();
		fcfsPriorityQueue.push(processes[i]);
	}

	std::cout << "now in fcfs order\n";
	while(!fcfsPriorityQueue.empty()){
		Process temp = fcfsPriorityQueue.top();
		//fcfsPriorityQueue.top().print();
		temp.print();
		fcfsPriorityQueue.pop();
	}*/

	//do Shortest Job First

	//do Round Robin


	//std::cout << "hello world" << std::endl;



	return EXIT_SUCCESS;
}

void fcfs(std::vector<Process> fcfsAdding) {
	unsigned int numProcesses = fcfsAdding.size();
	std::vector<Process *> cpu(m);	// stores all the running processes
	std::vector<int> cpuCS(m);		// countdown timers for context switches for each core

	//////////////////////////
	//First-Come-First-Serve//
	//////////////////////////

	/*
	1. see if things arrived at this ms
		if yes add them the process queue at back
	2. check to see if any processes have completed their I/O work
		if yes readd them to the proces queue keeping track of states
	3. check if current active process is finished
		see if it the last burst, if not swap to IO
		then do a context switch

	*/
	// std::vector<Process> fcfsAdding(processes);

	//std::queue<Process> fcfsProcessQueue;
/*
	Process currRunningProcess;
	bool currRunningProcessUsed = false;

	int time = 0;

	//instead of iterating through each ms, have a list of events that we just go to the first one?

	while(!fcfsAdding.empty() || !fcfsProcessQueue.empty()){

		//run the process
		if(currRunningProcessUsed){
			currRunningProcess.run(1);
		}



		time++;
	}*/


	//sort the vector by process ID?
	std::queue<Process *> fcfsQueue;
	std::vector<Process *> fcfsIOList;
	std::vector<Process *>::iterator IOitr;
	std::vector<Process *> IOtmp;

	//This is not a smart way to do things

	std::vector<Process>::iterator addingItr;
	int time = 0;
	while(numProcesses > 0){
		//std::cout <<"hello\n";

		/*if(!fcfsIOQueue.empty()){
			if(fcfsIOQueue.front().runIO(1) == 1){
				std::cout << "finished IO on process with ID " << fcfsIOQueue.front().processID << " and pop'd it at time: " << time << std::endl;
				if(fcfsIOQueue.front().curNumBursts > 0){
					fcfsQueue.push(fcfsIOQueue.front());
				}
				fcfsIOQueue.pop();
			}
		}*/

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

					cpu[i] = NULL;

					cpuCS[i] = t_cs/2;
				}
			} else {
				cpuCS[i]--;

				if(cpuCS[i] == 0 && cpu[i] != NULL){
					// TODO print "starting process" message
					std::cout<<"time "<<time<<"ms: Process "<<cpu[i]->processID<<" started using the CPU "
						<<printQueue(fcfsQueue)<<std::endl;
				}
			}
		}

		// // updating the cores
		// for (int i = 0; i < m; ++i) {
		// 	// std::cout<<time<<" cpucs " << i << " is " << cpuCS[i]<<std::endl;
		// 	if(cpuCS[i] == 0) {
		// 		if (cpu[i] == NULL) {
		// 			// the cpu core is currently idle
		// 			// try to give it a process to work on
		// 			if (!fcfsQueue.empty()) {
		// 				// set context switch time to half the normal context switch time
		// 				cpuCS[i] = t_cs/2;
		//
		// 				// give cpu core i a process from the front of the queue
		// 				// pop that process from the front the queue
		// 				cpu[i] = fcfsQueue.front();
		// 				fcfsQueue.pop();
		//
		// 				//TODO add output for starting to run process
		// 			}
		// 		} else if (cpu[i]->run(1) == 1) {
		//
		//
		// 			// process finished running
		// 			// try to replace it with another process
		// 			if (!fcfsQueue.empty()) {
		// 				cpuCS[i] = t_cs;
		// 				cpu[i] = fcfsQueue.front();
		// 				fcfsQueue.pop();
		// 			} else {
		// 				cpuCS[i] = t_cs/2;
		// 			}
		// 		}
		// 	} else {
		// 		cpuCS[i]--;
		//
		// 		if(cpuCS[i] == 0 && cpu[i] != NULL){
		// 			// TODO print "starting process" message
		// 			std::cout<<"time "<<time<<"ms: Process "<<cpu[i]->processID<<" started using the CPU "
		// 				<<printQueue(fcfsQueue)<<std::endl;
		// 		}
		// 	}
		// }

		IOitr = fcfsIOList.begin();
		while(IOitr != fcfsIOList.end()){
			//update IO for this process
			if((*IOitr)->runIO(1) == 1){
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
				cpu[i] = fcfsQueue.front();
				fcfsQueue.pop();

				cpuCS[i] += t_cs/2;
			}
		}

		// //run the first thing is the processing queue
		// if(!fcfsQueue.empty()){
		// 	if(fcfsQueue.front()->run(1) == 1){
		// 		//process finished running
		//
		// 		if(fcfsQueue.front()->curNumBursts <= 0){
		// 			//the process is done, stop running things
		// 			std::cout << "finished process with ID " << fcfsQueue.front()->processID << std::endl;
		// 		}else{
		// 			std::cout << "finished running process with ID " << fcfsQueue.front()->processID
		// 				<< " and pop'd it at time: " << time << "it has " << fcfsQueue.front()->curNumBursts << "remaining" << std::endl;
		// 			fcfsIOList.push_back(fcfsQueue.front());
		// 		}
		// 		fcfsQueue.pop();
		// 	}
		// }



		time++;
		//std::cout<<"new time "<<time<<std::endl;
		// if(time > 100)
		//  	exit(EXIT_FAILURE);
	}
}
