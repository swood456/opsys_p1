#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>



int main(int argc, char* argv[]){

	//check arguments
	if(argc != 3){
		fprintf(stderr, "ERROR: Invalid arguments\nUSAGE: ./a.out <input-file> <stats-output-file>\n" );
		return EXIT_FAILURE;
	}

	//open argv[1] to read file
	std::ifstream inFile;
	inFile.open(argv[1]);

	//check to see if the file is able to be opened
	if(!inFile){
		fprintf(stderr, "ERROR: Invalid input file format\n" );
		return EXIT_FAILURE;
	}

	inFile.close();



	
	std::cout << "hello world" << std::endl;



	return EXIT_SUCCESS;
}

class Process{

private:
	int processID;
	int initialArrivalTime;
	int totalCpuBurstTime;
	int totalNumBursts;
	int ioTime;

public:
	Process(int _processID, int _initialArrivalTime, int _totalCpuBurstTime, int _totalNumBursts, int _ioTime){
		processID = _processID;
		initialArrivalTime = _initialArrivalTime;
		totalCpuBurstTime = _totalCpuBurstTime;
		totalNumBursts = _totalNumBursts;
		ioTime = _ioTime;
	}
};