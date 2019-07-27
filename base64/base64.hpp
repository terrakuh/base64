#pragma once

#include <string>
#include <cstdio>
#include <cstdint>


class base64_error : public std::runtime_error
{
public:
	using std::runtime_error::runtime_error;
};

class base64
{
public:
	constexpr static char pad = '=';

	enum class ALPHABET
	{
		AUTO,
		NORMAL,
		URL_FILENAME_SAFE
	};

	static void decode_inplace(std::string& str, ALPHABET alphabet = ALPHABET::AUTO)
	{
		str.resize(decode_inplace(&str[0], str.length(), alphabet));
	}

	static size_t decode_inplace(char* str, size_t length, ALPHABET alphapet = ALPHABET::AUTO)
	{
		if (!length) {
			return 0;
		} else if (length % 4) {
			throw base64_error("invalid base64 input.");
		}

		return length / 4 * 3 - do_decode(str, length, str, alphapet);
	}

	static std::string encode(const std::string& str, ALPHABET alphabet = ALPHABET::NORMAL)
	{
		return encode(str.c_str(), str.length(), alphabet);
	}

	static std::string encode(const char* str, size_t length, ALPHABET alphabet = ALPHABET::NORMAL)
	{
		if (!length) {
			return "";
		}

		std::string result;
		const char* alpha = nullptr;
		const auto end = str + length;

		// Set alphabet
		switch (alphabet) {
		case ALPHABET::AUTO:
		case ALPHABET::NORMAL:
			alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			break;
		case ALPHABET::URL_FILENAME_SAFE:
			alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

			break;
		default:
			throw base64_error("invalid alphabet");
		}

		result.resize((length / 3 + (length % 3 ? 1 : 0)) * 4);

		auto out = &result[0];

		// Encode 3 bytes
		while (str + 3 <= end) {
			encode<3>(alpha, str, out);
		}

		// Encode last bit
		if (end - str == 1) {
			encode<1>(alpha, str, out);
		} else if (end - str == 2) {
			encode<2>(alpha, str, out);
		}

		return result;
	}

	static std::string decode(const std::string& str, ALPHABET alphabet = ALPHABET::AUTO)
	{
		return decode(str.c_str(), str.length(), alphabet);
	}

	static std::string decode(const char* str, size_t length, ALPHABET alphabet = ALPHABET::AUTO)
	{
		if (!length) {
			return "";
		} else if (length % 4) {
			throw base64_error("invalid base64 input.");
		}

		std::string result;

		result.resize(length / 4 * 3);
		result.resize(result.length() - do_decode(str, length, &result[0], alphabet));

		return result;
	}

private:
	template<int Count>
	static void encode(const char* alphabet, const char*& input, char*& output)
	{
		if (Count == 3) {
			output[0] = alphabet[input[0] >> 2 & 0x3f];
			output[1] = alphabet[((input[0] & 0x03) << 4) | (input[1] >> 4 & 0x0f)];
			output[2] = alphabet[((input[1] & 0x0f) << 2) | (input[2] >> 6 & 0x03)];
			output[3] = alphabet[input[2] & 0x3f];
		} else if (Count == 2) {
			output[0] = alphabet[input[0] >> 2 & 0x3f];
			output[1] = alphabet[((input[0] & 0x03) << 4) | (input[1] >> 4 & 0x0f)];
			output[2] = alphabet[(input[1] & 0x0f) << 2];
			output[3] = pad;
		} else if (Count == 1) {
			output[0] = alphabet[input[0] >> 2 & 0x3f];
			output[1] = alphabet[(input[0] & 0x03) << 4];
			output[2] = pad;
			output[3] = pad;
		}

		input += Count;
		output += 4;
	}
	template<int Count>
	static void decode(ALPHABET& alphabet, const char*& input, char*& output)
	{
		char tmp;

		output[0] = base64_value(alphabet, input[0]) << 2;

		tmp = base64_value(alphabet, input[1]);

		output[0] |= tmp >> 4;

		if (Count < 2) {
			output[1] = (tmp & 0x0f) << 4;

			tmp = base64_value(alphabet, input[2]);

			output[1] |= tmp >> 2;

			if (Count < 1) {
				output[2] = (tmp & 0x03) << 6;
				output[2] |= base64_value(alphabet, input[3]);
			}
		}

		input += 4;
		output += 3 - Count;
	}
	static char base64_value(ALPHABET& alphabet, char c)
	{
		if (c >= 'A' && c <= 'Z') {
			return c - 'A';
		} else if (c >= 'a' && c <= 'z') {
			return c - 'a' + 26;
		} else if (c >= '0' && c <= '9') {
			return c - '0' + 52;
		}

		// Comes down to alphabet
		if (alphabet == ALPHABET::NORMAL) {
			if (c == '+') {
				return 62;
			} else if (c == '/') {
				return 63;
			}
		} else if (alphabet == ALPHABET::URL_FILENAME_SAFE) {
			if (c == '-') {
				return 62;
			} else if (c == '_') {
				return 63;
			}
		} else {
			if (c == '+') {
				alphabet = ALPHABET::NORMAL;

				return 62;
			} else if (c == '/') {
				alphabet = ALPHABET::NORMAL;

				return 63;
			} else if (c == '-') {
				alphabet = ALPHABET::URL_FILENAME_SAFE;

				return 62;
			} else if (c == '_') {
				alphabet = ALPHABET::URL_FILENAME_SAFE;

				return 63;
			}
		}

		throw base64_error("invalid base64 character.");
	}

	static size_t do_decode(const char* input, size_t length, char* output, ALPHABET alphabet)
	{
		const auto end = input + length - 4;

		// Decode 4 bytes
		while (input + 4 <= end) {
			decode<0>(alphabet, input, output);
		}

		// Decode last 4 bytes
		if (input[3] == pad) {
			if (input[2] == pad) {
				decode<2>(alphabet, input, output);

				return 2;
			}

			decode<1>(alphabet, input, output);

			return 1;
		}

		decode<0>(alphabet, input, output);

		return 0;
	}
};