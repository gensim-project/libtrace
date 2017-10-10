#include "RecordFile.h"
#include "InstructionPrinter.h"

#include <iostream>
#include <cstdio>

int main(int argc, char **argv)
{
	if(argc == 1) {
		fprintf(stderr, "Usage: %s [record file]\n", argv[0]);
		return 1;
	}
	
	FILE *rfile = fopen(argv[1], "r");
	if(!rfile) {
		perror("Could not open file");
		return 1;
	}
	
	RecordFile rf (rfile);
	
	auto begin = rf.begin();
	auto end = rf.end();
	
	InstructionPrinter ip;
	
	while(begin != end) {
		std::cout << ip(begin, end) << std::endl;
		
	}
	
	return 0;
}
