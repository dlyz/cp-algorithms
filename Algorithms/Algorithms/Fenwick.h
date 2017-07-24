#pragma once
#include "header.h"

template <typename T>
class Fenwik
{
private:
	vector<T> _t;

public:
	void assign(int size)
	{
		_t.assign(size, 0);
	}

	void assign(int size, T val)
	{
		assign(size);
		if (val == 0) return;
		
		for (int i = 0; i < size; ++i)
		{
			increment(i, val);
		}		
	}

	void assign(const vector<T>& vals)
	{
		assign(vals.size());
		for (int i = 0; i < vals.size(); ++i)
		{
			increment(i, vals[i]);
		}
	}

	void increment(int i, T val)
	{
		for (; i < _t.size(); i = (i | (i+1)))
			_t[i] += val;
	}

	T sum(int r) const
	{
		T res = 0;
		for (; r >= 0; r = (r & (r+1)) - 1)
			res += _t[r];
		return res;
	}

	T sum(int l, int r) const
	{
		return sum(r) - sum(l-1);
	}
};

//Finds first free place in segment of array of ones.
/*
	int FindFree(int l, int r)
	{
		while(l < r)
		{
			int m = (l+r)/2;
			if (Sum(l, m) >= m-l+1)
				l = m + 1;
			else
				r = m;
		}
		return l;
	}
*/