#include <iostream>
#include <cassert>
#include <fstream>
#include <map>
#include <vector>
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

map<unsigned int, vector<char> > initializeDictionary(){
	map<unsigned int, vector<char> > dictionary;
	vector<char> append;
	for(unsigned int i = 0; i < 256; i++){
		append.push_back((char)i);
		pair<unsigned int, vector<char> > temp = make_pair<unsigned int, vector<char> >(i, append);
		dictionary.insert(temp);
		append = dictionary[i];
		append.clear();
	}
	for(unsigned int i = 0; i < 256; i++){
		assert(dictionary.at(i).at(0) == (char) i);
	}
	return dictionary;
}

char* compress(char* buffer){
	return buffer;
}

char* decompress(char* buffer){
	return buffer;
}

void cycle(string input, string output, string mode){
	map<unsigned int, vector<char> >  dictionary = initializeDictionary();
	
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
