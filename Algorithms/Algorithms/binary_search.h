#pragma once
#include "header.h"

double BinarySearch(double l, double r, double eps, double (*f)(double), double f0 = 0)
{
	while(r - l > eps)
	{
		double m = (l+r)/2;
		if (f(m) < 0)
		{
			l = m;
		}
		else
		{
			r = m;
		}
	}
	return (l+r)/2;
}

int LowerBound(int l, int r, int (*f)(int), int f0 = 0)
{
	while(l < r)
	{
		int m = (l+r)/2;
		if (f(m) < f0)
		{
			l = m + 1;
		}
		else
		{
			r = m;
		}
	}
	return l;
}

int UpperBound(int l, int r, int (*f)(int), int f0 = 0)
{
	while(l < r)
	{
		int m = (l+r)/2;
		if (f(m) <= f0)
		{
			l = m + 1;
		}
		else
		{
			r = m;
		}
	}
	return l;
}