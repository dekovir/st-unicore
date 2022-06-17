#pragma once
#include "unicore/Defs.hpp"

namespace unicore
{
	template<typename T>
	struct Vector2
	{
		T x;
		T y;

		constexpr Vector2() = default;
		constexpr Vector2(T x, T y);

		UC_NODISCARD constexpr T area() const { return x * y; }

		template<typename U>
		constexpr Vector2<U> cast() const
		{
			return Vector2<U>(static_cast<U>(x), static_cast<U>(y));
		}

		static const Vector2<T> Zero;
		static const Vector2<T> One;
	};

	typedef Vector2<float> Vector2f;
	typedef Vector2<int>   Vector2i;

	// IMPLEMENTATION //////////////////////////////////////////////////////////
	template <typename T>
	constexpr Vector2<T>::Vector2(T _x, T _y)
		: x(_x), y(_y)
	{
	}

	template<typename T>
	const Vector2<T> Vector2<T>::Zero{ 0, 0 };

	template<typename T>
	const Vector2<T> Vector2<T>::One{ 1, 1 };

	// OPERATORS ///////////////////////////////////////////////////////////////
	template<typename T>
	static std::enable_if_t<std::is_integral_v<T>, bool>
		operator == (const Vector2<T>& a, const Vector2<T>& b)
	{
		return a.x == b.x && a.y == b.y;
	}

	template<typename T>
	static std::enable_if_t<std::is_integral_v<T>, bool>
		operator != (const Vector2<T>& a, const Vector2<T>& b)
	{
		return a.x != b.x || a.y != b.y;
	}

	template<typename T>
	static Vector2<T> operator + (const Vector2<T>& a, const Vector2<T>& b)
	{
		return Vector2<T>(a.x + b.x, a.y + b.y);
	}

	template<typename T>
	static Vector2<T> operator - (const Vector2<T>& a, const Vector2<T>& b)
	{
		return Vector2<T>(a.x - b.x, a.y - b.y);
	}

	template<typename T>
	static Vector2<T> operator * (const Vector2<T>& a, const Vector2<T>& b)
	{
		return Vector2<T>(a.x * b.x, a.y * b.y);
	}

	template<typename T>
	static std::enable_if_t<std::is_floating_point_v<T>, Vector2<T>>
		operator / (const Vector2<T>& a, const Vector2<T>& b)
	{
		return Vector2<T>(a.x / b.x, a.y / b.y);
	}

	template<typename T>
	static Vector2<T> operator * (const Vector2<T>& a, T value)
	{
		return Vector2<T>(a.x * value, a.y * value);
	}

	template<typename T>
	static std::enable_if_t<std::is_floating_point_v<T>, Vector2<T>>
		operator / (const Vector2<T>& a, T value)
	{
		return Vector2<T>(a.x / value, a.y / value);
	}
}