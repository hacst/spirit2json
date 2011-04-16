#include <ostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include "spirit2json.h"

namespace spirit2json {

class counter : public boost::static_visitor<> {
	unsigned int &accumulated;
	unsigned int &strings;
	unsigned int &objects;
	unsigned int &arrays;
	unsigned int &bools;
	unsigned int &nulls;
	unsigned int &doubles;
public:
	counter(unsigned int &accumulated, 
			unsigned int &strings,
			unsigned int &objects,
			unsigned int &arrays,
			unsigned int &bools,
			unsigned int &nulls,
			unsigned int &doubles) : accumulated(accumulated),
	strings(strings), objects(objects), arrays(arrays), bools(bools),
	nulls(nulls), doubles(doubles) {}

	void operator()(std::nullptr_t& nptr) const {
		++accumulated;
		++nulls;
	}

	void operator()(JSONArray &arr) const {
		++accumulated;
		++arrays;

		for (auto it = arr.begin(); it != arr.end(); ++it) {
			boost::apply_visitor( *this, *it);
		}
	}

	void operator()(JSONObject &obj) const {
		++accumulated;
		++objects;
		for (auto it = obj.begin(); it != obj.end(); ++it) {
			++strings;
			++accumulated;
			boost::apply_visitor( *this, it->second);
		}
	}

	void operator() (bool &b) const {
		++bools;
		++accumulated;
	}

	void operator() (std::wstring &str) const {
		++strings;
		++accumulated;
	}

	void operator() (double &d) const {
		++accumulated;
		++doubles;
	}

};

void get_stats(unsigned int &accumulated, 
			unsigned int &strings,
			unsigned int &objects,
			unsigned int &arrays,
			unsigned int &bools,
			unsigned int &nulls,
			unsigned int &doubles,
			JSONValue &val) {
				boost::apply_visitor( counter(accumulated, strings, objects, arrays, bools, nulls, doubles), val);
}

/**
 * Printer class used in implementation of << operator overloads
 */
class printer : public boost::static_visitor<> {
	const unsigned int level;
	std::wostream& out;

public:
	printer(std::wostream& out, unsigned int level = 0) : level(level), out(out) {}

	std::wstring indent(unsigned int l) const {
		return std::wstring(l * 4, ' ');
	}

	void operator()(std::nullptr_t& nptr) const {
		out << "null";
	}

	void operator()(JSONArray &arr) const {
		out << "[" << std::endl;
		for (auto it = arr.begin(); it != arr.end(); ++it) {
			if (it != arr.begin())
				out << "," << std::endl;

			out << indent(level + 1);
			boost::apply_visitor ( printer(out, level + 1), *it);
		}
		out << std::endl << indent(level) << "]";

	}

	void operator()(JSONObject &obj) const {
		out << "{" << std::endl;
		for (auto it = obj.begin(); it != obj.end(); ++it) {
			if (it != obj.begin())
				out << ',' << std::endl;

			out << indent(level + 1) << "\"" << it->first << "\" : ";
			boost::apply_visitor( printer(out, level + 1), it->second);
		}
		out << std::endl << indent(level) << "}";

	}

	void operator() (bool &b) const {
		out << (b ? "true" : "false");
	}

	void operator() (std::wstring &str) const {
		out << "\"" << str << "\"";
	}

	template <typename T>
	void operator() (T &t) const {
		out << t;
	}

};


using namespace boost::spirit;
using namespace boost;

template <typename Iterator>
struct json_grammar : qi::grammar<Iterator, JSONValue(), qi::space_type> {
	json_grammar() : json_grammar::base_type(val) {
		using qi::lit;
		using qi::lexeme;
		using qi::_val;
		using standard_wide::char_;
		using namespace qi::labels;

		str = '"' >> *(("\\u" > qi::hex) |
					   ('\\' > escaped_char) |
					   (char_ - char_(L'\x0000', L'\x001F') - '"')
					  )
			>> '"';

		val %= str | bool_ | double_ | null | arr | obj;
		arr %= '[' >> -(val % ',') >> ']';
		obj %= '{' >> -(pair % ',') >> '}';
		null = lit("null")	[_val = nullptr];
		pair %= str >> ':' >> val;

		escaped_char.add("\"", '"')
				("\\", '\\')
				("/", '/')
				("b", '\b')
				("f", '\f')
				("n", '\n')
				("r", '\r')
				("t", '\t');

		val.name("Value");
		arr.name("Array");
		obj.name("Object");
		str.name("String");
		null.name("null");
		pair.name("Pair");
		
		qi::on_error<qi::fail>(
			val,
			std::cout
				<< phoenix::val("Error! Expecting ")
				<< _4
				<< phoenix::val(" here: \"")
				<< phoenix::construct<std::string>(_3, _2)
				<< phoenix::val("\"")
				<< std::endl
		);
	}
	
	qi::rule<Iterator, JSONValue(), qi::space_type> val;
	qi::rule<Iterator, JSONArray(), qi::space_type> arr;
	qi::rule<Iterator, JSONObject(), qi::space_type> obj;

	qi::rule<Iterator, std::wstring()> str;
	qi::symbols<char const, char const> escaped_char;

	qi::rule<Iterator, std::nullptr_t(), qi::space_type> null;

	qi::rule<Iterator, std::pair<std::wstring, JSONValue>(), qi::space_type> pair;
};

JSONValue parse(std::wstring str) {
	JSONValue result;
	std::wstring::const_iterator iter = str.begin();
	std::wstring::const_iterator end = str.end();

	bool r = qi::phrase_parse(iter, end, json_grammar<std::wstring::const_iterator>(), qi::space, result);
	//TODO: Implement this right
	if (!r || iter != str.end()) {
		throw ParsingFailed();
	}

	return result;
}

}

std::wostream& operator<<(std::wostream& output, spirit2json::JSONValue& val) {
	boost::apply_visitor(spirit2json::printer(output), val);
	return output;
}

std::wostream& operator<<(std::wostream& output, spirit2json::JSONArray& arr) {
	spirit2json::JSONValue val = spirit2json::JSONValue(arr);
	boost::apply_visitor(spirit2json::printer(output), val);
	return output;
}

std::wostream& operator<<(std::wostream& output, spirit2json::JSONObject& map) {
	spirit2json::JSONValue val = spirit2json::JSONValue(map);
	boost::apply_visitor(spirit2json::printer(output), val);
	return output;
}
