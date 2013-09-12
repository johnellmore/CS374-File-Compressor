#include <iostream>
#include <cassert>
#include <fstream>
using namespace std;

bool isSame(string input, string output){
	int size = 1;
	char* inputBuffer;
	char* outputBuffer;
	inputBuffer = new char[size];
	outputBuffer = new char[size];
	fstream inputFile;
	fstream outputFile;
	inputFile.open(input.c_str(), ios::out | ios::in | ios::binary);
	outputFile.open(output.c_str(), ios::out | ios::in | ios::binary);
	while(!inputFile.eof() && !outputFile.eof()){
		if(inputFile.eof() || outputFile.eof()){
			return false;
		}
		inputFile.read(inputBuffer, size);
		outputFile.read(outputBuffer, size);
		if(inputBuffer[0] != outputBuffer[0])
			return false;
	}
	delete[] inputBuffer;
	delete[] outputBuffer;
	inputFile.close();
	outputFile.close();
	return true;
}

char* compress(char* buffer){
	return buffer;
}

char* decompress(char* buffer){
	return buffer;
}

void cycle(string input, string output, string mode){
	char* buffer;
	int size = 1;
	fstream inputFile;
	fstream outputFile;
	inputFile.open(input.c_str(), ios::out | ios::in | ios::binary);
	outputFile.open(output.c_str(), ios::out);
	outputFile.close();
	outputFile.open(output.c_str(), ios::out | ios::in | ios::binary);
	buffer = new char[size];
	inputFile.read(buffer, size);
	while(!inputFile.eof()){
		if(mode == "compress"){
			compress(buffer);
		}
		else if(mode == "decompress"){
			decompress(buffer);
		}
		outputFile.write(buffer,size);
		inputFile.read(buffer, size);
	}
	delete[] buffer;
	inputFile.close();
	outputFile.close();
}

int main(){
	string inputFile = "input.txt"; 
	string compressedFile = "compressed.txt";
	string outputFile = "output.txt";
	cycle(inputFile, compressedFile, "compress");
	cycle(compressedFile, outputFile, "decompress");
	assert(isSame(inputFile, outputFile));
}