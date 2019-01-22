#pragma once

#include <string>
#include <cstdio>
#include <cstdint>


class base64_error : public std::runtime_error
{
public:
	std::runtime_error::runtime_error;
};

class base64
{
public:
	constexpr static char pad = '=';
	constexpr static char a[] = "";

	enum class ALPHABET
	{
		AUTO,
		NORMAL,
		URL_FILENAME_SAFE
	};

	static std::string encode(const std::string & _str, ALPHABET _alphabet = ALPHABET::NORMAL)
	{
		return encode(_str.c_str(), _str.length(), _alphabet);
	}
	static std::string encode(const char * _str, size_t _length, ALPHABET _alphabet = ALPHABET::NORMAL)
	{
		if (!_length) {
			return "";
		}

		std::string _result;
		const char * _alpha = nullptr;
		const auto _end = _str + _length;

		// Set alphabet
		switch (_alphabet) {
		case ALPHABET::AUTO:
		case ALPHABET::NORMAL:
			_alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			break;
		case ALPHABET::URL_FILENAME_SAFE:
			_alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

			break;
		}

		_result.resize((_length / 3 + (_length % 3 ? 1 : 0)) * 4);

		auto _out = &_result[0];

		// Encode 3 bytes
		while (_str + 3 <= _end) {
			encode<3>(_alpha, _str, _out);
		}

		// Encode last bit
		if (_end - _str == 1) {
			encode<1>(_alpha, _str, _out);
		} else if (_end - _str == 2) {
			encode<2>(_alpha, _str, _out);
		}

		return _result;
	}
private:
	template<int Count>
	static void encode(const char * _alphabet, const char *& _input, char *& _output)
	{
		if (Count == 3) {
			_output[0] = _alphabet[_input[0] >> 2 & 0x3f];
			_output[1] = _alphabet[(_input[0] & 0x03) << 4 | _input[1] >> 4 & 0x0f];
			_output[2] = _alphabet[(_input[1] & 0x0f) << 2 | _input[2] >> 6 & 0x03];
			_output[3] = _alphabet[_input[2] & 0x3f];
		} else if (Count == 2) {
			_output[0] = _alphabet[_input[0] >> 2 & 0x3f];
			_output[1] = _alphabet[(_input[0] & 0x03) << 4 | _input[1] >> 4 & 0x0f];
			_output[2] = _alphabet[(_input[1] & 0x0f) << 2];
			_output[3] = pad;
		} else if (Count == 1) {
			_output[0] = _alphabet[_input[0] >> 2 & 0x3f];
			_output[1] = _alphabet[(_input[0] & 0x03) << 4];
			_output[2] = pad;
			_output[3] = pad;
		}

		_input += Count;
		_output += 4;
	}
};