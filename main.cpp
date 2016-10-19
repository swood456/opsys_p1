#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>


class Process{

private:
	std::string processID;
	int initialArrivalTime;
	int totalCpuBurstTime;
	int totalNumBursts;
	int ioTime;

public:
	Process(std::string _processID, int _initialArrivalTime, int _totalCpuBurstTime, int _totalNumBursts, int _ioTime){
		processID = _processID;
		initialArrivalTime = _initialArrivalTime;
		totalCpuBurstTime = _totalCpuBurstTime;
		totalNumBursts = _totalNumBursts;
		ioTime = _ioTime;
	}

	void print(){
		std::cout << processID << " " << initialArrivalTime << " " << totalCpuBurstTime << " " << totalNumBursts << " " << ioTime << std::endl;
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

	//do First-Come-First-Serve

	//do Shortest Job First

	//do Round Robin

	
	//std::cout << "hello world" << std::endl;



	return EXIT_SUCCESS;
}

