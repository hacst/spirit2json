#include "StdAfx.h"
#define BOOST_TEST_MODULE spirit2json test
#include <spirit2json.h>
#include <boost/test/included/unit_test.hpp>
#include <string>

using namespace std;
using namespace spirit2json;

/*
	Tests for Number
*/
BOOST_AUTO_TEST_CASE(number_basic_usage) {
	BOOST_CHECK_EQUAL(get<double>(JSONValue(parse(L"42.0"))), 42.0);
	BOOST_CHECK_EQUAL(get<double>(JSONValue(parse(L"-5"))), -5.);
	BOOST_CHECK_EQUAL(get<double>(JSONValue(parse(L"42"))), 42.);
	BOOST_CHECK_EQUAL(get<double>(JSONValue(parse(L"42"))), 42);
	BOOST_CHECK_EQUAL(get<double>(JSONValue(parse(L"1234567890.09876"))), 1234567890.09876);
	BOOST_CHECK_EQUAL(get<double>(JSONValue(parse(L"-1234567890.12345"))), -1234567890.12345);

	BOOST_CHECK_THROW(parse(L"123.123.123"), ParsingFailed);
}

BOOST_AUTO_TEST_CASE(number_notations) {
	//TODO: Test whether Numbers can be entered in all notations specified by the spec
	BOOST_FAIL("Not implemented");
}

BOOST_AUTO_TEST_CASE(number_precision) {
	//TODO: Make sure we meet the precision requirements outlined in the JSON spec
	BOOST_FAIL("Not implemented");
}

/*
	Tests for String
*/
BOOST_AUTO_TEST_CASE(string_basic_usage) {
	BOOST_CHECK(get<wstring>(JSONValue(parse(L" \" testing \" "))) == L" testing ");
}

const wstring sanskrit(L"\x092A\x0936\x0941\x092A\x0924\x093F\x0930\x092A\x093F "
					   L"\x0924\x093E\x0928\x094D\x092F\x0939\x093E\x0928\x093F "
					   L"\x0915\x0943\x091A\x094D\x091B\x094D\x0930\x093E\x0926\x094D");

const wstring sanskrit_escaped(L"\"\\u092A\\u0936\\u0941\\u092A\\u0924\\u093F\\u0930\\u092A\\u093F "
							   L"\\u0924\\u093E\\u0928\\u094D\\u092F\\u0939\\u093E\\u0928\\u093F "
							   L"\\u0915\\u0943\\u091A\\u094D\\u091B\\u094D\\u0930\\u093E\\u0926\\u094D\"");

BOOST_AUTO_TEST_CASE(string_escape_characters) {
	// Normal escapes
	BOOST_CHECK(get<wstring>(JSONValue(parse(L"\"\\t \\r \\n \\f \\b \\/ \\\\ \\\"\"")))
			== L"\t \r \n \f \b / \\ \"");
	BOOST_CHECK(get<wstring>(JSONValue(parse(L"\"testing\\tescapes\\\"in\\rtext \"")))
			== L"testing\tescapes\"in\rtext ");

	// Make sure we do not ignore unknown escapes
	BOOST_CHECK_THROW(parse(L"\"\\x\""), ParsingFailed);
	BOOST_CHECK_THROW(parse(L"\"\\U\""), ParsingFailed);

	// Hex escapes
	BOOST_CHECK(get<wstring>(JSONValue(parse(sanskrit_escaped))) == sanskrit);
	// Make sure we only accept 4 character hex numbers
	BOOST_CHECK(get<wstring>(JSONValue(parse(L"\"\\uABABA\""))) == L"\xABAB"L"A");
	BOOST_CHECK_THROW(parse(L"\"\\uAB\""), ParsingFailed);
	BOOST_CHECK_THROW(parse(L"\"\\u\""), ParsingFailed);
}

BOOST_AUTO_TEST_CASE(string_unicode) {
	BOOST_CHECK(get<wstring>(JSONValue(parse(L"\"" + sanskrit + L"\""))) == sanskrit);

	// Make sure we do not accept any characters between U+0000 thru U+001F
	BOOST_CHECK_THROW(parse(L"\"\x0000\""), ParsingFailed);
	BOOST_CHECK_THROW(parse(L"\"\x000F\""), ParsingFailed);
	BOOST_CHECK_THROW(parse(L"\"\x001F\""), ParsingFailed);
}

