#pragma once
#include "header.h"

struct Node
{
	vint to;
	int tin, tout;
	int deep;
	vint ps;

	int p() const { return ps.front(); }

	bool IsParentFor(const Node& child) const
	{
		return tin <= child.tin && tout >= child.tout;
	}
};

class Lca
{
public:
	int n;
	int l;
	Node& g(int index) { return (*_g)[index]; }
	const Node& g(int index) const { return (*_g)[index]; }

	void Init(vector<Node>& gRef)
	{
		_g = &gRef;
		n = _g->size();
		l = 1;
		while ((1 << l) <= n) ++l;

		g(0).deep = -1;
		int time = 0;
		_Dfs(0, 0, time);
	}

	int GetLca(int a, int b) const
	{
		if (g(a).IsParentFor(g(b))) return a;
		if (g(b).IsParentFor(g(a))) return b;
		for (int i = l; i >= 0; --i)
		{
			int p = g(a).ps[i];
			if (!g(p).IsParentFor(g(b)))
			{
				a = p;
			}
		}
		return g(a).p();
	}

	// from a to b
	int FindDist(int a, int b)
	{
		int l = GetLca(a, b);
		return g(a).deep - g(l).deep + g(b).deep - g(l).deep;
	}

	// from c to a-b path
	int FindDistToPath(int a, int b, int c)
	{
		int f = GetLca(a, b);
		return min(_FindDistToPath(a, f, c), _FindDistToPath(b, f, c));
	}

private:
	vector<Node>* _g;
	void _Dfs(int i, int from, int& time)
	{
		Node& myg = g(i);
		vint& myps = myg.ps;
		myps.assign(l + 1, 0);
		for (int j = 0, p = from; p != 0; ++j)
		{
			myps[j] = p;
			p = g(p).ps[j];
		}

		myg.deep = g(from).deep + 1;
		myg.tin = ++time;
		forn(j, myg.to.size())
		{
			int to = myg.to[j];
			if (to == from) continue;

			_Dfs(to, i, time);
		}
		myg.tout = ++time;
	}
	int _FindDistToPath(int a, int f, int c)
	{
		int l = GetLca(a, c);
		int res = g(c).deep - g(l).deep;
		if (!g(f).IsParentFor(g(l)))
		{
			res += g(f).deep - g(l).deep;
		}
		return res;
	}

};