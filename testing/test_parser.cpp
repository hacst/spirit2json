#define BOOST_TEST_DYN_LINK

#include <spirit2json.h>
#include <string>
#include <boost/test/unit_test.hpp>
#include <boost/variant.hpp>

using namespace std;
using namespace spirit2json;

using boost::get;

BOOST_AUTO_TEST_SUITE(test_parser)

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
	JSONValue val(parse(L"null"));
	BOOST_CHECK(val == JSONValue(JSONNull()));
	
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
	arr.push_back(JSONNull());
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
		o.insert(JSONObjectPair(L"NULL", JSONNull()));
		BOOST_CHECK(JSONValue(o) == parse(L"{\"NULL\":null}"));
	}

	{
		JSONObject o;
		o.insert(JSONObjectPair(L"other", JSONArray()));
		BOOST_CHECK(JSONValue(o) == parse(L"{\"other\":[]}"));
	}

	obj.insert(JSONObjectPair(L"test", 0.5));
	BOOST_CHECK(JSONValue(obj) == parse(L"{\"test\":0.5}"));

	obj.insert(JSONObjectPair(L"other", JSONArray()));
	BOOST_CHECK(JSONValue(obj) == parse(L"{\"test\":0.5,\"other\":[]}"));

	obj.insert(JSONObjectPair(L"NULL", JSONNull()));
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

/*
	Tests whether the type enumeration works as expected
*/
BOOST_AUTO_TEST_CASE(type_enum) {
	BOOST_CHECK_EQUAL(JSONValue(JSONString()).which(), JSON_STRING);
	BOOST_CHECK_EQUAL(JSONValue(JSONNumber(0)).which(), JSON_NUMBER);
	BOOST_CHECK_EQUAL(JSONValue(JSONBool(false)).which(), JSON_BOOL);
	BOOST_CHECK_EQUAL(JSONValue(JSONNull()).which(), JSON_NULL);
	BOOST_CHECK_EQUAL(JSONValue(JSONArray()).which(), JSON_ARRAY);
	BOOST_CHECK_EQUAL(JSONValue(JSONObject()).which(), JSON_OBJECT);
}

/*
	Test whether our JSONNull type works as expected
*/
BOOST_AUTO_TEST_CASE(json_null) {
	char *ch = JSONNull();
	BOOST_CHECK(ch == 0);
	BOOST_CHECK(ch == JSONNull());
	BOOST_CHECK(JSONNull() == JSONNull());
}

BOOST_AUTO_TEST_SUITE_END()
