#pragma once
#include "header.h"

const int MAXN = 100001;

struct Hash
{
	static const uint64 P = 59;
	static uint64 ppow[];
	static struct _Init
	{
		_Init()
		{
			ppow[0] = 1;
			for(int i = 1; i < MAXN; i++)
			{
				ppow[i] = ppow[i-1]*P;
			}
		}
	} _init;

	static uint64 GetCode(char c)
	{
		return islower(c) ? c - 'a' + 1 : ('z' - 'a' + 1) + (c - 'A' + 1);
	}

	int n;
	vector<uint64> h; 

	void Build(const string &s)
	{
		n = s.size();
		h.resize(n);

		for(int i = 0; i < n; i++)
		{
			h[i] = GetCode(s[i]) * ppow[i] + (i > 0 ? h[i-1] : 0);
		}
	}

	uint64 GetHash(int i, int j)
	{
		return (h[j] - (i > 0 ? h[i-1] : 0)) * ppow[MAXN-i-1];
	}
};

Hash::_Init Hash::_init = Hash::_Init();
uint64 Hash::ppow[MAXN];
