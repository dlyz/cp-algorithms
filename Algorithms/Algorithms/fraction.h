#pragma once
#include "number.h"

template<typename T>
struct Fraction
{
	Fraction() {}
	Fraction(T value) : _num(value), _den(1) {}
	Fraction(T num, T den) : _num(num), _den(den)
	{
		if (num == 0) _den = 1;
		else if (_den < 0) _den *= -1, _num *= -1;
	}

	static Fraction<T> MakeRaw(T num, T den)
	{
		Fraction<T> res; res._num = num, res._den = den;
		return res;
	}

	T Num() const { return _num; }
	T Den() const { return _den; }
	float64 Value() const { return (float64)_num / _den; }

	Fraction<T> NormGcd() const
	{
		if (_num == 0 && _den == 0) return Fraction<T>(0, 0);
		T g = gcd(std::abs(_num), std::abs(_den));
		return MakeRaw(x / g, y / g);
	}

	operator float64() const { return Value(); }
	Fraction<T> operator-() const { return MakeRaw(-_num, _den); }

private:
	T _num;
	T _den;
};


template<typename T>
Fraction<T> operator +(const Fraction<T>& lhs, const Fraction<T>& rhs)
{
	T g = gcd(lhs.Den(), rhs.Den());
	return Fraction<T>(lhs.Num()*(rhs.Den() / g) + rhs.Num()*(lhs.Den() / g), (lhs.Den() / g)*rhs.Den());
}
template<typename T>
Fraction<T> operator -(const Fraction<T>& lhs, const Fraction<T>& rhs) { return lhs + (-rhs); }
template<typename T>
Fraction<T> operator *(const Fraction<T>& lhs, const Fraction<T>& rhs) { return Fraction<T>(lhs.Num() * rhs.Num(), lhs.Den() * rhs.Den()); }
template<typename T>
Fraction<T> operator /(const Fraction<T>& lhs, const Fraction<T>& rhs) { return Fraction<T>(lhs.Num() * rhs.Den(), lhs.Den() * rhs.Num()); }
