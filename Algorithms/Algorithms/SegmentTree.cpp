#include "SegmentTree.h"
#include "_tests.h"


template<typename T, T INF = T(1e9)>
class AwesomeArray
{
	vector<T> _v;

public:
	void assign(int size) { _v.assign(size, T{}); }
	void assign(int size, const T& value) { _v.assign(size, value); }
	void assign(const vector<T>& v) { _v = v; }

	void add(int l, int r, T val)
	{
		for (int i = l; i <= r; ++i) _v[i] += val;
	}
	void set(int l, int r, T val)
	{
		for (int i = l; i <= r; ++i) _v[i] = val;
	}
	T sum(int l, int r)
	{
		T res = T{};
		for (int i = l; i <= r; ++i) res += _v[i];
		return res;
	}
	pair<T, int> min(int l, int r)
	{
		auto res = pair<T, int>(INF, -1);
		for (int i = l; i <= r; ++i) res = std::min(res, make_pair(_v[i], i));
		return res;
	}
};


template<typename TExpected, typename TActual>
class AwesomeArrayTest
{
	TExpected _exp;
	TActual _act;

public:
	void assign(int size)	
	{
		_exp.assign(size);
		_act.assign(size);
	}
	template<typename T>
	void assign(int size, const T& value) 
	{
		_exp.assign(size, value);
		_act.assign(size, value);
	}
	template<typename T>
	void assign(const vector<T>& v)
	{
		_exp.assign(v);
		_act.assign(v);
	}

	template<typename T>
	void add(int l, int r, T val)
	{
		_exp.add(l, r, val);
		_act.add(l, r, val);
	}

	template<typename T>
	void set(int l, int r, T val)
	{
		_exp.set(l, r, val);
		_act.set(l, r, val);
	}

	void sum(int l, int r)
	{
		auto exp = _exp.sum(l, r);
		auto act = _act.sum(l, r);
		test::assert_equal(exp, act);
	}
	void min(int l, int r)
	{
		auto exp = _exp.min(l, r);
		auto act = _act.min(l, r);
		test::assert_equal(exp, act);
	}
};


void TestSegmentTree1()
{
	AwesomeArrayTest<AwesomeArray<int>, AddSetSumST<int>> t;
	
	vint v{ 1, 2, 3 };
	t.assign(v);
	t.assign(101);

	t.add(1, 50, 100);
	t.set(10, 70, 50);

	t.sum(1, 100);
}

void TestSegmentTree2()
{
	static const int INF = 1e9;
	AwesomeArrayTest<AwesomeArray<int, INF>, AddMinST<int, INF>> t;

	vint v{ 1, 2, 3 };
	t.assign(v);
	t.assign(101);
	t.assign(101, 2);

	t.add(1, 50, 100);
	t.add(10, 70, 50);

	t.min(1, 100);
}

auto testsPub = tests::publish(
	TestSegmentTree1,
	TestSegmentTree2
);