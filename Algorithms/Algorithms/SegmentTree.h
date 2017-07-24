#pragma once
#include "header.h"


namespace segment_tree
{
	struct Pos
	{
		int i;
		int l, r;

		Pos(int i, int l, int r) : i(i), l(l), r(r) {}

		bool is_leaf() const { return l == r; }
		int leaf_index() const { return l; }

		int m() const { return (l + r) >> 1; }
		int ileft() const { return i << 1; }
		int iright() const { return ileft() | 1; }
		int len() const { return r - l + 1; }

		Pos pleft() const { return Pos(ileft(), l, m()); }
		Pos pright() const { return Pos(iright(), m() + 1, r); }
	};

	struct Query
	{
		int l, r;

		Query() {}
		Query(int l, int r) : l(l), r(r) {}

		bool has_common(const Pos& p) const
		{
			return !(r < p.l || l > p.r);
		}
		bool includes(const Pos& p) const
		{
			return l == p.l && r == p.r;
		}

	};

	template<typename TNode>
	class Tree
	{
	public:
		typedef TNode node_type;

	private:
		mutable vector<node_type> _t;
		int _size;

	public:
		int size() const { return _size; }
		node_type& node(int i) const { return _t[i]; }

		void assign(int size)
		{
			_size = size;
			int tsize = 1;
			while (tsize < size) tsize <<= 1;
			tsize <<= 1;
			_t.assign(tsize, node_type());
		}
	};


	template<typename TNode>
	struct Iter : public Pos
	{
		typedef TNode node_type;
		typedef Tree<node_type> tree_type;

		const tree_type& tree;

		Iter(const tree_type& tree) : Pos(1, 0, tree.size() - 1), tree(tree) {}
		Iter(const tree_type& tree, const Pos& p) : Pos(p), tree(tree) {}
		
		Iter left() const { return Iter(tree, pleft()); }
		Iter right() const { return Iter(tree, pright()); }

		node_type& operator*() const { return tree.node(i); }
		node_type* operator->() const { return &tree.node(i); }
	};
	
	template<typename TOperation, typename TNode>
	void apply_to_children(TOperation& op, const Iter<TNode>& it)
	{
		if (it.is_leaf()) return;

		op.apply(it.left());
		op.apply(it.right());
	}
	
	template<typename TOperation, typename TNode>
	void build(TOperation& op, Iter<TNode> it)
	{
		if (it.is_leaf())
		{
			op.init(it);
			return;
		}

		build(op, it.left());
		build(op, it.right());
		op.recalc(it);
	}

	template<typename TOperation, typename TNode>
	typename TOperation::result_type calc(TOperation& op, Iter<TNode> it, const Query& q)
	{
		if (!q.has_common(it)) return op.get();

		op.push(it);
		if (q.includes(it))
		{
			return op.get(it);
		}

		return op.merge(
			calc(op, it.left(), q),
			calc(op, it.right(), q)
		);
	}

	template<typename TOperation, typename TNode>
	void modify(TOperation& op, Iter<TNode> it, const Query& q)
	{
		if (!q.has_common(it)) return;
		if (q.includes(it))
		{
			op.apply(it);
			op.push(it);
			return;
		}

		op.push(it);
		modify(op, it.left(), q);
		modify(op, it.right(), q);
		op.recalc(it);
	}


}


template<typename T>
class AddSumST
{
	struct Node
	{
		T sum;
		T add;
		Node() : sum(0), add(0) {}
	};

	typedef segment_tree::Iter<Node> iter;
	segment_tree::Tree<Node> _tree;
	iter root() const { return _tree; }
	
	struct calc_op
	{
		void push(const iter& it)
		{
			Node& node = *it;
			if (node.add != 0)
			{
				segment_tree::apply_to_children(add_op(node.add), it);
				node.sum += node.add * it.len();
				node.add = 0;
			}
		}
	};

	struct mod_op : public calc_op
	{
		void recalc(const iter& it)
		{
			it->sum = it.left()->sum + it.right()->sum;
		}
	};

	struct init_vec_op : public mod_op
	{
		const vector<T>& v;
		init_vec_op(const vector<T>& v) : v(v) {}

		void init(const iter& it)
		{
			it->sum = v[it.leaf_index()];
		}
	};

	struct add_op : public mod_op
	{
		T val;
		add_op(const T& val) : val(val) {}

		void apply(const iter& it)
		{
			it->add += val;
		}
	};
	
	struct sum_op : public calc_op
	{
		typedef T result_type;
		T get() { return 0; }
		T get(const iter& it) { return it->sum; }
		T merge(T left, T right) { return left + right; }
	};

public:
	void assign(int size)
	{
		_tree.assign(size);
	}
	void assign(const vector<T>& vals)
	{
		assign(vals.size());
		segment_tree::build(init_vec_op(vals), root());
	}

	T sum(int l, int r) const
	{
		return segment_tree::calc(sum_op(), root(), segment_tree::Query(l, r));
	}

