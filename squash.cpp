#include "fileobjects.h"

using namespace std;

int main(int argc, char *argv[]) {
	// require at least one argument
	if (argc < 2) {
		cerr << "Too few arguments." << endl;
		cerr << "For usage, please run:\n\tcompressor --help" << endl;
		return EXIT_FAILURE;
	}

	// specify defaults
	string inputHandle("");
	unsigned int action = 1; // 1 == compress, 2 == expand
	unsigned int algorithm = 1;
	string outputHandle = "compressed.cmp";

	// parse optional arguments
	for (int a = 2; a < argc; a++) {
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
			// could be the filename of the input file
			if (inputHandle == "") {
				inputHandle.assign(arg);
			} else {
				// unsupported extra argument detected
				cerr << "Extra unknown argument given: " << arg << endl;
				cerr << "For usage, please run:\n\tcompressor --help" << endl;
				return EXIT_FAILURE;
			}
		}
	}

	// ensure input file was set
	if (inputHandle == "") {
		cerr << "No input file given." << endl;
		cerr << "For usage, please run:\n\tcompressor --help" << endl;
		return EXIT_FAILURE;
	}

	// debug
	cout << "Input: " << inputHandle << endl;
	cout << "Algorithm: " << algorithm << endl;
	cout << "Output: " << outputHandle << endl;

	Archive archive(inputHandle, outputHandle);
	if (action == 1) {
		// COMPRESSING
		archive.compress(algorithm);
	} else {
		// EXPANDING
		archive.expand();
	}

	return EXIT_SUCCESS;
}