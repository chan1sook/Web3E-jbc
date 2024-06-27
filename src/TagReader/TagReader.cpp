/**
 *	Original Author: James Brown 2018
 *	Crawls a JSON looking for tags
 *	Saves any allocation
 *
 *	TODO: Move all JSON handling to this class
 *		- pass a char* so terminators can be inserted and have all the value pairs stay within the original data, which is on stack
 **/

#include "TagReader.h"

using namespace std;

typedef enum STATE_DEF
{
	KEY,
	DELIMITER,
	VALUE,
	DEREFERENCE,
	END_VALUE
} READ_STATE;

TagReader::TagReader()
{
	_length = 0;
}

String TagReader::getTag(const String &JSON_Str, const char *value)
{
	// first determine if the tag exists in the file
	int index = JSON_Str.indexOf(value);
	if (index == -1) // safe scan
	{
		return "";
	}

	// parse for the end value
	READ_STATE read_state = KEY;
	READ_STATE old_state;
	bool flagStartValue = false;
	char *retValue;
	size_t startIndex = 0;
	_length = 0;

	while (index < JSON_Str.length() && read_state != END_VALUE)
	{
		const char c = JSON_Str[index];

		switch (read_state)
		{
		case KEY:
			if (c == '\"' || c == ':')
				read_state = DELIMITER;
			break;
		case DELIMITER:
			if (c != '\"' && c != ':')
			{
				read_state = VALUE;
				index--;
			}
			break;
		case VALUE:
			if (!flagStartValue)
			{
				flagStartValue = true;
				startIndex = index;
			}
			if (c == '\"')
			{
				read_state = END_VALUE;
			}
			else
			{
				_length++;
			}
			break;
		case DEREFERENCE:
			read_state = old_state;
			break;
		case END_VALUE:
			break;
		}

		if (c == '\\')
		{
			old_state = read_state;
			read_state = DEREFERENCE;
		}
		index++;
	}

	return JSON_Str.substring(startIndex, startIndex + _length);
}
