#include <sstream>
#include "biglong.h"

namespace blong
{
	biglong biglong::get_random(const biglong& max)
	{
		std::stringstream strstream;
		strstream << std::hex;
		for(size_t i=0; i < max.value.size()*HEX_DIGITS/2; ++i)
			strstream << rand()%256;

		biglong random;
		random.from_string(strstream.str());
		random.value.back() %= max.value.back();
		random.remove_leading_zeros();

		return random;
	}

	biglong biglong::gcd(const biglong& a, const biglong& b)
	{
		if(a==zero)
			return b;
		if(b==zero)
			return a;

		biglong q = zero;
		biglong r0 = a;
		biglong r1 = b;
		biglong r2 = r0.div(r1,q);

		while(r2 > zero)
		{
			r0 = r1;
			r1 = r2;
			r2 = r0.div(r1,q);
		}

		return r1;
	}
}