#pragma once
#include "header.h"

#define L(i) ((i) << 1)
#define R(i) (((i) << 1) | 1)
#define M(l, r) (((l) + (r)) >> 1)

struct SegmentTree
{
	int n;
	vint v;
	vint p;
	void Init(const vint & a)
	{
		n = a.size();
		v.resize(4*(n+1));
		p.resize(4*(n+1));
		Build(1, 0, n-1, a);
	}

	void Build(int i, int l, int r, const vint & a)
	{
		if (l == r)
		{
			v[i] = a[l];
			p[i] = l;
			return;
		}

		int m = M(l, r);
		Build(L(i), l, m, a);
		Build(R(i), m+1, r, a);

		int from = (v[L(i)] <= v[R(i)] ? L(i) : R(i));
		v[i] = v[from];
		p[i] = p[from];
	}

	pii Min(int i, int l, int r, int ll, int rr) const
	{
		if (l == ll && r == rr)
		{
			return mp(v[i], p[i]);
		}
		
		int m = M(l, r);
		if (rr <= m)
		{
			return Min(L(i), l, m, ll, rr);
		}
		else if (ll > m)
		{			
			return Min(R(i), m+1, r, ll, rr);
		}
		else
		{			
			return min(
				Min(L(i), l, m, ll, m),
				Min(R(i), m+1, r, m+1, rr));
		}
	}

	int MinPos(int l, int r) const
	{
		return Min(1, 0, n-1, l, r).second;
	}
};

struct Node
{
	vint to;
};

struct Lca
{
	int n;
	vint w;
	vint deep;
	vint pos;
	SegmentTree st;

	void Init(const vector<Node> & g)
	{
		n = g.size();
		w.reserve(2*n);
		deep.reserve(2*n);
		
		DFS(0, -1, g);
		
		st.Init(deep);
		
		pos.resize(n);
		forr(i, w.size())
		{
			pos[w[i]] = i;
		}
	}

	void DFS(int i, int from, const vector<Node> & g)
	{
		int d = deep.empty() ? 0 : deep.back()+1;

		w.push_back(i);
		deep.push_back(d);
		forn(j, g[i].to.size())
		{
			int to = g[i].to[j];
			if (to == from) continue;

			DFS(to, i, g);
			w.push_back(i);
			deep.push_back(d);
		}
	}

	int GetLca(int a, int b)
	{
		if (pos[a] > pos[b])
			swap(a, b);
		return w[st.MinPos(pos[a], pos[b])];
	}
};