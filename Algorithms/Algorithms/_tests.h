#pragma once
#include "_test.h"
#include <vector>
#include <functional>

namespace tests
{
	using test_type = std::function<void()>;
	using collection_type = std::vector<test_type>;
	
	extern collection_type _collection;
	
	struct token {};

	template<typename... TTests>
	token publish(TTests&&... tests)
	{
		std::make_tuple(add(std::forward<TTests>(tests))...);
		return token{};
	}
	
	inline token add(const test_type& test)
	{
		_collection.push_back(test);
		return token{};
	}

	inline void run()
	{
		for (const auto& test : _collection)
		{
			test();
		}
	}

}