#pragma once
#include "header.h"

template<typename T> T gcd(T a, T b)
{
	return b ? gcd(b, a%b) : a;
}

template<typename T> T lcm(T a, T b)
{
	return a / gcd(a, b)*b;
}

template<typename TInner>
struct InverseComparer
{
	TInner _cmp;
	InverseComparer(const TInner& inner) : _cmp(inner) {}

	template<typename T1, typename T2>
	bool operator()(const T1& lhs, const T2& rhs) const { return !_cmp(lhs, rhs); }
};

template<typename TInner>
InverseComparer<TInner> InverseCompare(const TInner& innerComparer) { return InverseComparer<TInner>(innerComparer); }

namespace geom
{
	// CW = RIGHT, CCW = LEFT

	const float64 PI = 3.1415926535897932384626433832795;

#pragma region Points - Vectors

	template<typename T>
	struct PointBase
	{
		T x, y;
		PointBase() {}
		PointBase(T x, T y) : x(x), y(y) {}

		template<typename U>
		explicit PointBase(const PointBase<U>& other)
			: x((T)other.x), y((T)other.y)
		{}

		bool Equals(const PointBase& p) const { return x == p.x && y == p.y; }
		bool NotEquals(const PointBase& p) const { return !Equals(p); }

		struct LessXY
		{
			bool operator ()(const PointBase& p1, const PointBase& p2) const
			{
				return p1.x < p2.x || p1.x == p2.x && p1.y < p2.y;
			}
		};

		friend istream& operator >> (istream& is, PointBase& p)
		{
			return is >> p.x >> p.y;
		}

		friend ostream& operator << (ostream& os, const PointBase& p)
		{
			return os << p.x << ' ' << p.y;
		}

		void PrintDouble() const { printf("%.12f %.12f", (double)x, (double)y); }
	};


	template<typename T>
	struct Vector : public PointBase<T>
	{
		typedef PointBase<T> Base;
		using Base::x;
		using Base::y;

		Vector() {}
		Vector(T x, T y) : Base(x, y) {}
		static Vector Zero() { return Vector(0, 0); }

		template<typename U>
		explicit Vector(const PointBase<U>& other)
			: Base(other)
		{}

		Vector operator -() const { return Vector(-x, -y); }

		float64 Len() const { return sqrt(Len2F()); }
		float64 Len2F() const { return (float64)x*x + (float64)y*y; }
		T Len2() const { return x*x + y*y; }

		Vector<float64> Norm() const
		{
			return Vector<float64>(*this) / Len();
		}
		Vector<float64> Norm(float64 newLen) const
		{
			return Vector<float64>(*this) * (newLen / Len());
		}
		Vector NormGCD() const
		{
			if (*this == Zero()) return Zero();
			T g = gcd(abs(x), abs(y));
			return Vector(x / g, y / g);
		}

		Vector<float64> Project(const Vector& what) const
		{
			return Vector<float64>(*this) * ProjectK(what);
		}
		float64 ProjectK(const Vector& what) const
		{
			return (*this * what) / Len2F();
		}
		pair<T, T> ProjectKFrac(const Vector& what)
		{
			return pair<T, T>(*this * what, Len2());
		}

		// [-PI; PI]
		float64 Angle() const
		{
			return atan2((float64)y, (float64)x);
		}

		Vector<float64> TurnCw(float64 sina, float64 cosa) const
		{
			return Vector<float64>(x*cosa + y*sina, -x*sina + y*cosa);
		}
		Vector<float64> TurnCcw(float64 sina, float64 cosa) const
		{
			return Vector<float64>(x*cosa - y*sina, x*sina + y*cosa);
		}

		Vector<float64> TurnCw(float64 angle) const { return TurnCw(sin(angle), cos(angle)); }
		Vector<float64> TurnCcw(float64 angle) const { return TurnCcw(sin(angle), cos(angle)); }
		Vector<float64> Turn(float64 angle) const { return TurnCcw(sin(angle), cos(angle)); }

		Vector TurnCw90() const { return Vector(y, -x); }
		Vector TurnCcw90() const { return Vector(-y, x); }
		Vector Turn90() const { return TurnCcw90(); }
		Vector Turn180() const { return -*this; }

		// to the right from this
		bool IsCwTurn(const Vector& to) const { return (*this ^ to) < 0; }