/*
	Tests for Bool
*/

BOOST_AUTO_TEST_CASE(boolean_basic_usage) {
	BOOST_CHECK_EQUAL(get<bool>(JSONValue(parse(L"true"))), true);
	BOOST_CHECK_EQUAL(get<bool>(JSONValue(parse(L"false"))), false);

	// Make sure we parse case sensitive
	BOOST_CHECK_THROW(parse(L"False"), ParsingFailed);
	BOOST_CHECK_THROW(parse(L"FALSE"), ParsingFailed);
	BOOST_CHECK_THROW(parse(L"True"), ParsingFailed);
	BOOST_CHECK_THROW(parse(L"TRUE"), ParsingFailed);
}

/*
	Tests for null
*/
BOOST_AUTO_TEST_CASE(null_basic_usage) {
	//TODO: Figure out how to do this in a nice way
	JSONValue val(parse(L"null"));
	
	unsigned int accumulated = 0;
	unsigned int strings = 0;
	unsigned int objects = 0;
	unsigned int arrays = 0;
	unsigned int bools = 0;
	unsigned int nulls = 0;
	unsigned int doubles = 0;
	get_stats(accumulated, strings, objects, arrays, bools, nulls, doubles, val);
	BOOST_CHECK((nulls == 1) && (accumulated == 1));
	
	// Make sure we don't accept anything used in other languages
	BOOST_CHECK_THROW(parse(L"nil"), ParsingFailed);
	BOOST_CHECK_THROW(parse(L"None"), ParsingFailed);

	// Make sure we parse case sensitive
	BOOST_CHECK_THROW(parse(L"Null"), ParsingFailed);
	BOOST_CHECK_THROW(parse(L"NULL"), ParsingFailed);
}

/*
	Tests for Array
*/
BOOST_AUTO_TEST_CASE(array_basic_usage) {
	JSONArray arr;
	BOOST_CHECK(JSONValue(arr) == parse(L"[]"));

	arr.push_back(0.5);
	arr.push_back(nullptr);
	arr.push_back(wstring(L"testing"));
	arr.push_back(false);

	BOOST_CHECK(JSONValue(arr) == parse(L"[ 0.5, null  \t, \n\"testing\",false   ] "));
}

/*
	Tests for Object
*/
BOOST_AUTO_TEST_CASE(object_basic_usage) {
	JSONObject obj;
	BOOST_CHECK(JSONValue(obj) == parse(L"{}"));

	{
		JSONObject o;
		o.insert(JSONObject::value_type(L"NULL", nullptr));
		BOOST_CHECK(JSONValue(o) == parse(L"{\"NULL\":null}"));
	}

	{
		JSONObject o;
		o.insert(JSONObject::value_type(L"other", JSONArray()));
		BOOST_CHECK(JSONValue(o) == parse(L"{\"other\":[]}"));
	}

	obj.insert(JSONObject::value_type(L"test", 0.5));
	BOOST_CHECK(JSONValue(obj) == parse(L"{\"test\":0.5}"));

	obj.insert(JSONObject::value_type(L"other", JSONArray()));
	BOOST_CHECK(JSONValue(obj) == parse(L"{\"test\":0.5,\"other\":[]}"));

	obj.insert(JSONObject::value_type(L"NULL", nullptr));
	BOOST_CHECK(JSONValue(obj) == parse(L" { \"test\" :0.5,\n\t\t\"other\"  \t\n:[],  \"NULL\":null}"));

	BOOST_CHECK_THROW(parse(L"{:}"), ParsingFailed);
}

/*
	Tests for miscellaneous stuff
*/
BOOST_AUTO_TEST_CASE(misc) {
	BOOST_CHECK_THROW(parse(L""), ParsingFailed);
	BOOST_CHECK_THROW(parse(L"["), ParsingFailed);
	BOOST_CHECK_THROW(parse(L"]"), ParsingFailed);
	BOOST_CHECK_THROW(parse(L","), ParsingFailed);
}