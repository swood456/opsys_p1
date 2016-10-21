#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>
#include "Process.h"

const int m = 1;		//number of processors available
const int t_cs = 8;		//time it takes to complete a context switch
const int t_slice = 84;	//time slice value

///////////////////////
//Function prototypes//
///////////////////////

void fcfs(std::vector<Process>, int, double, std::ofstream&);
void sjf(std::vector<Process>, int, double, std::ofstream&);
void roundRobin(std::vector<Process>, int, double, std::ofstream&);

//Function to print the contents of the wait queue
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

//Function to print the contents of the priority queue
std::string printQueue(std::priority_queue<Process *, std::vector<Process *>, ShortestJobFirstLessThan> q){
	if(q.empty()) {
		return "[Q empty]";
	}
	std::string output = "[Q";
	while (!q.empty()) {
		output += " " + q.top()->processID;
		q.pop();
	}

	return output + "]";
}

/////////////////
//Main function//
/////////////////
int main(int argc, char* argv[]){

	//check arguments
	if(argc != 3){
		fprintf(stderr, "ERROR: Invalid arguments\nUSAGE: ./a.out <input-file> <stats-output-file>\n" );
		return EXIT_FAILURE;
	}

	//open argv[1] to read file
	std::ifstream inFile;
	inFile.open(argv[1]);

	//make variables to store input
	std::string line;
	std::vector<Process> processes;


	//check to see if the file is able to be opened
	if(!inFile){
		fprintf(stderr, "ERROR: Invalid input file format\n" );
		return EXIT_FAILURE;
	}

	//count the total number of bursts and the average time of each burst
	int numBursts = 0;
	double avgBurstTime = 0;

	//go through the file line by line
	while(std::getline(inFile, line)){
		if(line[0] != '#'){

			//break the input line down to be just the things we care about
			std::string processID;
			int processInfo [4];

			//Look for | to break the input string down
			std::string delimiter = "|";

			size_t pos = 0;
			std::string token;

			//find the first | in the string, save its location
			pos = line.find(delimiter);

			//find the substring from the start of the string to where the first | is
			processID = line.substr(0, pos);

			//remove everything up to and including the first |
			line.erase(0, pos + delimiter.length());

			//loop through and find the 4 remaining int values
			for(int i = 0; i < 4; i++){

				//again, find the first |
				pos = line.find(delimiter);

				//make that section into a string
				token = line.substr(0, pos);

				//convert the string into an int using atoi
				processInfo[i] = atoi(token.c_str());

				//remove the substring
			    line.erase(0, pos + delimiter.length());

			}

			//push the new Process object onto the vector of processes
			processes.push_back(Process(processID, processInfo[0], processInfo[1], processInfo[2], processInfo[3]));
			
			//keep track of the number of bursts and the burst time for the process
			numBursts += processInfo[2];
			avgBurstTime += processInfo[1] * processInfo[2];
		}
	}

	//calculate the average burst time
	avgBurstTime =  (double)avgBurstTime / (double)numBursts;

	//close the input file
	inFile.close();

	//open the ouput file
	std::ofstream outputFile;
	outputFile.open(argv[2]);

	//set up the output file to output decimal numbers with only 2 decimal places (rounded)
	outputFile << std::fixed << std::setprecision(2);

	//run First Come First Serve
	fcfs(processes, numBursts, avgBurstTime, outputFile);

	// run Shortest Job First
	sjf(processes, numBursts, avgBurstTime, outputFile);

	// run Round Robin
	roundRobin(processes, numBursts, avgBurstTime, outputFile);

	//close the output file
	outputFile.close();

	//exit
	return EXIT_SUCCESS;
}

//////////////////////////
//First-Come-First-Serve//
//////////////////////////


