#include "biglong.h"

namespace blong
{
	biglong& biglong::operator++()
	{
		++value[0];
		for(size_t i=0; value[i]>=BASE && i < value.size(); ++i)
		{
			value[i]=0;
			if(i < value.size()-1)
				++value[i+1];
			else 
			{
				value.push_back(1);
				break;
			}
		}
		return *this;
	}

	biglong biglong::operator+(const biglong& right_op) const
	{
		biglong result;
		size_t max_size = value.size() > right_op.value.size() ? value.size() : right_op.value.size();
		result.value.resize(max_size, 0);
	
		UNSIGINT carry_prev = 0;
		for (size_t i = 0; i < result.value.size(); ++i)
		{
			result.value[i] = (i < value.size() ? value[i] : 0) + 
				(i < right_op.value.size() ? right_op.value[i] : 0) + carry_prev;

			carry_prev = 0;

			if (result.value[i] >= BASE)
			{
				carry_prev = result.value[i] >> BASE_POWER;
				result.value[i] &= REM_MASK;	
			}
		}
		if(carry_prev!=0)
			result.value.push_back(carry_prev);

		return result;
	}

	biglong biglong::operator+(UNSIGINT right_op) const
	{
		biglong right_long = biglong(right_op);
		return (*this)+right_long;
	}

	biglong& biglong::operator+=(const biglong& right_op)
	{
		size_t max_size = value.size() > right_op.value.size() ? value.size() : right_op.value.size();
		value.resize(max_size, 0);
	
		UNSIGINT carry_prev = 0;
		for (size_t i = 0; i < value.size(); ++i)
		{
			value[i] = (i < value.size() ? value[i] : 0) + 
				(i < right_op.value.size() ? right_op.value[i] : 0) + carry_prev;

			if (value[i] >= BASE)
			{
				carry_prev = value[i] >> BASE_POWER;
				value[i] &= REM_MASK;	
			}
		}
		if(carry_prev!=0)
			value.push_back(carry_prev);

		return *this;
	}
}