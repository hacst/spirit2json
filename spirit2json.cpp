#include "spirit2json.h"

namespace spirit2json {

/**
 * Printer class used in implementation of << operator overloads
 */
class printer : public boost::static_visitor<> {
	const unsigned int level;
	std::ostream& out;

public:
	printer(std::ostream& out, unsigned int level = 0) : out(out), level(level) {}

	std::string indent(unsigned int l) const {
		return std::string(l * 4, ' ');
	}

	void operator()(nullptr_t& nptr) const {
		out << "null";
	}

	void operator()(JSONArray &arr) const {
		out << "[" << std::endl;
		for (auto it = arr.begin(); it != arr.end(); ++it) {
			out << indent(level + 1);
			boost::apply_visitor ( printer(out, level + 1), *it);
			out << "," << std::endl;
		}
		out << indent(level) << "]";

	}

	void operator()(JSONObject &obj) const {
		out << "{" << std::endl;
		for (auto it = obj.begin(); it != obj.end(); ++it) {
			out << indent(level + 1) << "\"" << it->first << "\" : ";
			boost::apply_visitor( printer(out, level + 1), it->second);
			out << "," << std::endl;
		}
		out << indent(level) << "}";

	}

	void operator() (bool &b) const {
		out << (b ? "true" : "false");
	}

	void operator() (std::string &str) const {
		out << "\"" << str << "\"";
	}

	template <typename T>
	void operator() (T &t) const {
		out << t;
	}

};

std::ostream& operator<<(std::ostream& output, JSONValue& val)
{
	boost::apply_visitor(printer(output), val);
	return output;
}

std::ostream& operator<<(std::ostream& output, JSONArray& arr) {
	boost::apply_visitor(printer(output), JSONValue(arr));
	return output;
}

std::ostream& operator<<(std::ostream& output, JSONObject& map) {
	boost::apply_visitor(printer(output), JSONValue(map));
	return output;
}

}