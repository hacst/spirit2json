#ifndef SPIRIT2JSON_H
#define SPIRIT2JSON_H

#include "StdAfx.h"
#include <string>
#include <stdexcept>
#include <ostream>
#include <vector>
#include <map>
#include <boost/variant.hpp>

struct boost::recursive_variant_ {};

namespace spirit2json {

typedef boost::make_recursive_variant<
	std::wstring,
	double,
	bool,
	std::nullptr_t,
	std::vector<boost::recursive_variant_ >,
	std::map<std::wstring, boost::recursive_variant_ > >::type JSONValue;

typedef std::vector<JSONValue> JSONArray;
typedef std::map<std::wstring, JSONValue> JSONObject;

class Exception : public std::exception {
	virtual const char* what() const throw() {
		return "spirit2json: Exception";
	}
};

class ParsingFailed : public Exception {
	virtual const char* what() const throw() {
		return "spirit2json: Failed to parse given json";
	}
};

JSONValue parse(std::wstring str);

void get_stats(unsigned int &accumulated, 
			unsigned int &strings,
			unsigned int &objects,
			unsigned int &arrays,
			unsigned int &bools,
			unsigned int &nulls,
			unsigned int &doubles,
			JSONValue &val);


}

std::wostream& operator<<(std::wostream& output, spirit2json::JSONValue& val);
std::wostream& operator<<(std::wostream& output, spirit2json::JSONArray& arr);
std::wostream& operator<<(std::wostream& output, spirit2json::JSONObject& obj);

#endif
