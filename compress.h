#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <memory>
#include <utility>
#include <stdexcept>
#include <climits>
#include <algorithm>


class BitWriter {
	unsigned int buffer;
	int bitLength;
	std::ostream* output;

public:
	BitWriter(std::ostream* s) : buffer(0), bitLength(0), output(s) {};
	void write(unsigned int data, int length);
	void close();
};

class BitReader {
	unsigned int buffer;
	int bufferLength;
	std::istream* input;
	bool error;

public:
	BitReader(std::istream* s) : buffer(0), bufferLength(0), input(s), error(false) {};
	unsigned int read(int length);
	bool okay() { return !error; }
};

class Trie {
	typedef std::unique_ptr<Trie> TriePtr;
	typedef std::map<char, TriePtr> Map;

	Map children;
	int value;

public:
	Trie() : value(-1) {};
	Trie(int v) : value(v) {};
	void insert(std::list<char> key, int value);
	int longestPrefixOf(std::istream &buffer, std::list<char> &prefix);
};

class LZW {
	unsigned int nextVal;
	unsigned int maxValue;
	unsigned int maxBitWidth;
	bool compressing;

public:
	LZW() : nextVal(0), maxValue(4095), maxBitWidth(12) {};
	LZW(unsigned int bitWidth);
	int nextValue();
	int currentValue() const { return nextVal-1; };
	int codeLength() const;
	void compress(std::istream &input, std::ostream &output);
	void decompress(std::istream &input, std::ostream &output);
};
