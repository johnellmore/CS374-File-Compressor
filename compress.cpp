#include <iostream>
#include <cassert>

using namespace std;

string compress(string input){
	return input;
}

string decompress(string input){
	return input;
}

int main(){
	string input = "TOBEORNOTTOBEORTOBEORNOT";
	string output = decompress(compress(input));
	assert(input == output);
	cout << "done" << endl;
}