		// to the left from this
		bool IsCcwTurn(const Vector& to) const { return (*this ^ to) > 0; }

		struct IsCw
		{
			bool operator()(const Vector &from, const Vector &to) const { return from.IsCwTurn(to); }
		};

		struct IsCcw
		{
			bool operator()(const Vector &from, const Vector &to) const { return from.IsCcwTurn(to); }
		};

		struct LessAngle
		{
			int Side(const Vector<T> &v) const
			{
				return v.y < 0 || v.y == 0 && v.x < 0;
			}

			bool operator ()(const Vector<T> &v1, const Vector<T> &v2) const
			{
				int s1 = Side(v1);
				int s2 = Side(v2);
				if (s1 != s2) return s1 < s2;
				return v1.IsCcwTurn(v2);
			}
		};

		struct LessPolar
		{
			bool operator ()(const Vector<T> &v1, const Vector<T> &v2) const
			{
				int s1 = LessAngle().Side(v1);
				int s2 = LessAngle().Side(v2);
				if (s1 != s2) return s1 < s2;
				if (v1.IsCcwTurn(v2)) return true;
				else if (v1.IsCwTurn(v2)) return false;
				else return v1.Len2() < v2.Len2();
			}
		};


		friend bool operator ==(const Vector& p1, const Vector& p2) { return p1.Equals(p2); }
		friend bool operator !=(const Vector& p1, const Vector& p2) { return p1.NotEquals(p2); }

		friend Vector operator -(const Vector &p1, const Vector &p2) { return Vector(p1.x - p2.x, p1.y - p2.y); }
		friend Vector operator +(const Vector &p1, const Vector &p2) { return Vector(p1.x + p2.x, p1.y + p2.y); }

		friend T operator ^(const Vector &p1, const Vector &p2) { return p1.x*p2.y - p2.x*p1.y; }
		friend T operator *(const Vector &p1, const Vector &p2) { return p1.x*p2.x + p1.y*p2.y; }

		friend Vector operator *(const T& k, const Vector &p) { return Vector(k*p.x, k*p.y); }
		friend Vector operator *(const Vector &p, const T& k) { return Vector(k*p.x, k*p.y); }
		friend Vector operator /(const Vector &p, const T& k) { return Vector(p.x / k, p.y / k); }

	};

	template<typename T>
	struct Point : public PointBase<T>
	{
		typedef geom::Vector<T> Vector;
		typedef PointBase<T> Base;
		using Base::x;
		using Base::y;

		Point() {}
		Point(T x, T y) : Base(x, y) {}
		static Point Zero() { return Point(0, 0); }

		template<typename U>
		explicit Point(const PointBase<U>& other)
			: Base(other)
		{}

		template<typename TVecCmp>
		struct VectorCmpAdapter
		{
			Point origin;
			VectorCmpAdapter(const Point& origin) : origin(origin)
			{}

			bool operator ()(const Point &p1, const Point &p2) const
			{
				return TVecCmp()(p1 - origin, p2 - origin);
			}
		};

		typedef VectorCmpAdapter<typename Vector::LessAngle> LessAngle;
		typedef VectorCmpAdapter<typename Vector::LessPolar> LessPolar;
		typedef VectorCmpAdapter<typename Vector::IsCw> IsCw;
		typedef VectorCmpAdapter<typename Vector::IsCcw> IsCcw;

		friend bool operator ==(const Point& p1, const Point& p2) { return p1.Equals(p2); }
		friend bool operator !=(const Point& p1, const Point& p2) { return p1.NotEquals(p2); }

		friend Vector operator -(const Point &p1, const Point &p2) { return Vector(p1.x - p2.x, p1.y - p2.y); }
		friend Point operator +(const Point &p1, const Vector &p2) { return Point(p1.x + p2.x, p1.y + p2.y); }
		friend Point operator -(const Point &p1, const Vector &p2) { return Point(p1.x - p2.x, p1.y - p2.y); }
	};


#pragma endregion

#pragma region square

	template<typename T>
	T GetSquareDir2(const vector< Point<T> >& pts)
	{
		T result = 0;
		forn(i, pts.size())
		{
			const Point<T>& p1 = pts[i];
			const Point<T>& p2 = pts[(i + 1) % pts.size()];
			result += (p1.y + p2.y) * (p2.x - p1.x);
		}
		return result;
	}