	void add(int l, int r, T val)
	{
		return segment_tree::modify(add_op(val), root(), segment_tree::Query(l, r));
	}

};


template<typename T>
class AddSetSumST
{
	static const int UNDEF = -1e9;
	struct Node
	{
		T sum;
		T add;
		T set;
		Node() : sum(0), add(0), set(UNDEF) {}
	};

	typedef segment_tree::Iter<Node> iter;
	segment_tree::Tree<Node> _tree;
	iter root() const { return _tree; }

	struct calc_op
	{
		void push(const iter& it)
		{
			Node& node = *it;
			if (node.set != UNDEF)
			{
				segment_tree::apply_to_children(set_op(node.set), it);
				node.sum = node.set * it.len();
				node.set = UNDEF;
			}
			else if (node.add != 0)
			{
				segment_tree::apply_to_children(add_op(node.add), it);
				node.sum += node.add * it.len();
				node.add = 0;
			}
		}
	};

	struct mod_op : public calc_op
	{
		void recalc(const iter& it)
		{
			it->sum = it.left()->sum + it.right()->sum;
		}
	};

	struct init_vec_op : public mod_op
	{
		const vector<T>& v;
		init_vec_op(const vector<T>& v) : v(v) {}

		void init(const iter& it)
		{
			it->sum = v[it.leaf_index()];
		}
	};
	
	struct add_op : public mod_op
	{
		T val;
		add_op(const T& val) : val(val) {}

		void apply(const iter& it)
		{
			Node& node = *it;
			if (node.set != UNDEF)
			{
				node.set += val;
			}
			else
			{
				node.add += val;
			}
		}
	};

	struct set_op : public mod_op
	{
		T val;
		set_op(const T& val) : val(val) {}

		void apply(const iter& it) const
		{
			Node& node = *it;
			node.add = 0;
			node.set = val;
		}
	};

	struct sum_op : public calc_op
	{
		typedef T result_type;
		T get() { return 0; }
		T get(const iter& it) { return it->sum; }
		T merge(T left, T right) { return left + right; }
	};

public:
	void assign(int size)
	{
		_tree.assign(size);
	}
	void assign(const vector<T>& vals)
	{
		assign(vals.size());
		segment_tree::build(init_vec_op(vals), root());
	}

	T sum(int l, int r) const
	{
		return segment_tree::calc(sum_op(), root(), segment_tree::Query(l, r));
	}

	void add(int l, int r, T val)
	{
		return segment_tree::modify(add_op(val), root(), segment_tree::Query(l, r));
	}

	void set(int l, int r, T val)
	{
		return segment_tree::modify(set_op(val), root(), segment_tree::Query(l, r));
	}

};


template<typename T>
class AddMinST
{
	static const int INF = 1e9;
	struct Node
	{
		pair<T, int> minp;
		T add;

		void init(T val, int index)
		{
			minp.first = val;
			minp.second = index;
			add = 0;
		}
	};

	typedef segment_tree::Iter<Node> iter;
	segment_tree::Tree<Node> _tree;
	iter root() const { return _tree; }
	
	struct calc_op
	{
		void push(const iter& it)
		{
			Node& node = *it;
			if (node.add != 0)
			{
				segment_tree::apply_to_children(add_op(node.add), it);
				node.minp.first += node.add;
				node.add = 0;
			}
		}
	};

	struct mod_op : public calc_op
	{
		void recalc(const iter& it)
		{
			it->minp = std::min(it.left()->minp, it.right()->minp);
		}
	};

	struct init_vec_op : public mod_op
	{
		const vector<T>& v;
		init_vec_op(const vector<T>& v) : v(v) {}

		void init(const iter& it)
		{
			it->init(v[it.leaf_index()], it.leaf_index());
		}
	};

	struct init_const_op : public mod_op
	{
		T val;
		init_const_op(const T& val) : val(val) {}

		void init(const iter& it)
		{
			it->init(val, it.leaf_index());
		}
	};

	struct add_op : public mod_op
	{
		T val;
		add_op(const T& val) : val(val) {}

		void apply(const iter& it)
		{
			it->add += val;
		}
	};

	struct min_op : public calc_op
	{
		typedef pair<T, int> result_type;

		result_type get()
		{
			return result_type(INF, -1); 
		}
		result_type get(const iter& it)
		{
			return it->minp;
		}
		result_type merge(result_type left, result_type right)
		{
			return std::min(left, right);
		}
	};

public:
	void assign(int size, const T& val = 0)
	{
		_tree.assign(size);
		segment_tree::build(init_const_op(val), root());
	}
	void assign(const vector<T>& vals)
	{
		_tree.assign(vals.size());
		segment_tree::build(init_vec_op(vals), root());
	}

	pair<T, int> min(int l, int r) const
	{
		return segment_tree::calc(min_op(), root(), segment_tree::Query(l, r));
	}

	void add(int l, int r, T val)
	{
		return segment_tree::modify(add_op(val), root(), segment_tree::Query(l, r));
	}

};