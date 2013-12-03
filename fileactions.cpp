#include "fileactions.h"
#include "compress.h"

using namespace std;

string extractString(istream &stream, int getLength) {
	string output;
	char buffer[4096];
	while (getLength > 0 && !stream.eof()) {
		stream.read(buffer, min(getLength, 4096));
		int charsGotten = stream.gcount();
		output.append(buffer, charsGotten);
		getLength -= charsGotten;
	}
	return output;
}

void compressIntoArchive(list<string> inputFiles, string outputFile, unsigned int algorithm) {
	stringstream archive;

	// print the number of files in the archive
	archive << inputFiles.size() << " ";

	// build archive string from input files
	for (list<string>::iterator file = inputFiles.begin(); file != inputFiles.end(); ++file) {
		int stripPrefix = (*file).find_last_of('/') + 1;
		addObjectToArchive(*file, archive, stripPrefix);
	}

	// DO COMPRESSION
	stringstream compressed;
	if (algorithm == 1) {
		// do no compression
		compressed << archive.rdbuf();
	} else if (algorithm == 2) {
		// do base64 encoding
	} else if (algorithm == 3) {
		// do LZW compression
		LZW compressor;
		compressor.compress(archive, compressed);
	}

	// write file to disk
	ofstream file(outputFile.c_str(), ofstream::trunc);
	if (!file.is_open())
		throw runtime_error(std::string("Could not create output file \"") + outputFile + "\".");

	file << algorithm << " "; // write the algorithm used to the beginning of the file
	file << compressed.rdbuf();

	file.close();
}

void expandFromArchive(string inputFile) {
	// open the archive file
	ifstream file(inputFile.c_str());
	if (!file.is_open())
		throw runtime_error(std::string("Could not open input file \"") + inputFile + "\".");

	// get the algorithm used
	char algorithm[] = "xx";
	file.get(algorithm, 2);
	file.ignore();

	// DECOMPRESS
	stringstream archive;
	if (algorithm[0] == '1') {
		// no compression method, just concatenation
		archive << file.rdbuf();
	} else if (algorithm[0] == '2') {
		// do base64 decoding
	} else if (algorithm[0] == '3') {
		// do LZW decompression
		LZW decompressor;
		stringstream full;
		full << file.rdbuf();
		//cout << full.str();
		//return;
		decompressor.decompress(full, archive);
	}
	file.close();

	// build files from archive string
	int numObjects;
	archive >> numObjects;
	for (int i = 0; i < numObjects; i++)
		makeObjectFromArchive(archive);
}

void addObjectToArchive(string handle, stringstream &archive, int stripPrefix) {
	// build internal file name
	string shortHandle(handle, stripPrefix);

	// build file header
	archive << shortHandle.size() << " "; // length of filename
	archive << shortHandle << " "; // filename

	// determine the type of the file
	struct stat fileStat;
	int statSuccess = stat(handle.c_str(), &fileStat);
	if (S_ISDIR(fileStat.st_mode)) {
		// IS A DIRECTORY
		archive << "dir ";

		// open the directory
		DIR* dp = opendir(handle.c_str());
		struct dirent *ep;
		if (dp == NULL)
			throw runtime_error(std::string("Could not open directory \"") + handle + "\".");

		// count files
		unsigned int numObjects = 0;
		while (ep = readdir(dp)) {
			if ((string)"." == ep->d_name || (string)".." == ep->d_name) continue;
			numObjects++;
		}
		rewinddir(dp);
		archive << numObjects << " ";

		// read in directory entries to the string
		while (ep = readdir(dp)) {
			if ((string)"." == ep->d_name || (string)".." == ep->d_name) continue;

			// build file name
			string fullFilename = handle + "/" + ep->d_name;
			addObjectToArchive(fullFilename, archive, stripPrefix);
		}

		// we're done with this directory
		closedir(dp);

	} else if (S_ISREG(fileStat.st_mode)) {
		// IS A NORMAL FILE
		archive << "file ";

		// open the file for reading
		ifstream file(handle.c_str(), ifstream::binary);
		if (!file.is_open())
			throw runtime_error(std::string("Could not open input file \"") + handle + "\".");

		// get file length
		file.seekg(0, ifstream::end);
		unsigned int fileLength = file.tellg();
		archive << fileLength << " ";

		// read file to the stream
		if (fileLength > 0) {
			file.seekg(0, ifstream::beg);
			archive << file.rdbuf();
		}
		archive << " ";

		// we're done with this file
		file.close();

	} else // IS UNKNOWN OBJECT TYPE
		throw runtime_error(std::string("File entry \"") + handle + "\" is of unknown type.");
}

void makeObjectFromArchive(stringstream &archive) {

	// get length of filename
	int handleLength;
	archive >> handleLength;
	archive.ignore();

	// get filename
	string filename;
	filename = extractString(archive, handleLength);

	// get type
	string type;
	archive >> type;

	cerr << "HANDLING: " << filename << " of type " << type << endl;

	if (type == "dir") {
		// HANDLING A DIRECTORY

		// create the directory
		if (mkdir(filename.c_str(), 0777) != 0)
			throw runtime_error(std::string("Error creating directory: \"") + filename + "\".");

		// get number of directory entries
		int numObjects;
		archive >> numObjects;

		// process each directory entry
		for (int i = 0; i < numObjects; i++)
			makeObjectFromArchive(archive);

	} else if (type == "file") {
		// HANDLING A FILE

		// get the length of the content block
		int contentLength;
		archive >> contentLength;
		archive.ignore();

		// create the file
		ofstream file(filename.c_str(), ofstream::trunc);
		if (!file.is_open())
			throw runtime_error(std::string("Could not recreate file \"") + filename + "\".");

		// write the contents to the file
		string contents = extractString(archive, contentLength);
		file.write(contents.c_str(), contents.size());
		file.close();

	} else 
		throw runtime_error(std::string("Unknown object type: \"") + filename + "\".");
}