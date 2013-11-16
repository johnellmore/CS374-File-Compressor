#include "fileactions.h"
#include "compress.h"

using namespace std;

int main(int argc, char *argv[]) {
	// require at least one argument
	if (argc < 2) {
		cerr << "Too few arguments." << endl;
		cerr << "For usage, please run:\n\tcompressor --help" << endl;
		return EXIT_FAILURE;
	}

	// specify defaults
	list<string> inputFiles;
	unsigned int action = 1; // 1 == compress, 2 == expand
	unsigned int algorithm = 1; // default is just a tarball (no compression)
	string outputHandle = "compressed.cmp"; // default output file

	// parse optional arguments
	for (int a = 1; a < argc; a++) {
		string arg(argv[a]);
		if (arg == "-e") {
			// -e means we want to expand an existing archive (versus compressing)
			action = 2;
		} else if (arg == "-a") {
			// choose algorithm
			arg.assign(argv[++a]);
			if (arg == "none")
				algorithm = 1;
			else if (arg == "base64")
				algorithm = 2;
			else if (arg == "lzw")
				algorithm = 3;
			else {
				cerr << "Invalid algorithm: " << arg << endl;
				cerr << "For usage, please run:\n\tcompressor --help" << endl;
				return EXIT_FAILURE;
			}
		} else if (arg == "-o") {
			// choose output filename
			outputHandle.assign(argv[++a]);
		} else if (arg == "--help") {
			cerr << "Someday, there will be usage information." << endl;
			return EXIT_SUCCESS;
		} else {
			// should be an input file
			inputFiles.push_back(arg);
		}
	}

	// validate the number of input files
	if (action == 2 && inputFiles.size() != 1) {
		cerr << "Expanding requires exactly one input file." << endl;
		cerr << "For usage, please run:\n\tcompressor --help" << endl;
		return EXIT_FAILURE;
	} else if (action == 1 && inputFiles.size() < 1) {
		cerr << "Compression requires at least one input file." << endl;
		cerr << "For usage, please run:\n\tcompressor --help" << endl;
		return EXIT_FAILURE;
	}

	// make sure the input files exist and are valid inputs
	for (list<string>::iterator it = inputFiles.begin(); it != inputFiles.end(); ++it) {
		// remove trailing slash from file
		string fileName(*it);
		while (fileName[fileName.size()-1] == '/')
			fileName.erase(fileName.size()-1);
		*it = fileName;

		// get file info
		struct stat fileStat;
		int statSuccess = stat(fileName.c_str(), &fileStat);

		// if the file doesn't exist, end program in failure
		if (statSuccess < 0) {
			cerr << "Could not get information on input file \"";
			cerr << fileName;
			cerr << "\". Exiting." << endl;
			return EXIT_FAILURE;
		}
		// disallow symbolic links
		if (S_ISLNK(fileStat.st_mode)) {
			cerr << "File \"" << fileName << "\" is a symbolic link. Exiting." << endl;
			return EXIT_FAILURE;
		}
	}

	// debug
	// cerr << "Input files:" << endl;
	// for (list<string>::iterator it = inputFiles.begin(); it != inputFiles.end(); ++it)
	// 	cerr << "\t" << *it << endl;
	// cerr << "Algorithm: " << algorithm << endl;
	// cerr << "Output: " << outputHandle << endl;

	// Done with the input validation. Let's start processing!
	try {
		if (action == 1)
			compressIntoArchive(inputFiles, outputHandle, algorithm);
		else
			expandFromArchive(*(inputFiles.begin()));
	} catch (exception &e) {
		cerr << "An error occurred:" << endl << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}