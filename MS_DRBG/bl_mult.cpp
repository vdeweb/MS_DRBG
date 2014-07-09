#include <bitset>
#include "biglong.h"

namespace blong
{
	biglong biglong::operator*(const biglong& right_op) const
	{
		biglong result;
		result.value.resize(value.size()+right_op.value.size(),0);

		for(size_t j=0; j < right_op.value.size(); ++j)
		{
			UNSIGINT carry_prev = 0;	
			size_t i=0;
			for(i; i < value.size(); ++i)	
			{			
				UNSIGINT temp_prod = value[i]*right_op.value[j];

				UNSIGINT carry_curr = 0;

				if (temp_prod >= BASE)
				{
					carry_curr = temp_prod >> BASE_POWER;
					temp_prod &= REM_MASK;	
				}
			
				SIGINT k = i+j;
				result.value[k] += temp_prod + carry_prev;

				if (result.value[k] >= BASE)
				{
					carry_curr += result.value[k] >> BASE_POWER;
					result.value[k] &= REM_MASK;	
				}

				carry_prev = carry_curr;
			}

			i=value.size()+j;
			while(carry_prev != 0)
			{
				if(i >= result.value.size())
					result.value.push_back(0);

				UNSIGINT quot_curr = 0;

				if (carry_prev >= BASE)
				{
					quot_curr = carry_prev >> BASE_POWER;
					carry_prev &= REM_MASK;	
				}
			
				result.value[i] += carry_prev;

				if (result.value[i] >= BASE)
				{
					quot_curr += result.value[i] >> BASE_POWER;
					result.value[i] &= REM_MASK;	
				}

				carry_prev = quot_curr;

				++i;
			}
		}
		result.remove_leading_zeros();

		return result;
	}

	biglong biglong::operator*(SHORT_UNSIGINT right_op) const
	{
		biglong result;
		result.value.resize(value.size(),0);	

		UNSIGINT carry_prev = 0;	
		size_t i=0;
		for(i; i < value.size(); ++i)	
		{
			UNSIGINT temp_prod = value[i]*right_op;
		
			UNSIGINT carry_curr = 0;
		
			if (temp_prod >= BASE)
			{
				carry_curr = temp_prod >> BASE_POWER;
				temp_prod &= REM_MASK;	
			}
			
			result.value[i] += temp_prod + carry_prev;
		
			if (result.value[i] >= BASE)
			{
				carry_curr += result.value[i] >> BASE_POWER;
				result.value[i] &= REM_MASK;	
			}
		
			carry_prev = carry_curr;
		}
	
		i=result.value.size();
		while(carry_prev != 0)
		{
			result.value.push_back(0);
		
			UNSIGINT carry_curr = 0;
		
			if (carry_prev >= BASE)
			{
				carry_curr = carry_prev >> BASE_POWER;
				carry_prev &= REM_MASK;	
			}
			
			result.value[i] += carry_prev;
		
			if (result.value[i] >= BASE)
			{
				carry_curr += result.value[i] >> BASE_POWER;
				result.value[i] &= REM_MASK;	
			}
		
			carry_prev = carry_curr;
		
			++i;
		}	
		result.remove_leading_zeros();

		return result;
	}

	biglong biglong::operator*(UNSIGINT right_op) const
	{
		biglong right_long = biglong(right_op);
		return (*this)*right_long;
	}

	biglong biglong::power_mod(const biglong& power, const biglong& modulo) const
	{
		if(power == one)
			return *this;
		if(power == zero)
			return one;

		biglong result = one;
		biglong squared = *this;
		biglong quot = zero;
		for(size_t i=0; i<power.value.size(); ++i)
		{
			std::bitset<BASE_POWER> bin_power(static_cast<unsigned long long>(power.value[i]));
			for(int j=0; j < BASE_POWER; ++j)
			{
				if(bin_power.test(j))
					result = (result*squared).div(modulo, quot);
				squared = (squared*squared).div(modulo, quot);
			}
		}
		return result;
	}
}