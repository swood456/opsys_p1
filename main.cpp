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
	std::vector<Process> fcfsAdding(processes);

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
	std::queue<Process> fcfsQueue;
	std::vector<Process> fcfsIOList;
	std::vector<Process>::iterator IOitr;

	//This is not a smart way to do things

	std::vector<Process>::iterator addingItr;
	int time = 0;
	while(!fcfsAdding.empty() || !fcfsQueue.empty() || !fcfsIOList.empty()){
		//std::cout <<"hello\n";

		IOitr = fcfsIOList.begin();
		while(IOitr != fcfsIOList.end()){
			//update IO for this process
			if(IOitr->runIO(1) == 1){
				//add the process back into the queue
				fcfsQueue.push(*IOitr);

				//remove the process from the IO list
				fcfsIOList.erase(IOitr);

			}
		}
		/*if(!fcfsIOQueue.empty()){
			if(fcfsIOQueue.front().runIO(1) == 1){
				std::cout << "finished IO on process with ID " << fcfsIOQueue.front().processID << " and pop'd it at time: " << time << std::endl;
				if(fcfsIOQueue.front().curNumBursts > 0){
					fcfsQueue.push(fcfsIOQueue.front());
				}
				fcfsIOQueue.pop();
			}
		}*/



		//run the first thing is the processing queue
		if(!fcfsQueue.empty()){
			if(fcfsQueue.front().run(1) == 1){
				//process finished running

				if(fcfsQueue.front().curNumBursts <= 0){
					//the process is done, stop running things
					std::cout << "finished process with ID " << fcfsQueue.front().processID << std::endl;
				}else{
					std::cout << "finished running process with ID " << fcfsQueue.front().processID << " and pop'd it at time: " << time << "it has " << fcfsQueue.front().curNumBursts << "remaining" << std::endl;
					fcfsIOList.push_back(fcfsQueue.front());
				}
				fcfsQueue.pop();
			}
		}

		//look to see if any processes arrive
		addingItr = fcfsAdding.begin();
		while(addingItr != fcfsAdding.end()){

			if(addingItr->initialArrivalTime == time){

				fcfsQueue.push(*addingItr);
				fcfsAdding.erase(addingItr);

				std::cout << "added a thing\n";

			} else{
				addingItr++;
			}
		}



		time++;

		if(time > 99999)
			return EXIT_FAILURE;
	}

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
