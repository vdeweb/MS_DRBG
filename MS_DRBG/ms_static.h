#include <windows.h>
#pragma comment(lib, "advapi32.lib")

#include "sha1.h"

namespace drbg
{
	inline unsigned char* ms_drbg::get_raw_bytes(const size_t value)
	{
		size_t size = sizeof(value);
		unsigned char* raw_bytes = new unsigned char[size];

		for(size_t i=0; i < size; ++i)
			raw_bytes[i] = (value >> (8*i)) & 0xff;

		return raw_bytes;
	}

	inline unsigned char* ms_drbg::get_entropy(const size_t length)
	{
		HCRYPTPROV hProvider = NULL;

		CryptAcquireContextW(&hProvider, nullptr, nullptr, PROV_RSA_FULL,
		                     CRYPT_VERIFYCONTEXT | CRYPT_SILENT);

		unsigned char* entropy_input = new unsigned char[length];

		CryptGenRandom(hProvider, static_cast<DWORD>(length), entropy_input);
		CryptReleaseContext(hProvider, NULL);

		return entropy_input;
	}

	inline unsigned char* ms_drbg::concatenate(const void* left, const size_t left_size,
	                                           const void* right, const size_t right_size)
	{
		unsigned char* result = new unsigned char[left_size+right_size];
		memcpy(result, left, left_size);
		memcpy(result+left_size, right, right_size);
		return result;
	}

	inline string ms_drbg::hash_df_hex(const unsigned char* seed_material,
	                                   const size_t seed_material_size,
									   const size_t seedlen)
	{
		size_t len = seedlen/20 + (seedlen%20 ? 1 : 0);
		string result = "";
		unsigned char hash[20];
		char hexstring[41];
		for(size_t counter=1; counter <= len; ++counter)
		{
			unsigned char* counter_bytes = get_raw_bytes(counter);
			size_t counter_size = sizeof(counter);
			unsigned char* seedlen_bytes = get_raw_bytes(seedlen);
			size_t seedlen_size = sizeof(seedlen_size);
			unsigned char* counter_seedlen_bytes = concatenate(counter_bytes,
			                                                   counter_size, seedlen_bytes,
															   seedlen_size);
			size_t counter_seedlen_size = counter_size + seedlen_size;
			delete[] counter_bytes;
			delete[] seedlen_bytes;

			unsigned char* hash_material = concatenate(counter_seedlen_bytes,
			                                           counter_seedlen_size, seed_material,
													   seed_material_size);
			size_t hash_material_size = counter_seedlen_size + seed_material_size;
			delete[] counter_seedlen_bytes;

			sha1::calc(hash_material, static_cast<int>(hash_material_size), hash);
			sha1::to_hex_string(hash, hexstring);
			result += string(hexstring);

			delete[] hash_material;
		}

		return result.substr(0,2*seedlen);
	}

	inline void ms_drbg::reseed(biglong& intenal_state, const size_t byte_len, const string& additional_input_string)
	{
		unsigned char* entropy_input = get_entropy(byte_len);
		unsigned char* internal_bytes = nullptr;
		size_t internal_size = intenal_state.get_raw_bytes(internal_bytes);
		unsigned char* internal_entropy_bytes = concatenate(internal_bytes,
		                                                    internal_size, entropy_input, byte_len);
		size_t internal_entropy_size = internal_size + byte_len;
		delete[] entropy_input;
		delete[] internal_bytes;

		unsigned char* internal_entropy_additional_bytes = concatenate(internal_entropy_bytes, internal_entropy_size,
		                                                               additional_input_string.c_str(),
		                                                               additional_input_string.length());
		size_t internal_entropy_additional_size = internal_entropy_size + additional_input_string.length();
		delete[] internal_entropy_bytes;

		intenal_state = hash_df_hex(internal_entropy_additional_bytes, internal_entropy_additional_size, byte_len);
		delete[] internal_entropy_additional_bytes;
	}
}
