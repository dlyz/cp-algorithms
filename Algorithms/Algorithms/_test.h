#pragma once
#include <iostream>

namespace test
{
	template<typename T1, typename T2>
	std::ostream& operator <<(std::ostream& out, const std::pair<T1, T2>& value)
	{
		return out << "(" << value.first << ", " << value.second << ")";
	}

	inline void fail()
	{
		std::cout << std::endl;
		terminate();
	}

	template<typename TArg1, typename... TArgs>
	void fail(TArg1&& arg1, TArgs&&... args)
	{
		std::cout << std::forward<TArg1>(arg1);
		return fail(std::forward<TArgs>(args)...);
	}

	template<typename T>
	void assert_equal(const T& expected, const T& actual)
	{
		if (expected == actual) return;
		fail("assert_equal failed: ", expected, " != ", actual);
	}

	inline void assert_bool(bool value)
	{
		if (value) return;
		fail("assert failed");	
	}

}