void fcfs(std::vector<Process> fcfsAdding, int numBursts, double avgBurstTime, std::ofstream& outputFile) {

	std::cout<<"time 0ms: Simulator started for FCFS [Q empty]"<<std::endl;

	unsigned int numProcesses = fcfsAdding.size();
	std::vector<Process *> cpu(m);	// stores all the running processes
	std::vector<int> cpuCS(m);		// countdown timers for context switches for each core

	double	avgWaitTime = 0;
	double	avgTurnTime = 0;
	int		numContextSwitches = 0;


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

	std::queue<Process *> fcfsQueue;			// the ready queue
	std::vector<Process *> fcfsIOList;			// list of processes blocked by I/O
	std::vector<Process *>::iterator IOitr;		// iterator for fcfsIOList
	std::vector<Process *> IOtmp;				// temp IO list for newly blocked processes
	std::vector<Process>::iterator addingItr;	// iterator for the fcfsAdding vector

	// remove any finished processes from the CPU
	int time = 0;
	while(numProcesses > 0){
		// iterate through all the cores
		for (int i = 0; i < m; ++i) {
			// if the core is not undergoing a context switch
			if (cpuCS[i] == 0) {
				// if there is a process in the core, run it 1 tick and see if it's done
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

						// add to list of blocked I/O processes
						IOtmp.push_back(cpu[i]);
					}

					// increment the average turnaround time
					avgTurnTime += time - cpu[i]->burstArrivalTime;

					cpu[i] = NULL;

					cpuCS[i] = t_cs/2;
				}
			} else {
				// we are undergoing a context switch, so decrement the context switch timer
				cpuCS[i]--;

				if(cpuCS[i] == 0 && cpu[i] != NULL){
					std::cout<<"time "<<time<<"ms: Process "<<cpu[i]->processID<<" started using the CPU "
						<<printQueue(fcfsQueue)<<std::endl;
				}
			}
		}

		// iterate through all the I/O blocked processes
		IOitr = fcfsIOList.begin();
		while(IOitr != fcfsIOList.end()){
			// update IO for this process
			// if it's done running I/O, put it back on the ready queue
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

		// add newly blocked processes to the I/O list
		fcfsIOList.insert(fcfsIOList.end(), IOtmp.begin(), IOtmp.end());
		IOtmp.clear();

		//look to see if any processes arrive
		addingItr = fcfsAdding.begin();
		while(addingItr != fcfsAdding.end()){

			if(addingItr->initialArrivalTime == time){
				fcfsQueue.push(&(*addingItr));

				std::cout<<"time "<<time<<"ms: Process "<<addingItr->processID
					<<" arrived "<<printQueue(fcfsQueue)<<std::endl;
			}

			addingItr++;
		}

		// iterate through all the cores and try to give them a process if they are idle
		for (int i = 0; i < m; ++i) {
			if (cpu[i] == NULL && !fcfsQueue.empty()) {
				// increment number of context switches
				numContextSwitches++;

				// increment average wait time
				avgWaitTime += time - fcfsQueue.front()->burstArrivalTime;

				cpu[i] = fcfsQueue.front();
				fcfsQueue.pop();

				cpuCS[i] += t_cs/2;
			}
		}

		time++;
	}

	// output statistics to the output file
	outputFile << "Algorithm FCFS\n-- average CPU burst time: " << avgBurstTime <<" ms\n-- average wait time: " <<
		avgWaitTime/numBursts<<" ms\n"<< "-- average turnaround time: "<<avgTurnTime/numBursts<<
		" ms\n-- total number of context switches: "<<numContextSwitches<<"\n-- total number of preemptions: 0\n";

	std::cout<<"time "<<time+ t_cs/2 - 1<<"ms: Simulator ended for FCFS"<<std::endl<<std::endl;

}


	/////////////////////
	//Shorest-Job-First//
	/////////////////////

