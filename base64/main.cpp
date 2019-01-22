#include <iostream>

#include "base64.hpp"


void test(const std::string & _what, const std::string & _expected)
{
	std::cout << "base64(\"" << _what << "\") = " << _expected << ": " << (base64::encode(_what) == _expected) << '\n';
}

int main()
{
	test("", "");
	test("f", "Zg==");
	test("fo", "Zm8=");
	test("foo", "Zm9v");
	test("foob", "Zm9vYg==");
	test("fooba", "Zm9vYmE=");
	test("foobar", "Zm9vYmFy");
}