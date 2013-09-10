#include "fileobjects.h"

using namespace std;

Archive::Archive(const string archive, const string output)
  : archiveHandle(archive),
	outputHandle(output) {};

void Archive::compress(unsigned int algo) {
	// TODO: build tree of files, implement tarring and compressing
}

void Archive::expand() {
	// TODO: decompress using file signature, then untar, rebuild tree, and write
}