#define BOOST_TEST_DYN_LINK

#include <spirit2json.h>
#include <string>
#include <fstream>
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace spirit2json;

BOOST_AUTO_TEST_SUITE(test_generator)

JSONString loadFile(std::string path)
{
	wifstream file(path.c_str());
	if (file.bad()) {
		stringstream error;
		error << "Could not load file: " << path;
		BOOST_FAIL(error.str());
	}

	wstringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

BOOST_AUTO_TEST_CASE(roundtrip_tests) {
	{
		JSONValue original(parse(loadFile("testing/samples/json_org_glossary_sample.json")));
		JSONValue converted(parse(generate(original)));

		BOOST_CHECK(original == converted);
	}

	{
		JSONValue original(parse(loadFile("testing/samples/json_org_menu_sample.json")));
		JSONValue converted(parse(generate(original)));

		BOOST_CHECK(original == converted);
	}

	{
		JSONValue original(parse(loadFile("testing/samples/json_org_web_app_sample.json")));
		JSONValue converted(parse(generate(original)));

		BOOST_CHECK(original == converted);
	}

	{
		JSONValue original(parse(loadFile("testing/samples/json_org_widget_sample.json")));
		JSONValue converted(parse(generate(original)));

		BOOST_CHECK(original == converted);
	}

	{
		JSONValue original(parse(loadFile("testing/samples/coordinate_sample.json")));
		JSONValue converted(parse(generate(original)));

		BOOST_CHECK(original == converted);
	}

	{
		JSONValue original(parse(loadFile("testing/samples/small_sample.json")));
		JSONValue converted(parse(generate(original)));

		BOOST_CHECK(original == converted);
	}
}

BOOST_AUTO_TEST_CASE(string_escape_characters)
{

	BOOST_FAIL("Not implemented yet");
}

BOOST_AUTO_TEST_SUITE_END()
