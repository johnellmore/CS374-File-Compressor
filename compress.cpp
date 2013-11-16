#include "compress.h"

using namespace std;


/* BITWRITER CLASS */

void BitWriter::write(unsigned int data, int length) {
	// shift the data into the buffer
	data = data << (4*CHAR_BIT-bitLength-length);
	buffer = buffer | data;
	bitLength += length;

	// if buffer is over the limit, write it
	while (bitLength >= CHAR_BIT) {
		unsigned char firstByte = buffer >> (3*CHAR_BIT);
		output->put(firstByte);
		buffer = buffer << CHAR_BIT;
		bitLength -= CHAR_BIT;
	}
}

void BitWriter::writeDebug(unsigned int data, int length) {
	(*output) << data << " ";
	length;
}

void BitWriter::close() {
	if (!bitLength) return;
	unsigned char lastByte = buffer >> (3*CHAR_BIT);
	output->put(lastByte); // end will be zero-filled
}


/* BITREADER */

unsigned int BitReader::read(int length) {
	//cout << hex << buffer << " " << bufferLength << endl;
	// read enough into the buffer
	while (bufferLength < length) {
		// read in the next byte
		unsigned int toShift = 0;
		input->read((char *)&toShift+3, 1);
		toShift = toShift >> bufferLength;

		// merge the with buffer
		//cout << hex << buffer << " " << toShift << " ";
		buffer = buffer | toShift;
		//cout << hex << buffer << " ";
		bufferLength += CHAR_BIT;
	}
	//cout << endl;

	// get needed value

	unsigned int result = buffer >> (4*CHAR_BIT-length);
	//cout << "[" << result << "] " << endl;
	// erase value from buffer
	//cout << buffer << " ";
	buffer = buffer << length;
	bufferLength -= length;
	//cout << buffer << endl;

	return result;
}


/* TRIE CLASS */

void Trie::insert(list<char> key, int value) {
	int keySize = key.size();
	if (keySize == 0) {
		// cannot insert a zero-sized key
		throw runtime_error(string("Trie cannot handle keys of zero length."));

	} else if (keySize == 1) {
		// see if a Trie already exists at this key
		Trie::Map::iterator step = children.find(key.front());
		if (step != children.end())
			throw runtime_error(string("Key already exists in Trie."));

		// the key doesn't exist, so create a new child Trie with the value
		Trie::TriePtr child(new Trie(value));
		children[key.front()] = (move(child));

 	} else {
		// we're not at the right Trie yet
		// if the first char doesn't have a Trie, we're broken
		Trie::Map::iterator step = children.find(key.front());
		if (step == children.end())
			throw runtime_error(string("Cannot insert key; parents do not exist."));
		
		// if it does, call insert recursively down the child tree
		key.pop_front();
		step->second->insert(key, value);
	}
}

int Trie::longestPrefixOf(istream &buffer, list<char> &prefix) {
	// if we've hit the end of the buffer, this is the best node
	if (buffer.eof()) return value;

	// pull out the next char to match
	char c;
	buffer.get(c);

	// if there are no more matching child nodes, this is the best node
	Trie::Map::iterator child = children.find(c);
	if (child == children.end()) {
		buffer.unget();
		return value;
	}
	prefix.push_back(c);

	// otherwise, continue down the tree
	return child->second->longestPrefixOf(buffer, prefix);
}


/* LZW CLASS */

int LZW::nextValue() {
	if (nextVal > maxValue) return -1;
	return nextVal++;
}

int LZW::codeLength() const {
	int length = 1;
	unsigned int val = nextVal - 1;
	while (val >>= 1) length++;
	return length;
}

void LZW::compress(istream &input, ostream &output) {
	// init structures
	BitWriter out(&output);
	Trie dict;

	// pre-fill dictionary with all 8-bit combos
	int charCombos = 1 << CHAR_BIT;
	for (int i = 0; i < charCombos; i++) {
		list<char> test;
		test.assign(1, i);
		dict.insert(test, nextValue());
	}
	int closeWord = nextValue();

	// start the processing
	while (!input.eof()) {
		// scan through the best prefix here
		list<char> prefix;
		int value = dict.longestPrefixOf(input, prefix);

		// write this value to the output
		out.write(value, codeLength());

		// add this prefix (plus the following char) to the dictionary
		char nextChar = (char)input.peek();
		if (nextChar == EOF) break;
		list<char> nextPattern(prefix);
		nextPattern.push_back(nextChar);
		dict.insert(nextPattern, nextValue());
	}

	// finish out with the close word
	out.write(closeWord, codeLength());
	out.close();
}

void LZW::decompress(istream &input, ostream &output) {
	// init structures
	BitReader in(&input);
	vector<vector<char>> dict;
	dict.reserve(maxValue+1);

	// pre-fill dictionary with all 8-bit combos
	int charCombos = 1 << CHAR_BIT;
	for (int i = 0; i < charCombos; i++) {
		dict[nextValue()].assign(1, (char)i);
	}
	int closeWord = nextValue(); // this word will signify the end of the stream

	// iterate through each value
	int value;
	int lastValue = -1;
	while ((value = in.read(codeLength())) != closeWord) {

		// if we haven't added the value to the dictionary yet, it's invalid
		if (value > currentValue())
			throw runtime_error(string("Cannot decompress; input is corrupted."));
		
		// write the token to the file
		vector<char> thisToken;
		if (value == currentValue()+1) {
			// this is the special tricky case
			thisToken = dict[lastValue];
			thisToken.push_back(thisToken[0]);
			output.write(&thisToken[0], thisToken.size());

			// save the tricky entry
			dict[nextValue()] = thisToken;

		} else {
			// this is a normal case
			thisToken = dict[value];
			output.write(&thisToken[0], thisToken.size());

			// add the appropriate entry to the dictionary
			if (lastValue != -1) {
				vector<char> toAdd = dict[lastValue];
				toAdd.push_back(thisToken[0]);
				dict[nextValue()] = toAdd;
			}
			
		}

		lastValue = value;
	}
}