#pragma once
#include "header.h"

const int UNDEF = -1e9;

template<typename T>
class SegmentTree
{
private:

	struct Pos
	{
		int i;
		int l, r;
		Pos(int i, int l, int r) : i(i), l(l), r(r) {}

		bool is_leaf() const { return l == r; }

		int m() const { return (l + r) >> 1; }
		int cli() const { return i << 1; }
		int cri() const { return cli() | 1; }
		int len() const { return r - l + 1; }

		Pos cl() const { return Pos(cli(), l, m()); }
		Pos cr() const { return Pos(cri(), m() + 1, r); }
	};

	struct Query
	{
		int l, r;
		T val;

		Query() {}
		Query(int l, int r, T val) : l(l), r(r), val(val) {}

		bool has_common(const Pos& p) const
		{
			return !(r < p.l || l > p.r);
		}
		bool includes(const Pos& p) const
		{
			return l == p.l && r == p.r;
		}
	};

	struct Node;
	mutable vector<Node> _t;
	int _size;

	Pos root_pos() const { return Pos(1, 0, _size - 1); }
	Node& node(const Pos& p) const { return _t[p.i]; }
	Node& node(int i) const { return _t[i]; }


	struct ConstArray
	{
		T val;
		ConstArray(const T& val) : val(val) {}

		const T& operator[](int i) const { return val; }
	};

	template<typename TContainer>
	void build(Pos p, const TContainer& vals)
	{
		if (p.is_leaf())
		{
			node(p).sum = vals[p.l];
			return;
		}

		build(p.cl(), vals);
		build(p.cr(), vals);
		recalc(p);
	}

	template<typename TOperation>
	typename TOperation::result_type calc(TOperation op, Pos p, const Query& q) const
	{
		if (!q.has_common(p)) return op.def();

		push(p);
		if (q.includes(p))
		{
			return op.get(node(p));
		}

		return op.merge(
			calc(op, p.cl(), q),
			calc(op, p.cr(), q)
		);
	}

	template<typename TOperation>
	void modify(TOperation op, Pos p, const Query& q)
	{
		if (!q.has_common(p)) return;
		if (q.includes(p))
		{
			op.apply(node(p), q.val);
			push(p);
			return;
		}

		push(p);
		modify(op, p.cl(), q);
		modify(op, p.cr(), q);
		recalc(p);
	}

	template<typename TOperation>
	void apply_children(TOperation op, const Pos& p, const T& val) const
	{
		if (p.is_leaf()) return;
		
		op.apply(node(p.cli()), val);
		op.apply(node(p.cri()), val);
	}

public:
	void assign(int size)
	{
		_size = size;
		int tsize = 1;
		while (tsize < size) tsize <<= 1;
		tsize <<= 1;
		_t.assign(tsize, Node());
	}

	void assign(int size, const T& val)
	{
		assign(size);
		if (val == 0) return;
		build(root_pos(), ConstArray(val));
	}

	void assign(const vector<T>& vals)
	{
		assign(vals.size());
		build(root_pos(), vals);
	}

private:

#pragma region edit here

	struct Node
	{
		T sum;
		T add;
		T set;
		Node() : sum(0), add(0), set(UNDEF) {}
	};

	void recalc(const Pos& p)
	{
		node(p).sum = node(p.cli()).sum + node(p.cri()).sum;
	}

	void push(const Pos& p) const
	{
		Node& cur = node(p);
		if (cur.set != UNDEF)
		{
			cur.sum = cur.set * p.len();
			apply_children(set_op(), p, cur.set);
			cur.set = UNDEF;
		}
		else if (cur.add != 0)
		{
			cur.sum += cur.add * p.len();
			apply_children(add_op(), p, cur.add);
			cur.add = 0;
		}
	}


	struct add_op
	{
		void apply(Node& cur, const T& val) const
		{
			if (cur.set != UNDEF)
			{
				cur.set += val;
			}
			else
			{
				cur.add += val;
			}
		}
	};

	struct set_op
	{
		void apply(Node& cur, const T& val) const
		{
			cur.add = 0;
			cur.set = val;
		}
	};

	struct sum_op
	{
		typedef T result_type;
		T merge(T left, T right) const
		{
			return left + right;
		}
		T get(const Node& cur) const { return cur.sum; }
		T def() const { return 0; }
	};

public:

	T sum(int l, int r) const
	{
		return calc(sum_op(), root_pos(), Query(l, r, 0));
	}

	void add(int l, int r, T val)
	{
		return modify(add_op(), root_pos(), Query(l, r, val));
	}

	void set(int l, int r, T val)
	{
		return modify(set_op(), root_pos(), Query(l, r, val));
	}

#pragma endregion



};
