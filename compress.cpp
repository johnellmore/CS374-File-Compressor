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

void BitWriter::close() {
	if (!bitLength) return;
	unsigned char lastByte = buffer >> (3*CHAR_BIT);
	output->put(lastByte); // end will be zero-filled
}


/* BITREADER */

unsigned int BitReader::read(int length) {
	// read enough into the buffer
	while (bufferLength < length) {
		// read in the next byte
		unsigned int toShift = 0;
		input->read((char *)&toShift+3, 1);
		toShift = toShift >> bufferLength;

		// merge the with buffer
		buffer = buffer | toShift;
		bufferLength += CHAR_BIT;
	}

	// get needed value
	unsigned int result = buffer >> (4*CHAR_BIT-length);
	// erase value from buffer
	buffer = buffer << length;
	bufferLength -= length;

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

LZW::LZW(unsigned int bitWidth)
:nextVal(0), maxBitWidth(bitWidth) {
	maxValue = 1;
	while (bitWidth--) maxValue*2;
	maxValue -= 1;
};

int LZW::nextValue() {
	if (nextVal > maxValue) return -1;
	return nextVal++;
}

int LZW::codeLength() const {
	unsigned int length = 1;
	unsigned int val = nextVal + (int)!compressing; // decompressing jumps ahead 
	while (val >>= 1) length++;
	return min(length, maxBitWidth);
}

void LZW::compress(istream &input, ostream &output) {
	compressing = true;

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

		// check for the end of the stream
		char nextChar = (char)input.peek();
		if (nextChar == EOF) break;

		// if there's room, add prefix plus following char to the dictionary
		int next = nextValue();
		if (next != -1) {
			list<char> nextPattern(prefix);
			nextPattern.push_back(nextChar);
			dict.insert(nextPattern, next);
		}
	}

	// finish out with the close word
	out.write(closeWord, codeLength());
	out.close();
}

void LZW::decompress(istream &input, ostream &output) {
	compressing = false;

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

	// start combing through codewords
	int value;
	int lastValue = in.read(codeLength());
	vector<char> thisToken = dict[lastValue];
	while (true) {
		// write the corresponding string to the stream
		output.write(&thisToken[0], thisToken.size());

		// read the next value, handle closeword
		value = in.read(codeLength());
		if (value == closeWord) break;

		// add the new dictionary entry
		int next = nextValue();
		if (next != -1) {
			vector<char> toAdd;
			if (value == currentValue()+1) {
				// this is the tricky case
				toAdd = dict[lastValue];
				toAdd.push_back(toAdd[0]);
				dict[next] = toAdd;
			} else {
				// normal handling
				thisToken = dict[value];
				toAdd = dict[lastValue];
				toAdd.push_back(thisToken[0]);
				dict[next] = toAdd;
			}
		}
		lastValue = value;
		if (lastValue > currentValue())
			throw runtime_error(string("Cannot decompress input; uses non-standard dictionary."));

		thisToken = dict[lastValue];
	}

}