void sjf(std::vector<Process> sjfAdding, int numBursts, double avgBurstTime, std::ofstream& outputFile) {

	std::cout<<"time 0ms: Simulator started for SJF [Q empty]"<<std::endl;

	//count of the number of processes still running. We stop when this is 0
	unsigned int numProcesses = sjfAdding.size();

	std::vector<Process *> cpu(m);	// stores all the running processes
	std::vector<int> cpuCS(m);		// countdown timers for context switches for each core

	//variables to hold onto the statistics
	double	avgWaitTime = 0;
	double	avgTurnTime = 0;
	int		numContextSwitches = 0;


	//ready queue, uses a priority queue that sorts by process burst time
	std::priority_queue<Process*, std::vector<Process*>, ShortestJobFirstLessThan> sjfQueue;
	
	//vector of Processes that are currently blocked on I/O
	std::vector<Process *> sjfIOList;

	//Iterator to go through the list of IO variables
	std::vector<Process *>::iterator IOitr;

	//Vecotr that is used to make sure that Processes do not have I/O run on them the same tick they finish on the CPU
	std::vector<Process *> IOtmp;

	//Iterator to go through all the processes that still need to be added
	std::vector<Process>::iterator addingItr;

	//int value to keep track of what ms we are on now
	int time = 0;

	//loop until all processes are finished running
	while(numProcesses > 0){

		//go through every CPU core
		for (int i = 0; i < m; ++i) {
			//if the current core is not waiting on a context switch
			if (cpuCS[i] == 0) {

				//if the current CPU is not pointing to NULL (there is some process running on it)
				//	run the Process for 1 ms and check to see if it finishes
				if (cpu[i] != NULL && cpu[i]->run(1) == 1) {
					//the process has completed its burst, then see if it is actualy done or has some I/O to do
					if (cpu[i]->curNumBursts <= 0) {
						//the Process is completely done, print out that it is done
						std::cout<<"time "<<time<<"ms: Process "<<cpu[i]->processID<<" terminated "
							<<printQueue(sjfQueue)<<std::endl;

						//decrement the number of running processes
						numProcesses--;
					} else {
						//the process still has more things to do, put it into I/O
						std::cout<<"time "<<time<<"ms: Process "<<cpu[i]->processID<<" completed a CPU burst; "
							<<cpu[i]->curNumBursts<<" to go "<<printQueue(sjfQueue)<<std::endl;

						std::cout<<"time "<<time<<"ms: Process "<<cpu[i]->processID<<" blocked on I/O until time "
							<<time+cpu[i]->ioTime<<"ms "<<printQueue(sjfQueue)<<std::endl;

						//put the Process into a temp I/O queue so that it does not run I/O this tick
						IOtmp.push_back(cpu[i]);
					}

					//regardless of if the process is completely done or has I/O, do these things

					//increment the average tunraround time
					avgTurnTime += time - cpu[i]->burstArrivalTime;

					//free up this cpu core
					cpu[i] = NULL;

					//se up a half context switch
					cpuCS[i] = t_cs/2;
				}
			} else {
				//We are currently doing a context switch

				//decrement the context switch counter
				cpuCS[i]--;

				//if we just reach 0, that means that a process can start doing work on the CPU
				if(cpuCS[i] == 0 && cpu[i] != NULL){
					std::cout<<"time "<<time<<"ms: Process "<<cpu[i]->processID<<" started using the CPU "
						<<printQueue(sjfQueue)<<std::endl;
				}
			}
		}


		//iterate through the vector of objects in I/O
		IOitr = sjfIOList.begin();
		while(IOitr != sjfIOList.end()){
			
			//run I/O on this process for 1 ms. See if it finished I/O
			if((*IOitr)->runIO(1) == 1){
				//the Process is done with I/O

				//set the time it enters into the ready queue to now
				(*IOitr)->burstArrivalTime = time;

				//add the process back into the ready queue
				sjfQueue.push(*IOitr);

				//print that the object has finished I/O
				std::cout<<"time "<<time<<"ms: Process "<< (*IOitr)->processID
					<<" completed I/O "<<printQueue(sjfQueue)<<std::endl;

				//remove the process from the I/O list
				sjfIOList.erase(IOitr);

			}
			else{
				//Process is not done with I/O, so look at next object
				IOitr++;
			}
		}

		//add all the objects that just finished CPU time this tick into the real I/O list
		sjfIOList.insert(sjfIOList.end(), IOtmp.begin(), IOtmp.end());

		//clear the holding list
		IOtmp.clear();

		//look to see if any processes arrived at this time
		addingItr = sjfAdding.begin();
		while(addingItr != sjfAdding.end()){

			//if a process arrives at the current time
			if(addingItr->initialArrivalTime == time){

				//push the object into the ready queue
				sjfQueue.push(&(*addingItr));

				//annoucnce that it arrives
				std::cout<<"time "<<time<<"ms: Process "<<addingItr->processID
					<<" arrived "<<printQueue(sjfQueue)<<std::endl;
			}
			addingItr++;
			
		}

		// Add objects to CPU if the CPU is ready for them
		for (int i = 0; i < m; ++i) {
			if (cpu[i] == NULL && !sjfQueue.empty()) {
				numContextSwitches++;
				avgWaitTime += time - sjfQueue.top()->burstArrivalTime;

				cpu[i] = sjfQueue.top();
				sjfQueue.pop();

				cpuCS[i] += t_cs/2;
			}
		}

		time++;
	}

	outputFile << "Algorithm SJF\n-- average CPU burst time: " << avgBurstTime <<" ms\n-- average wait time: " <<
		avgWaitTime/numBursts<<" ms\n"<< "-- average turnaround time: "<<avgTurnTime/numBursts<<
		" ms\n-- total number of context switches: "<<numContextSwitches<<"\n-- total number of preemptions: 0\n";

	std::cout<<"time "<<time+ t_cs/2 - 1<<"ms: Simulator ended for SJF"<<std::endl<<std::endl;

}

