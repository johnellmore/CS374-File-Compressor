#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <algorithm>
#include <stdexcept>
#include <stdio.h> // directories
#include <dirent.h> // directories
#include <unistd.h> // stat function
#include <sys/stat.h> // stat function
#include <sys/types.h> // stat function

std::string extractString(std::istream &stream, int getLength);

void compressIntoArchive(std::list<std::string> inputFiles, std::string outputFile, unsigned int algorithm);
void expandFromArchive(std::string inputFile);
void addObjectToArchive(std::string handle, std::stringstream &output, int stripPrefix);
void makeObjectFromArchive(std::stringstream &archive);