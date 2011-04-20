/**
 * \file spirit2json.cpp
 * \author Stefan Hacker
 * \copyright \verbatim
 *
 * Copyright (c) 2011, Stefan Hacker <dd0t@users.sourceforge.net>
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the authors nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \endverbatim
 */

#include "stdafx.h"

#include "spirit2json.h"

namespace spirit2json {

//////////////
// PARSING //
////////////

using namespace boost;
using namespace boost::spirit;

/**
 * \brief Spirit2 qi grammar for parsing json strings to JSONValue AST representations.
 */
template <typename Iterator>
struct json_grammar : qi::grammar<Iterator, JSONValue(), qi::space_type> {
	json_grammar() : json_grammar::base_type(val) {
		using qi::lit;
		using qi::lexeme;
		using qi::_val;
		using qi::uint_parser;
		using standard_wide::char_;
		using namespace qi::labels;

		str = '"' > *(("\\u" > uint_parser<unsigned, 16, 4, 4>() ) |
					   ('\\' > escaped_char) |
					   (char_ - char_(L'\x0000', L'\x001F') - '"')
					  )
			> '"';

		val %= str | bool_ | double_ | null | arr | obj;
		arr %= '[' > -(val % ',') > ']';
		obj %= '{' > -(pair % ',') > '}';
		null = lit("null")	[_val = JSONNull()];
		pair %= str > ':' > val;

		escaped_char.add("\"", '"')
				("\\", '\\')
				("/", '/')
				("b", '\b')
				("f", '\f')
				("n", '\n')
				("r", '\r')
				("t", '\t');
		escaped_char.name("Escape character");

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
				<< phoenix::val(" instead found: \"")
				<< phoenix::construct<std::string>(_3, _2)
				<< phoenix::val("\"")
				<< std::endl
		);
	}
	
	qi::rule<Iterator, JSONValue(), qi::space_type> val;  //!< Rule parsing JSONValue types
	qi::rule<Iterator, JSONArray(), qi::space_type> arr;  //!< Rule parsing JSONArray types
	qi::rule<Iterator, JSONObject(), qi::space_type> obj; //!< Rule parsing JSONObject types

	qi::rule<Iterator, std::wstring()> str;				  //!< Rule parsing JSONString types
	qi::symbols<char const, char const> escaped_char;	  //!< List of escape characters and their code

	qi::rule<Iterator, JSONNull(), qi::space_type> null;  //!< Rule parsing JSONNull types

	qi::rule<Iterator, std::pair<JSONString, JSONValue>(), qi::space_type> pair; //!< Rule parsing pairs in JSONObject types
};

JSONValue parse(JSONString str) {
	JSONValue result;

	JSONString::const_iterator iter = str.begin();
	JSONString::const_iterator end = str.end();

	bool r = qi::phrase_parse(iter, end, json_grammar<JSONString::const_iterator>(), qi::space, result);
	//TODO: Implement this right
	if (!r || iter != str.end()) {
		throw ParsingFailed();
	}

	return result;
}


/////////////////
// Generation //
///////////////

/**
 * \brief Static visitor for pretty printing a JSONValue variant to an std::wostream.
 */
class prettyPrinter : public boost::static_visitor<> {
	const unsigned int level;
	std::wostream& out;

public:
	prettyPrinter(std::wostream& out, unsigned int level = 0) : level(level), out(out) {}

	JSONString indent(unsigned int l) const {
		return JSONString(l * 4, ' ');
	}

	void operator()(JSONNull&) const {
		out << L"null";
	}

	void operator()(JSONArray &arr) const {
		out << L"[" << std::endl;
		for (auto it = arr.begin(); it != arr.end(); ++it) {
			if (it != arr.begin())
				out << L"," << std::endl;

			out << indent(level + 1);
			boost::apply_visitor ( prettyPrinter(out, level + 1), *it);
		}
		out << std::endl << indent(level) << L"]";

	}

	void operator()(JSONObject &obj) const {
		out << L"{" << std::endl;
		for (auto it = obj.begin(); it != obj.end(); ++it) {
			if (it != obj.begin())
				out << L',' << std::endl;

			out << indent(level + 1) << L"\"" << it->first << L"\" : ";
			boost::apply_visitor( prettyPrinter(out, level + 1), it->second);
		}
		out << std::endl << indent(level) << L"}";

	}

	void operator() (JSONBool &b) const {
		out << (b ? L"true" : L"false");
	}

	void operator() (JSONString &str) const {
		out << L"\"" << str << L"\"";
	}

	template <typename T>
	void operator() (T &t) const {
		out << t;
	}

};

JSONString generate(JSONValue& val) {
	std::wstringstream ss;
	boost::apply_visitor(prettyPrinter(ss), val);
	return ss.str();
}

} // namespace spirit2json

std::wostream& operator<<(std::wostream& output, spirit2json::JSONValue& val) {
	boost::apply_visitor(spirit2json::prettyPrinter(output), val);
	return output;
}