void roundRobin(std::vector<Process> rrAdding, int numBursts, double avgBurstTime, std::ofstream& outputFile) {

	std::cout<<"time 0ms: Simulator started for RR [Q empty]"<<std::endl;

	unsigned int numProcesses = rrAdding.size();
	std::vector<Process *> cpu(m);	// stores all the running processes
	std::vector<int> cpuCS(m);		// countdown timers for context switches for each core

	double	avgWaitTime = 0;
	double	avgTurnTime = 0;
	int		numContextSwitches = 0;
	int		numPreemptions = 0;

	///////////////
	//Round-Robin//
	///////////////

	
	std::queue<Process *> fcfsQueue;
	std::vector<Process *> sjfIOList;
	std::vector<Process *>::iterator IOitr;
	std::vector<Process *> IOtmp;
	std::vector<int> rrTimers(m);
	for(int i = 0; i < m; i++){
		rrTimers[i] = t_slice;
	}

	std::vector<Process>::iterator addingItr;
	int time = 0;
	while(numProcesses > 0){

		for (int i = 0; i < m; ++i) {
			if (cpuCS[i] == 0) {
				if (cpu[i] != NULL){
					rrTimers[i]--;
					if(cpu[i]->run(1) == 1) {
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
						rrTimers[i] = t_slice;
					} else if(rrTimers[i] == 0){
						//time slice is over, preempt if the queue is not empty
						if(fcfsQueue.empty()){
							//do not preempt, just reset timer and say something
							std::cout << "time "<<time<<"ms: Time slice expired; no preemption because ready queue is empty [Q empty]" << std::endl;
							rrTimers[i] = t_slice;
						} else{

							//preempt
							++numPreemptions;
							++numContextSwitches;

							avgTurnTime += time - cpu[i]->burstArrivalTime;
							avgWaitTime += time - fcfsQueue.front()->burstArrivalTime;
							cpu[i]->burstArrivalTime = time;

							//push the current process to the back of the queue
							fcfsQueue.push(cpu[i]);

							std::cout << "time "<<time<<"ms: Time slice expired; process "<< cpu[i]->processID <<
								" preempted with "<< cpu[i]->curProcessTime <<"ms to go " <<printQueue(fcfsQueue) << std::endl;
							//context switch into the new process
							cpu[i] = fcfsQueue.front();
							fcfsQueue.pop();
							cpuCS[i] = t_cs;

							//reset timeslice
							rrTimers[i] = t_slice;
						}
					}
				}
			} else {
				cpuCS[i]--;

				if(cpuCS[i] == 0 && cpu[i] != NULL){
					std::cout<<"time "<<time<<"ms: Process "<<cpu[i]->processID<<" started using the CPU "
						<<printQueue(fcfsQueue)<<std::endl;
				}
			}
		}

		IOitr = sjfIOList.begin();
		while(IOitr != sjfIOList.end()){
			//update IO for this process
			if((*IOitr)->runIO(1) == 1){
				(*IOitr)->burstArrivalTime = time;

				//add the process back into the queue
				fcfsQueue.push(*IOitr);

				std::cout<<"time "<<time<<"ms: Process "<< (*IOitr)->processID
					<<" completed I/O "<<printQueue(fcfsQueue)<<std::endl;

				//remove the process from the IO list
				sjfIOList.erase(IOitr);

			}
			else{
				IOitr++;
			}
		}

		sjfIOList.insert(sjfIOList.end(), IOtmp.begin(), IOtmp.end());
		IOtmp.clear();

		//look to see if any processes arrive
		addingItr = rrAdding.begin();
		while(addingItr != rrAdding.end()){

			if(addingItr->initialArrivalTime == time){
				fcfsQueue.push(&(*addingItr));

				std::cout<<"time "<<time<<"ms: Process "<<addingItr->processID
					<<" arrived "<<printQueue(fcfsQueue)<<std::endl;

				//rrAdding.erase(addingItr);
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

				cpu[i] = fcfsQueue.front();
				fcfsQueue.pop();

				cpuCS[i] += t_cs/2;
			}
		}

		time++;
	}

	//outputFile << "hello from fcfs\n";
	outputFile << "Algorithm RR\n-- average CPU burst time: " << avgBurstTime <<" ms\n-- average wait time: " <<
		avgWaitTime/numBursts<<" ms\n"<< "-- average turnaround time: "<<avgTurnTime/numBursts<<
		" ms\n-- total number of context switches: "<<numContextSwitches<<"\n-- total number of preemptions: " <<
		numPreemptions;

	std::cout<<"time "<<time+ t_cs/2 - 1<<"ms: Simulator ended for RR"<<std::endl;

}
