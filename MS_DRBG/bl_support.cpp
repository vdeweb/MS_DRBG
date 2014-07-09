#include <iomanip>
#include <sstream>
#include "biglong.h"

namespace blong
{
	void biglong::normalize()
	{
		UNSIGINT quot_prev = 0;
		for (size_t i = 0; i < value.size(); ++i)
		{
			UNSIGINT quot_curr = 0;

			if (value[i] >= BASE)
			{
				quot_curr = value[i] >> BASE_POWER;
				value[i] &= REM_MASK;
			}

			value[i]+=quot_prev;

			if (value[i] >= BASE)
			{
				quot_curr += value[i] >> BASE_POWER;
				value[i] &= REM_MASK;
			}

			quot_prev = quot_curr;
			if(i == value.size()-1 && quot_prev != 0)
				value.push_back(0);
		}
	}

	void biglong::remove_leading_zeros()
	{
		for (SIGINT i = value.size() - 1; i > 0; --i)
		{
			if (value[i] != 0) return;
			value.erase(value.begin() + i);
		}
	}

	void biglong::from_string(const std::string& hex_string)
	{
		if(hex_string.empty()) throw;

		value.clear();
		value.reserve(hex_string.size() / HEX_DIGITS + 1);

		SIGINT i = hex_string.size() - HEX_DIGITS;
		for (i; i >= 0; i -= HEX_DIGITS)
			value.push_back(strtoul(hex_string.substr(i, HEX_DIGITS).c_str(), nullptr, 16));
		if (-i < HEX_DIGITS)
			value.push_back(strtoul(hex_string.substr(0, i + HEX_DIGITS).c_str(), nullptr, 16));

		remove_leading_zeros();
	}

	std::string biglong::to_string() const
	{
		std::stringstream strstream;
		strstream << std::hex << value.back();
		for (SIGINT i = value.size() - 2; i >= 0; --i)
			strstream << std::setfill('0') << std::setw(HEX_DIGITS) << value[i];
		return strstream.str();
	}

	bool biglong::isOdd() const
	{
		return value[0]&1;
	}

	size_t biglong::get_raw_bytes(unsigned char*& raw_bytes) const
	{
		size_t digit_size = sizeof(SHORT_UNSIGINT);
		raw_bytes = new unsigned char[digit_size*value.size()];

		for(size_t i=0; i < value.size(); ++i)
		{
			for(size_t j=0; j < digit_size; ++j)
				raw_bytes[i*digit_size+j] = (value[i] >> (8*j)) & 0xff;
		}

		return digit_size*value.size();
	}

	size_t biglong::get_raw_bytes(unsigned char*& raw_bytes,
	                                           const size_t min_length) const
	{
		size_t digit_size = sizeof(SHORT_UNSIGINT);
		size_t vector_byte_size = digit_size*value.size();
		size_t length = vector_byte_size > min_length ? vector_byte_size : min_length;
		raw_bytes = new unsigned char[length];

		for(size_t i=0; i < value.size(); ++i)
		{
			for(size_t j=0; j < digit_size; ++j)
				raw_bytes[i*digit_size+j] = (value[i] >> (8*j)) & 0xff;
		}

		for(size_t i = vector_byte_size; i < min_length; ++i)
			raw_bytes[i] = 0;

		return length;
	}
}