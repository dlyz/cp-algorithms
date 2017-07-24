#pragma once
#include "header.h"


namespace segment_tree
{
	template<typename TValue>
	struct ConstArray
	{
		typedef TValue value_type;
		value_type val;
		ConstArray(const value_type& val) : val(val) {}
		const value_type& operator[](int) const { return val; }
	};

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
	class Tree;

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
	
	template<typename TOperation, typename TNode, typename TContainer>
	void build(TOperation& op, Iter<TNode> it, const TContainer& container)
	{
		if (it.is_leaf())
		{
			op.init(it, container[it.l]);
			return;
		}

		build(op, it.left(), container);
		build(op, it.right(), container);
		op.recalc(it);
	}

	template<typename TOperation, typename TNode>
	typename TOperation::result_type calc(TOperation& op, Iter<TNode> it, const Query& q)
	{
		if (!q.has_common(it)) return op.def();

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

	template<typename TNode>
	class Tree
	{
	public:
		typedef TNode node_type;
		typedef Iter<TNode> iterator;

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

		template<typename TOperation, typename TValue>
		void assign(TOperation& operation, int size, const TValue& val)
		{
			assign(size);
			build(operation, iterator(*this), ConstArray<TValue>(val));
		}

		template<typename TOperation, typename TValue>
		void assign(TOperation& operation, const vector<TValue>& vals)
		{
			assign(vals.size());
			build(operation, iterator(*this), vals);
		}

		template<typename TOperation>
		typename TOperation::result_type calc(TOperation& op, const Query& q) const
		{
			return segment_tree::calc(op, iterator(*this), q);
		}

		template<typename TOperation>
		void modify(TOperation& op, const Query& q)
		{
			return segment_tree::modify(op, iterator(*this), q);
		}
	};

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
	
	struct base_op
	{
		void init(const iter& it, const T& val)
		{
			it->sum = val;
		}

		void recalc(const iter& it)
		{
			it->sum = it.left()->sum + it.right()->sum;
		}

		void push(const iter& it)
		{
			Node& node = *it;
			if (node.add != 0)
			{
				node.sum += node.add * it.len();
				segment_tree::apply_to_children(add_op(node.add), it);
				node.add = 0;
			}
		}
	};

	struct add_op : public base_op
	{
		T val;
		add_op(const T& val) : val(val) {}

		void apply(const iter& it)
		{
			it->add += val;
		}
	};
	
	struct sum_op : public base_op
	{
		typedef T result_type;
		T merge(T left, T right)
		{
			return left + right;
		}
		T get(const iter& it)
		{
			return it->sum;
		}
		T def() const { return 0; }
	};

public:
	void assign(int size, const T& val = 0)
	{
		if (val == 0) _tree.assign(size);
		else _tree.assign(base_op(), size, val);
	}
	void assign(const vector<T>& vals)
	{
		_tree.assign(base_op(), vals);
	}

	T sum(int l, int r) const
	{
		return _tree.calc(sum_op(), segment_tree::Query(l, r));
	}

	void add(int l, int r, T val)
	{
		return _tree.modify(add_op(val), segment_tree::Query(l, r));
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

	
	struct base_op
	{
		void init(const iter& it, const T& val)
		{
			it->sum = val;
		}

		void recalc(const iter& it)
		{
			it->sum = it.left()->sum + it.right()->sum;
		}

		void push(const iter& it)
		{
			Node& node = *it;
			if (node.set != UNDEF)
			{
				node.sum = node.set * it.len();
				segment_tree::apply_to_children(set_op(node.set), it);
				node.set = UNDEF;
			}
			else if (node.add != 0)
			{
				node.sum += node.add * it.len();
				segment_tree::apply_to_children(add_op(node.add), it);
				node.add = 0;
			}
		}
	};
	
	struct add_op : public base_op
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

	struct set_op : public base_op
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

	struct sum_op : public base_op
	{
		typedef T result_type;
		T def() const { return 0; }
		T get(const iter& it)
		{
			return it->sum;
		}
		T merge(T left, T right)
		{
			return left + right;
		}
	};

public:
	void assign(int size, const T& val = 0)
	{
		if (val == 0) _tree.assign(size);
		else _tree.assign(base_op(), size, val);
	}
	void assign(const vector<T>& vals)
	{
		_tree.assign(base_op(), vals);
	}

	T sum(int l, int r) const
	{
		return _tree.calc(sum_op(), segment_tree::Query(l, r));
	}

	void add(int l, int r, T val)
	{
		return _tree.modify(add_op(val), segment_tree::Query(l, r));
	}

	void set(int l, int r, T val)
	{
		return _tree.modify(set_op(val), segment_tree::Query(l, r));
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

		void recalc(const Node& l, const Node& r)
		{
			minp = std::min(l.minp, r.minp);
		}

		void apply()
		{
			minp.first += add;
			add = 0;
		}
	};

	typedef segment_tree::Iter<Node> iter;
	segment_tree::Tree<Node> _tree;

	struct base_op
	{
		void init(const iter& it, const T& val)
		{
			it->init(val, it.leaf_index());
		}

		void recalc(const iter& it)
		{
			it->recalc(*it.left(), *it.right());
		}

		void push(const iter& it)
		{
			Node& node = *it;
			if (node.add != 0)
			{
				segment_tree::apply_to_children(add_op(node.add), it);
				node.apply();
			}
		}
	};

	struct add_op : public base_op
	{
		T val;
		add_op(const T& val) : val(val) {}

		void apply(const iter& it)
		{
			it->add += val;
		}
	};

	struct min_op : public base_op
	{
		typedef pair<T, int> result_type;
		result_type merge(result_type left, result_type right)
		{
			return std::min(left, right);
		}
		result_type get(const iter& it)
		{
			return it->minp;
		}
		result_type def() const { return result_type(INF, -1); }
	};

public:
	void assign(int size, const T& val = 0)
	{
		_tree.assign(base_op(), size, val);
	}
	void assign(const vector<T>& vals)
	{
		_tree.assign(base_op(), vals);
	}

	pair<T, int> min(int l, int r) const
	{
		return _tree.calc(min_op(), segment_tree::Query(l, r));
	}

	void add(int l, int r, T val)
	{
		return _tree.modify(add_op(val), segment_tree::Query(l, r));
	}

};