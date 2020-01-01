#pragma once

#include <string>
#include <cstdio>
#include <cstdint>
#include <stdexcept>


class base64_error : public std::runtime_error
{
public:
	using std::runtime_error::runtime_error;
};

class base64
{
public:
	constexpr static char pad = '=';

	enum class alphabet
	{
		auto_,
		normal,
		url_filename_safe
	};

	static void decode_inplace(std::string& str, alphabet alphabet = alphabet::auto_)
	{
		str.resize(decode_inplace(&str[0], str.length(), alphabet));
	}

	static size_t decode_inplace(char* str, size_t length, alphabet alphapet = alphabet::auto_)
	{
		if (!length) {
			return 0;
		} else if (length % 4) {
			throw base64_error("invalid base64 input.");
		}

		return length / 4 * 3 - do_decode(str, length, str, alphapet);
	}
	/**
	 Encodes all the elements from `in_begin` to `in_end` to `out`.

	 @warning The source and destination cannot overlap. The destination must be able to hold at least `required_encode_size(std::distance(in_begin, in_end))`, otherwise the behavior depends on the output iterator.

	 @tparam Input_iterator the source; the returned elements are cast to `std::uint8_t` and should not be greater than 8 bits
	 @tparam Output_iterator the destination; the elements written to it are from the type `char`
	 @param in_begin the beginning of the source
	 @param in_end the ending of the source
	 @param out the destination iterator
	 @param alphabet which alphabet should be used
	 @returns the iterator to the next element past the last element copied
	 @throws see `Input_iterator` and `Output_iterator`
	*/
	template<typename Input_iterator, typename Output_iterator>
	static Output_iterator encode(Input_iterator in_begin, Input_iterator in_end, Output_iterator out,
	                              alphabet alphabet = alphabet::normal)
	{
		constexpr auto pad = '=';
		const char* alpha  = alphabet == alphabet::url_filename_safe
		                        ? "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"
		                        : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		while (in_begin != in_end) {
			std::uint8_t i0 = 0, i1 = 0, i2 = 0;

			// first character
			i0 = static_cast<std::uint8_t>(*in_begin);
			++in_begin;

			*out = alpha[i0 >> 2 & 0x3f];
			++out;

			// part of first character and second
			if (in_begin != in_end) {
				i1 = static_cast<std::uint8_t>(*in_begin);
				++in_begin;

				*out = alpha[((i0 & 0x3) << 4) | (i1 >> 4 & 0x0f)];
				++out;
			} else {
				*out = alpha[(i0 & 0x3) << 4];
				++out;

				// last padding
				*out = pad;
				++out;

				// last padding
				*out = pad;
				++out;

				break;
			}

			// part of second character and third
			if (in_begin != in_end) {
				i2 = static_cast<std::uint8_t>(*in_begin);
				++in_begin;

				*out = alpha[((i1 & 0xf) << 2) | (i2 >> 6 & 0x03)];
				++out;
			} else {
				*out = alpha[(i1 & 0xf) << 2];
				++out;

				// last padding
				*out = pad;
				++out;

				break;
			}

			// rest of third
			*out = alpha[i2 & 0x3f];
			++out;
		}

		return out;
	}
	/**
	 Encodes a string.

	 @param str the string that should be encoded
	 @param alphabet which alphabet should be used
	 @returns the encoded base64 string
	 @throws see base64::encode()
	*/
	static std::string encode(const std::string& str, alphabet alphabet = alphabet::normal)
	{
		std::string result;

		result.reserve(required_encode_size(str.length()) + 1);

		encode(str.begin(), str.end(), std::back_inserter(result), alphabet);

		return result;
	}
	/**
	 Encodes a char array.

	 @param str the char array
	 @param length the length of the array
	 @param alphabet which alphabet should be used
	 @returns the encoded string
	*/
	static std::string encode(const char* str, size_t length, alphabet alphabet = alphabet::normal)
	{
		std::string result;

		result.reserve(required_encode_size(length) + 1);

		encode(str, str + length, std::back_inserter(result), alphabet);

		return result;
	}
	static std::string decode(const std::string& str, alphabet alphabet = alphabet::auto_)
	{
		return decode(str.c_str(), str.length(), alphabet);
	}

	static std::string decode(const char* str, size_t length, alphabet alphabet = alphabet::auto_)
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
	static std::size_t required_encode_size(std::size_t length) noexcept
	{
		return (length / 3 + (length % 3 ? 1 : 0)) * 4;
	}

private:
	template<int Count>
	static void decode(alphabet& alphabet, const char*& input, char*& output)
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
	static char base64_value(alphabet& alphabet, char c)
	{
		if (c >= 'A' && c <= 'Z') {
			return c - 'A';
		} else if (c >= 'a' && c <= 'z') {
			return c - 'a' + 26;
		} else if (c >= '0' && c <= '9') {
			return c - '0' + 52;
		}

		// Comes down to alphabet
		if (alphabet == alphabet::normal) {
			if (c == '+') {
				return 62;
			} else if (c == '/') {
				return 63;
			}
		} else if (alphabet == alphabet::url_filename_safe) {
			if (c == '-') {
				return 62;
			} else if (c == '_') {
				return 63;
			}
		} else {
			if (c == '+') {
				alphabet = alphabet::normal;

				return 62;
			} else if (c == '/') {
				alphabet = alphabet::normal;

				return 63;
			} else if (c == '-') {
				alphabet = alphabet::url_filename_safe;

				return 62;
			} else if (c == '_') {
				alphabet = alphabet::url_filename_safe;

				return 63;
			}
		}

		throw base64_error("invalid base64 character.");
	}

	static size_t do_decode(const char* input, size_t length, char* output, alphabet alphabet)
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