#pragma once
#include "header.h"

//size = n, k from zero, n*n*logn
vint GetPermutation(int n, int k)
{
	vint p(n);
	forn(i, n)
	{
		p[i] = i+1;
	}
		
	const int MAXFACTOR = 15;
	vint64 factors(MAXFACTOR, 1);
	for(int i = 1; i < MAXFACTOR; i++)
	{
		factors[i] = factors[i-1] * i;
	}

	if (k >= factors[n])
	{
		//такой перестановки не существует
		return vint();
	}

	for(int i = 0; i < n; i++)
	{
		int ost = n - i - 1;
		int j = i + k/factors[ost];
		k %= factors[ost];
		swap(p[i], p[j]);
		sort(p.begin()+i+1, p.end());
	}
	return p;
}