	template<typename T>
	T GetSquare2(const vector< Point<T> >& pts)
	{
		return std::abs(GetSquareDir2(pts));
	}

	template<typename T>
	float64 GetSquare(const vector< Point<T> >& pts)
	{
		return GetSquare2(pts) / 2.0;
	}

#pragma endregion

#pragma region structs

	template<typename T>
	struct Circle
	{
		Point<T> c;
		T r;
		Circle() {}
		template<typename U>
		explicit Circle(const Circle<U> &other) : c(other.c), r(other.r) { }
		Circle(const Point<T>& c, T r) : c(c), r(r) {}

		bool IsIn(const Point<T>& p) const { return (p - c).Len2() < r*r; }
		bool IsInOrTouch(const Point<T>& p) const { return (p - c).Len2() <= r*r; }
	};

	template<typename T>
	struct Ray
	{
		Point<T> p;
		Vector<T> v;

		Ray() {}
		template<typename U>
		explicit Ray(const Ray<U> &r) : p(r.p), v(r.v) { }
		Ray(const Point<T> &p, const Vector<T> &v) : p(p), v(v) { }

		Point<T> MovePoint(T count = 1) const { return p + v * count; }
		Point<float64> Project(const Vector<T>& what) const { return Point<float64>(p) + v.Project(what); }
		Point<float64> Project(const Point<T>& what) const { return Point<float64>(p) + v.Project(what - p); }

		bool IsCw(const Point<T>& np) const { return v.IsCwTurn(np - p); }
		bool IsCcw(const Point<T>& np) const { return v.IsCcwTurn(np - p); }
	};


#pragma endregion

#pragma region rays intersection

	//      Knum=0        Knum=Kden
	//      |             |
	// -----*------------->-----------
	// max value = 4 * MAX_X^4.
	// int64 for ~ 0 <= X <= 20000
	template<typename T>
	class RaysI10nSlim
	{
	public:
		typedef Ray<T> RayType;

		RaysI10nSlim(const RayType& r1, const RayType& r2)
		{
			T v11 = r1.v * r1.v;
			T v12 = r1.v * r2.v;
			T v22 = r2.v * r2.v;
			_den = v22*v11 - v12*v12;
			T mul = _den < 0 ? -1 : 1;
			_den *= mul;

			T pv1 = (r1.p - r2.p) * r1.v;
			T pv2 = (r1.p - r2.p) * r2.v;

			_num1 = (-pv1*v22 + pv2*v12)*mul;
			_num2 = (pv2*v11 - pv1*v12)*mul;
		}

		T K1Num() const { return _num1; }
		T K2Num() const { return _num2; }
		T KDen() const { return _den; }

		float64 K1() const { return float64(_num1) / _den; }
		float64 K2() const { return float64(_num2) / _den; }

		template<typename U = T>
		enable_if_t<is_integral<U>::value, bool> Has() const { return _den != 0; }

		template<typename U = T>
		enable_if_t<is_floating_point<U>::value, bool> Has(float64 eps) const { return _den > eps; }

	private:
		T _num1;
		T _num2;
		T _den;
	};

	template<typename T>
	class RaysI10n : public RaysI10nSlim<T>
	{
	public:
		typedef RaysI10nSlim<T> Base;
		typedef typename Base::RayType RayType;
		using Base::K1;
		using Base::K2;

		RaysI10n(const RayType& r1, const RayType& r2) :
			Base(r1, r2), _r1(r1), _r2(r2)
		{ }

		const RayType& R1() const { return _r1; }
		const RayType& R2() const { return _r2; }

		Point<float64> Point1() const { return Ray<float64>(_r1).MovePoint(K1()); }
		Point<float64> Point2() const { return Ray<float64>(_r2).MovePoint(K2()); }

	private:
		RayType _r1;
		RayType _r2;
	};

	template<typename T>
	RaysI10nSlim<T> IntersectSlim(const Ray<T>& r1, const Ray<T>& r2) { return RaysI10nSlim<T>(r1, r2); }
	template<typename T>
	RaysI10n<T> Intersect(const Ray<T>& r1, const Ray<T>& r2) { return RaysI10n<T>(r1, r2); }

#pragma endregion

#pragma region ray-circle intersection

