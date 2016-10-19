#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <queue>

const int m = 1;		//number of processors available
const int t_cs = 8;		//time it takes to complete a context switch
const int t_slice = 84;	//time slice value


class Process{
//these should be made private eventually, but they never will be
public:
	std::string processID;
	int initialArrivalTime;
	int totalCpuBurstTime;
	int totalNumBursts;
	int ioTime;

	int curNumBursts;
	int curIOTime;
	int curProcessTime;

public:
	Process(){
		processID = "";
		initialArrivalTime = -1;
		totalCpuBurstTime = -1;
		totalNumBursts = -1;
		ioTime = -1;

		curNumBursts = -1;
		curIOTime = -1;
		curProcessTime = -1;
	}

	Process(std::string _processID, int _initialArrivalTime, int _totalCpuBurstTime, int _totalNumBursts, int _ioTime){
		processID = _processID;
		initialArrivalTime = _initialArrivalTime;
		totalCpuBurstTime = _totalCpuBurstTime;
		totalNumBursts = _totalNumBursts;
		ioTime = _ioTime;

		curNumBursts = totalNumBursts;
		curIOTime = ioTime;
		curProcessTime = totalCpuBurstTime;
	}

	void print(){
		std::cout << processID << " " << initialArrivalTime << " " << totalCpuBurstTime << " " << totalNumBursts << " " << ioTime << std::endl;
	}

	int run(int duration){
		curProcessTime -= duration;

		if(curProcessTime <= 0){
			curNumBursts--;
			curProcessTime = totalCpuBurstTime;
			return 1;
		}
		return 0;
	}

	int runIO(int duration){
		curIOTime -= duration;

		if(curIOTime <= 0){
			curIOTime = ioTime;
			return 1;
		}
		return 0;
	}
};

struct FirstComeLessThan{
	bool operator()(const Process& lhs, const Process& rhs) const{
		if(lhs.initialArrivalTime == rhs.initialArrivalTime){
			return lhs.processID > rhs.processID;
		}
		return lhs.initialArrivalTime > rhs.initialArrivalTime;
	}
};


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

	std::vector<Process> fcfsAdding(processes);

	std::queue<Process> fcfsProcessQueue;
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
	}

	/*
	//sort the vector by process ID?
	std::queue<Process> fcfsQueue;
	std::queue<Process> fcfsIOQueue;

	//This is not a smart way to do things

	std::vector<Process>::iterator addingItr;
	int time = 0;
	while(!fcfsAdding.empty() || !fcfsQueue.empty() || !fcfsIOQueue.empty()){
		//std::cout <<"hello\n";
		if(!fcfsIOQueue.empty()){
			if(fcfsIOQueue.front().runIO(1) == 1){
				std::cout << "finished IO on process with ID " << fcfsIOQueue.front().processID << " and pop'd it at time: " << time << std::endl;
				if(fcfsIOQueue.front().curNumBursts > 0){
					fcfsQueue.push(fcfsIOQueue.front());
				}
				fcfsIOQueue.pop();
			}
		}


		//run the first thing is the processing queue
		if(!fcfsQueue.empty()){
			if(fcfsQueue.front().run(1) == 1){
				std::cout << "finished running process with ID " << fcfsQueue.front().processID << " and pop'd it at time: " << time << std::endl;
				fcfsIOQueue.push(fcfsQueue.front());

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
	}
	*/
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

