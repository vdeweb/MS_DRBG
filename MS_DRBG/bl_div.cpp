#include "biglong.h"

namespace blong
{
	biglong biglong::div(const biglong& right_op, biglong& quot) const
	{
		if(right_op.value.back() == 0)
			throw;

		if(*this < right_op)
		{
			quot = 0;
			return *this;
		}

		if(right_op.value.size()==1)
		{
			quot.value.clear();
			quot.value.resize(value.size(),0);

			UNSIGINT r = 0;
			UNSIGINT v = right_op.value.back();
			for(SIGINT i=value.size()-1; i >=0; --i)
			{
				UNSIGINT temp1 = r*BASE;
				UNSIGINT temp2 = value[i];
			
				quot.value[i] = temp1/v + temp2/v;
				r = temp1%v + temp2%v;

				if(r >= v)
				{
					r-=v;
					++(quot.value[i]);
				}
			}
			quot.remove_leading_zeros();
		
			return biglong(r);
		}

		// else...
		SIGINT n = right_op.value.size();
		SIGINT m = value.size()-n;

		quot.value.clear();
		quot.value.resize(m+1,0);

		UNSIGINT norm = static_cast<UNSIGINT>(BASE/(right_op.value.back()+1));
		biglong left = *this * norm;
		biglong right = right_op * norm;

		if(left.value.size()-n==m)
			left.value.push_back(0);

		for(SIGINT j = m; j>=0; --j)
		{
			UNSIGINT u2 = left.value[j+n]*BASE;
			UNSIGINT u1 = left.value[j+n-1];
			UNSIGINT v = right.value[n-1];
			
			UNSIGINT q = u2/v + u1/v;
			UNSIGINT rem = u2%v + u1%v;

			if(rem >= v)
			{
				rem-=v;
				++q;
			}
					
			biglong right_n_2 = biglong(right.value[n-2]);
			biglong left_j_n_2 = biglong(left.value[j+n-2]);
			while(rem < BASE)
			{				
				if(q == BASE || right_n_2 * q > left_j_n_2 + BASE*rem)
				{
					rem+=v;
					--q;
				}
				else break;
			}

			UNSIGINT carry_prev = 0, temp1, temp2;
			SIGINT borrow = 0;
			SIGINT i=0;
			for(i; i < n; ++i)
			{
				UNSIGINT carry_curr;

				temp1 = right.value[i]*q;
				carry_curr = temp1 >> BASE_POWER;
				temp1 &= REM_MASK;

				temp1+=carry_prev;
				carry_curr+=temp1 >> BASE_POWER;
				temp1 &= REM_MASK;

				carry_prev = carry_curr;

				temp2 = left.value[j+i];
				if(temp2 < temp1-borrow)
				{
					left.value[j+i] = temp2 + BASE - temp1 + borrow;
					borrow = -1;
				}
				else
				{
					left.value[j+i] = temp2 - temp1 + borrow;
					borrow = 0;
				}
			}

			temp2 = left.value[j+i];
			if(temp2 < carry_prev-borrow)
			{
				left.value[j+i] = temp2 + BASE - carry_prev + borrow;
				borrow = -1;
			}
			else
			{
				left.value[j+i] = temp2 - carry_prev + borrow;
				borrow = 0;
			}

			if(borrow==0)
				quot.value[j]=q;
			else
			{
				quot.value[j]=q-1;

				carry_prev = 0;
				for(i=0; i < n; ++i)
				{
					left.value[j+i] += right.value[i]+carry_prev;
					if(left.value[j+i] >= BASE)
					{
						carry_prev=left.value[j+i] >> BASE_POWER;
						left.value[j+i] &= REM_MASK;
					}
					else carry_prev = 0;
				}
				left.value[j+i]+=carry_prev-BASE;
			}
		}
		quot.remove_leading_zeros();
		left.remove_leading_zeros();

		if(norm > 1)
		{
			UNSIGINT r = 0;
			for(SIGINT i=left.value.size()-1; i >=0; --i)
			{
				UNSIGINT temp1 = r*BASE;
				UNSIGINT temp2 = left.value[i];
				
				left.value[i] = temp1/norm + temp2/norm;
				r = temp1%norm + temp2%norm;
				
				if(r >= norm)
				{
					r-=norm;
					++(left.value[i]);
				}
			}
			left.remove_leading_zeros();
		}

		return left;
	}
}