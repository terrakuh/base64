#include "base64.hpp"

#include <iomanip>
#include <iostream>

void test(const std::string& _what, const std::string& _expected)
{
	auto _actual = base64::encode(_what);

	std::cout << "base64(\"" << _what << "\") = " << _expected << ": " << (_actual == _expected) << " | "
	          << (base64::decode(_actual) == _what) << '\n';
}

int main()
{
	std::cout << std::boolalpha;

	test("", "");
	test("f", "Zg==");
	test("fo", "Zm8=");
	test("foo", "Zm9v");
	test("foob", "Zm9vYg==");
	test("fooba", "Zm9vYmE=");
	test("foobar", "Zm9vYmFy");
}