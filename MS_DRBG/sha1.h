#ifndef SHA1
#define SHA1

namespace drbg
{
	class Sha1
	{
	public :
		static void calc(const void* src, const int bytelength, unsigned char* hash);
		static void to_hex_string(const unsigned char* hash, char* hexstring);
		static unsigned int hash_len_in_bytes();

	private :
		static unsigned int rol(const unsigned int value, const unsigned int steps);
		static void clear_buffer(unsigned int* buffer);
		static void inner_hash(unsigned int* result, unsigned int* w);
	};

    
	// Rotate an integer value to left.
	inline unsigned int Sha1::rol(const unsigned int value, const unsigned int steps)
	{
		return ((value << steps) | (value >> (32 - steps)));
	}

	// Sets the first 16 integers in the buffer to zero.
	// Used for clearing the W buffer.
	inline void Sha1::clear_buffer(unsigned int* buffer)
	{
		for (int pos = 16; --pos >= 0;)
		{
			buffer[pos] = 0;
		}
	}

	inline void Sha1::inner_hash(unsigned int* result, unsigned int* w)
	{
		unsigned int a = result[0];
		unsigned int b = result[1];
		unsigned int c = result[2];
		unsigned int d = result[3];
		unsigned int e = result[4];

		int round = 0;

		#define sha1macro(func,val) \
		{ \
			const unsigned int t = rol(a, 5) + (func) + e + val + w[round]; \
			e = d; \
			d = c; \
			c = rol(b, 30); \
			b = a; \
			a = t; \
		}

		while (round < 16)
		{
			sha1macro((b & c) | (~b & d), 0x5a827999)
			++round;
		}
		while (round < 20)
		{
			w[round] = rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
			sha1macro((b & c) | (~b & d), 0x5a827999)
			++round;
		}
		while (round < 40)
		{
			w[round] = rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
			sha1macro(b ^ c ^ d, 0x6ed9eba1)
			++round;
		}
		while (round < 60)
		{
			w[round] = rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
			sha1macro((b & c) | (b & d) | (c & d), 0x8f1bbcdc)
			++round;
		}
		while (round < 80)
		{
			w[round] = rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
			sha1macro(b ^ c ^ d, 0xca62c1d6)
			++round;
		}

		#undef sha1macro

		result[0] += a;
		result[1] += b;
		result[2] += c;
		result[3] += d;
		result[4] += e;
	}


	inline void Sha1::calc(const void* src, const int bytelength, unsigned char* hash)
	{
		unsigned int result[5] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };

		const unsigned char* sarray = static_cast<const unsigned char*>(src);

		unsigned int w[80];

		const int endOfFullBlocks = bytelength - 64;
		int endCurrentBlock;
		int currentBlock = 0;

		while (currentBlock <= endOfFullBlocks)
		{
			endCurrentBlock = currentBlock + 64;

			for (int roundPos = 0; currentBlock < endCurrentBlock; currentBlock += 4)
			{
				w[roundPos++] = static_cast<unsigned int>(sarray[currentBlock + 3])
					| (static_cast<unsigned int>(sarray[currentBlock + 2]) << 8)
					| (static_cast<unsigned int>(sarray[currentBlock + 1]) << 16)
					| (static_cast<unsigned int>(sarray[currentBlock]) << 24);
			}
			inner_hash(result, w);
		}

		endCurrentBlock = bytelength - currentBlock;
		clear_buffer(w);
		int lastBlockBytes = 0;
		for (;lastBlockBytes < endCurrentBlock; ++lastBlockBytes)
		{
			w[lastBlockBytes >> 2] |= static_cast<unsigned int>(
					sarray[lastBlockBytes + currentBlock])
					<< ((3 - (lastBlockBytes & 3)) << 3);
		}
		w[lastBlockBytes >> 2] |= 0x80 << ((3 - (lastBlockBytes & 3)) << 3);
		if (endCurrentBlock >= 56)
		{
			inner_hash(result, w);
			clear_buffer(w);
		}
		w[15] = bytelength << 3;
		inner_hash(result, w);

		for (int hashByte = 20; --hashByte >= 0;)
		{
			hash[hashByte] = (result[hashByte >> 2]
				>> (((3 - hashByte) & 0x3) << 3)) & 0xff;
		}
	}

	inline void Sha1::to_hex_string(const unsigned char* hash, char* hexstring)
	{
		const char hexDigits[] = { "0123456789abcdef" };

		for (int hashByte = 20; --hashByte >= 0;)
		{
			hexstring[hashByte << 1] = hexDigits[(hash[hashByte] >> 4) & 0xf];
			hexstring[(hashByte << 1) + 1] = hexDigits[hash[hashByte] & 0xf];
		}
		hexstring[40] = 0;
	}

	inline unsigned int Sha1::hash_len_in_bytes()
	{
		return 20;
	}
}
#endif