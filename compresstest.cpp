#include <sstream>
#include "compress.h"

using namespace std;

int main() {
	stringstream intermediate;
	LZW compressor;
	LZW decompressor;

	// compress the data
	compressor.compress(cin, intermediate);
	
	// dump intermediate data
	//cerr << intermediate.str() << endl;

	// decompress the data
	decompressor.decompress(intermediate, cout);
	cout << endl;
}