#include <cstdlib>
#include <iostream>
#include <list>
#include <memory>
#include <boost/filesystem>

class Object;
class File;
class Directory;
class Archive;

class Object {
	std::string handle;
	unsigned int modified;
	// ...
};

class File : public Object {
	std::string contents;

public:
	std::string toString();
	void fromString(const std::string input);
};

class Directory : public Object {
	std::list<std::unique_ptr<Object>> contents;

public:
	std::string toString();
	void fromString(const std::string input);
};

class Archive {
	std::string archiveHandle;
	std::string outputHandle;
	std::unique_ptr<Object> contents;

public:
	Archive(const std::string archive, const std::string output);
	void compress(unsigned int algo);
	void expand();
};
