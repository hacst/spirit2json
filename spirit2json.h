#ifndef SPIRIT2JSON_H
#define SPIRIT2JSON_H

#include <string>
#include <ostream>
#include <vector>
#include <unordered_map>
#include <boost/variant.hpp>

struct boost::recursive_variant_ {};

namespace spirit2json {

typedef boost::make_recursive_variant<
	std::string,
	double,
	bool,
	std::nullptr_t,
	std::vector<boost::recursive_variant_ >,
	std::unordered_map<std::string, boost::recursive_variant_ > >::type JSONValue;

typedef std::vector<JSONValue> JSONArray;
typedef std::unordered_map<std::string, JSONValue> JSONObject;

std::ostream& operator<<(std::ostream& output, JSONValue& val);
std::ostream& operator<<(std::ostream& output, JSONArray& arr);
std::ostream& operator<<(std::ostream& output, JSONObject& obj);

}





#endif