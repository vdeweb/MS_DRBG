#include <iomanip>
#include "biglong.h"

namespace blong
{
	std::istream& operator>>(std::istream &stream, biglong &n)
	{
		std::string str;
		stream >> str;
		n.from_string(str);
		return stream;
	}

	std::ostream& operator<<(std::ostream &stream, const biglong &n)
	{
		stream << std::hex << n.value.back();
		for (SIGINT i = n.value.size() - 2; i >= 0; --i)
			stream << std::setfill('0') << std::setw(HEX_DIGITS) << n.value[i];

		return stream;
	}
}