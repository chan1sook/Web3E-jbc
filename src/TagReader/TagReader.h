#ifndef WEB3_TAG_READER_H
#define WEB3_TAG_READER_H

#include <Arduino.h>
#include <string>
#include <string.h>
#include <vector>

using namespace std;

class TagReader
{
public:
	TagReader();
	String getTag(const String &JSON_Str, const char *value);
	size_t length() { return _length; }

private:
	size_t _length;
};

#endif // !WEB3_TAG_READER
