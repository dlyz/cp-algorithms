#pragma once
#include "header.h"

template<typename T> T gcd(T a, T b)
{
	return b ? gcd(b, a%b) : a;
}

template<typename T> T lcm(T a, T b)
{
	return a/gcd(a, b)*b;
}

//до n включительно
int GetPrimes(int n, vector<bool>& prime)
{
	int cnt = 0;
	n++;
	prime.assign(n, true);
	if (n >= 0) prime[0] = false;
	if (n >= 1) prime[1] = false;
	forn(i, n)
	{
		if (prime[i])
		{
			++cnt;
			for(int j = 2*i; j < n; j += i)
			{
				prime[j] = false;
			}
		}
	}
	return cnt;
}

//до n включительно
vint GetPrimes(int n)
{
	vector<bool> vb;
	GetPrimes(n, vb);
	vint res;

	forn(i, vb.size())
	{
		if (vb[i]) res.push_back(i);
	}
	return res;
}

// TODO: improve to sqrt(n)
// TODO: go only with primes
void GetPrimeDivisors(int64 n, vector<pii>& v)
{
	v.clear();
	for(int i = 2; i <= n; i++)
	{
		if (n%i == 0)
		{
			v.push_back(mp(i, 0));
			do
			{
				++v.back().second;
				n /= i;
			}
			while (n%i == 0);
		}
	}
}

// TODO: improve to sqrt(n)
void GetPrimeDivisors(int64 n, vint& v)
{
	v.clear();
	for(int i = 2; i <= n && n > 1; i++)
	{
		while(n%i == 0)
		{
			v.push_back(i);
			n /= i;
		}
	}
}


template<typename T, typename U>
T Pow(const T& x, U y)
{
	if (y == 0) return T(1);
	T p = Pow(x, y >> 1);
	p *= p;
	if (y & 1)
	{
		p *= x;
	}
	return p;
}


template<typename T, typename U>
T PowMod(const T& x, U y, const T& MOD)
{
	if (y == 0) return T(1);
	T p = Pow(x, y >> 1);
	p *= p;
	p %= MOD;
	if (y & 1)
	{
		p *= x;
		p %= MOD;
	}
	return p;
}

// TODO: TEST!
template<typename T, typename U>
T PowNRec(const T& x, U y)
{
	int cnt = 0;
	U ty = y;
	while (ty)
	{
		++cnt;
		ty >>= 1;
	}

	T p = T(1);
	while (cnt)
	{
		p *= p;
		if (y & bit(--cnt))
		{
			p *= x;
		}
	}

	return p;
}
