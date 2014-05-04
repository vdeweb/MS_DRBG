#include "biglong.h"

namespace blong
{
	//truncated subtraction
	biglong biglong::trunc_sub(const biglong& right_op) const
	{
		if(*this <= right_op)
			return biglong(0);

		 biglong result;
		 result.value.resize(value.size(), 0);
	
		SIGINT borrow = 0;
		for (size_t i = 0; i < result.value.size(); ++i)
		{
			SIGINT temp = static_cast<SIGINT>(value[i]) - (i < right_op.value.size() ?
				static_cast<SIGINT>(right_op.value[i]) : 0) + borrow;
			if(temp < 0)
			{
				temp+=BASE;
				borrow = -1;
			}
			else borrow = 0;

			result.value[i] = static_cast<UNSIGINT>(temp);
		}
		result.remove_leading_zeros();

		return result;
	}
}