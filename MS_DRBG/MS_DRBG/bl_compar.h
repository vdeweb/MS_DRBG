#include "biglong.h"

namespace blong
{
	inline bool biglong::operator==(const biglong& right_op) const
	{
		if (value.size() != right_op.value.size())
			return false;
		for (SIGINT i = value.size() - 1; i >= 0; --i)
			if (value[i] != right_op.value[i])
				return false;    
		return true;
	}

	inline bool biglong::operator!=(const biglong& right_op) const
	{
		if (value.size() != right_op.value.size())
			return true;
		for (SIGINT i = value.size() - 1; i >= 0; --i)
			if (value[i] != right_op.value[i])
				return true; 
		return false;
	}

	inline bool biglong::operator<(const biglong& right_op) const
	{
		if (value.size() > right_op.value.size())
			return false;
		if (value.size() < right_op.value.size())
			return true;

		for (SIGINT i = value.size() - 1; i >= 0; --i)
		{
			if (value[i] < right_op.value[i])
				return true;
			if (value[i] > right_op.value[i])
				return false;
		}
		return false;
	}

	inline bool biglong::operator<=(const biglong& right_op) const
	{
		if (value.size() > right_op.value.size())
			return false;
		if (value.size() < right_op.value.size())
			return true;

		for (SIGINT i = value.size() - 1; i >= 0; --i)
		{
			if (value[i] < right_op.value[i])
				return true;
			if (value[i] > right_op.value[i])
				return false;
		}
		return true;
	}

	inline bool biglong::operator>(const biglong& right_op) const
	{
		if (value.size() > right_op.value.size())
			return true;
		if (value.size() < right_op.value.size())
			return false;

		for (SIGINT i = value.size() - 1; i >= 0; --i)
		{
			if (value[i] < right_op.value[i])
				return false;
			if (value[i] > right_op.value[i])
				return true;
		}
		return false;
	}

	inline bool biglong::operator>=(const biglong& right_op) const
	{
		if (value.size() > right_op.value.size())
			return true;
		if (value.size() < right_op.value.size())
			return false;

		for (SIGINT i = value.size() - 1; i >= 0; --i)
		{
			if (value[i] < right_op.value[i])
				return false;
			if (value[i] > right_op.value[i])
				return true;
		}
		return true;
	}
}