	// int64 for ~ 0 <= X <= 20000
	template<typename T>
	class RayCircleI10nSlim
	{
	public:
		typedef Ray<T> RayType;
		typedef Circle<T> CircleType;

		RayCircleI10nSlim(const RayType& r, const CircleType &c)
		{
			Vector<T> cp = r.p - c.c;
			T A = r.v*r.v;
			T B = r.v*cp;
			T C = cp*cp - sqr(c.r);
			_d = B*B - A*C;

			_k0 = -float64(B) / A;
			_d12 = sqrt(float64(max<T>(0, _d))) / A;
		}

		float64 Delta() const { return _d12; }
		float64 K0() const { return _k0; }
		float64 K1() const { return _k0 - _d12; }
		float64 K2() const { return _k0 + _d12; }

		template<typename U = T>
		enable_if_t<is_integral<U>::value, int> PointsCnt() const { return _d < 0 ? 0 : _d == 0 ? 1 : 2; }

		template<typename U = T>
		enable_if_t<is_floating_point<U>::value, int> PointsCnt(float64 eps) const { return _d < -eps ? 0 : _d < eps ? 1 : 2; }

	private:
		T _d;
		float64 _k0;
		float64 _d12;
	};

	template<typename T>
	class RayCircleI10n : public RayCircleI10nSlim<T>
	{
	public:
		typedef RayCircleI10nSlim<T> Base;
		typedef typename Base::RayType RayType;
		typedef typename Base::CircleType CircleType;
		using Base::K0;
		using Base::K1;
		using Base::K2;

		RayCircleI10n(const RayType& r, const CircleType &c) :
			Base(r, c), _r(r), _c(c)
		{}

		const RayType& R() const { return _r; }
		const CircleType& C() const { return _c; }

		Point<float64> Point1() const { return Ray<float64>(_r).MovePoint(K1()); }
		Point<float64> Point2() const { return Ray<float64>(_r).MovePoint(K2()); }
		Point<float64> Point0() const { return Ray<float64>(_r).MovePoint(K0()); }

	private:
		RayType _r;
		CircleType _c;
	};


	template<typename T>
	RayCircleI10nSlim<T> IntersectSlim(const Ray<T>& r, const Circle<T>& c) { return RayCircleI10nSlim<T>(r, c); }
	template<typename T>
	RayCircleI10n<T> Intersect(const Ray<T>& r, const Circle<T>& c) { return RayCircleI10n<T>(r, c); }
	template<typename T>
	RayCircleI10nSlim<T> IntersectSlim(const Circle<T>& c, const Ray<T>& r) { return RayCircleI10nSlim<T>(r, c); }
	template<typename T>
	RayCircleI10n<T> Intersect(const Circle<T>& c, const Ray<T>& r) { return RayCircleI10n<T>(r, c); }

#pragma endregion

#pragma region convex

	template<typename T, typename TDir = typename Vector<T>::IsCw>
	void AddToConvex(const Point<T>& p, vector<Point<T>>& convex, TDir direction = TDir())
	{
		while (convex.size() > 1)
		{
			Point<T> origin = convex[convex.size() - 2];
			if (direction(convex.back() - origin, p - origin)) break;
			convex.pop_back();
		}
		convex.push_back(p);
	}

	template<typename T, typename TDir = typename Vector<T>::IsCw>
	vector<Point<T>> ConvexHullSorted(const vector<Point<T>>& pts, TDir direction = TDir())
	{
		vector<Point<T>> up, down;
		forn(i, pts.size())	AddToConvex(pts[i], up, direction);
		forr(i, pts.size())	AddToConvex(pts[i], down, direction);
		up.reserve(up.size() + down.size());
		copy(down.begin() + 1, down.end() - 1, back_inserter(up));
		return up;
	}

#pragma endregion

	template<typename T>
	struct Types
	{
		typedef Point<T> p;
		typedef Vector<T> v;
		typedef Ray<T> ray;
		typedef Circle<T> circle;
		typedef RaysI10nSlim<T> ri_s;
		typedef RaysI10n<T> ri;
		typedef RayCircleI10nSlim<T> rci_s;
		typedef RayCircleI10n<T> rci;
	};
}


typedef geom::Types<int> gmi;
typedef geom::Types<int64> gml;
typedef geom::Types<float64> gmf;