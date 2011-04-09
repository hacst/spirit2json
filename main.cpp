#include <iostream>
#include <ostream>
#include "spirit2json.h"

using namespace std;
using namespace spirit2json;



int main() {
	JSONValue val(42.0);

    JSONArray arr;
	arr.push_back(val);
    arr.push_back(0.5);
    arr.push_back(nullptr);
    arr.push_back(true);
    arr.push_back(false);

    JSONObject obj;
    obj.insert(JSONObject::value_type("test", 0.5));
    obj.insert(JSONObject::value_type("other", arr));
    obj.insert(JSONObject::value_type("NULL", nullptr));

	cout << "Value" << endl;
	cout << val << endl << endl;

	cout << "Array" << endl;
	cout << arr << endl << endl;

	cout << "Obj" << endl;
	cout << obj << endl << endl;

    return 0;
}