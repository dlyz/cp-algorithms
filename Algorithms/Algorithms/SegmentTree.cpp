#include "SegmentTree.h"

void TestSegmentTree1()
{
	AddSetSumST<int> st;
	
	vint v{ 1, 2, 3 };
	st.assign(v);
	st.assign(100);

	st.add(1, 50, 100);
	st.set(10, 70, 50);

	st.sum(1, 100);


}


void TestSegmentTree2()
{
	AddMinST<int> st;

	vint v{ 1, 2, 3 };
	st.assign(v);
	st.assign(100);
	st.assign(100, 2);

	st.add(1, 50, 100);
	st.add(10, 70, 50);

	pii res = st.min(1, 100);


}