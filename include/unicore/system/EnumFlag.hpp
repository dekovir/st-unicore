#pragma once
#include "unicore/system/StringBuilder.hpp"

namespace unicore
{
	template<typename T>
	class EnumFlag
	{
		static_assert(std::is_enum_v<T>, "Must be enum");
	public:
		typedef std::underlying_type_t<T> TValue;

		constexpr EnumFlag() : _value(0) {}
		constexpr EnumFlag(T value) : _value(static_cast<TValue>(value)) {}
		constexpr explicit EnumFlag(TValue value) : _value(value) {}

		constexpr EnumFlag(const EnumFlag& other) = default;
		constexpr EnumFlag(EnumFlag&& other) noexcept = default;

		~EnumFlag() = default;

		EnumFlag& operator=(const EnumFlag& other) = default;
		EnumFlag& operator=(EnumFlag&& other) noexcept = default;

		UC_NODISCARD constexpr TValue value() const { return _value; }

		UC_NODISCARD constexpr bool empty() const { return _value == 0; }

		UC_NODISCARD constexpr bool has(T flag) const
		{
			return (_value & static_cast<TValue>(flag)) == static_cast<TValue>(flag);
		}

		void reset()
		{
			_value = 0;
		}

		template<T Flag>
		UC_NODISCARD constexpr bool has() const
		{
			return (_value & static_cast<TValue>(Flag)) == static_cast<TValue>(Flag);
		}

		template<T Flag>
		UC_NODISCARD constexpr bool any() const
		{
			return (_value & static_cast<TValue>(Flag)) == static_cast<TValue>(Flag);
		}

		template<T FlagA, T FlagB, T... Rest>
		UC_NODISCARD constexpr bool any() const
		{
			return any<FlagA>() || any<FlagB, Rest...>();
		}

		template<T Flag>
		UC_NODISCARD constexpr bool all() const
		{
			return (_value & static_cast<TValue>(Flag)) == static_cast<TValue>(Flag);
		}

		template<T FlagA, T FlagB, T... Rest>
		UC_NODISCARD constexpr bool all() const
		{
			return all<FlagA>() && all<FlagB, Rest...>();
		}

		void set(T flag)
		{
			_value |= static_cast<TValue>(flag);
		}

		void remove(T flag)
		{
			_value &= ~static_cast<TValue>(flag);
		}

		void set(T flag, bool value)
		{
			if (value)
				set(flag);
			else remove(flag);
		}

		void toggle(T flag)
		{
			if (flag(flag))
				remove(flag);
			else set(flag);
		}

		UC_NODISCARD constexpr bool operator[](T flag) const { return has(flag); }

		constexpr EnumFlag<T> operator|(T flag) const
		{
			EnumFlag<T> flags(_value);
			flags.set(flag);
			return flags;
		}

		EnumFlag<T>& operator|=(T flag)
		{
			set(flag);
			return *this;
		}

		EnumFlag<T>& operator=(T flag)
		{
			_value = static_cast<TValue>(flag);
			return *this;
		}

		EnumFlag<T>& operator=(TValue value)
		{
			_value = value;
			return *this;
		}

		static constexpr bool is_changed(const EnumFlag<T>& a, const EnumFlag<T>& b, T flag)
		{
			return a.has(flag) != b.has(flag);
		}

		template<T Flag>
		static constexpr TValue merge()
		{
			return static_cast<TValue>(Flag);
		}

		template<T FlagA, T FlagB, T... Rest>
		static constexpr TValue merge()
		{
			return merge<FlagA>() | merge<FlagB, Rest...>();
		}

		static const EnumFlag<T> Zero;

	protected:
		TValue _value;
	};

	template<typename T>
	static constexpr bool operator == (const EnumFlag<T> a, const EnumFlag<T> b)
	{
		return a.value() == b.value();
	}

	template<typename T>
	static constexpr bool operator != (const EnumFlag<T> a, const EnumFlag<T> b)
	{
		return a.value() != b.value();
	}

	template<typename T>
	static constexpr bool operator == (const EnumFlag<T> a, const typename EnumFlag<T>::TValue value)
	{
		return a.value() == value;
	}

	template<typename T>
	static constexpr bool operator != (const EnumFlag<T> a, const typename EnumFlag<T>::TValue value)
	{
		return a.value() != value;
	}

	//template<typename T>
	//static constexpr EnumFlag<T> operator |(const T a, const T b)
	//{
	//	return EnumFlag<T>(
	//		static_cast<typename EnumFlag<T>::TValue>(a) |
	//		static_cast<typename EnumFlag<T>::TValue>(b)
	//		);
	//}

	template<typename T>
	static constexpr EnumFlag<T> operator |(const EnumFlag<T> flags, const T value)
	{
		return EnumFlag<T>(
			flags.value() |
			static_cast<typename EnumFlag<T>::TValue>(value)
			);
	}

	template<typename T>
	static constexpr EnumFlag<T> operator |(const T value, const EnumFlag<T> flags)
	{
		return EnumFlag<T>(
			flags.value() |
			static_cast<typename EnumFlag<T>::TValue>(value)
			);
	}

	template<typename T>
	const EnumFlag<T> EnumFlag<T>::Zero(0);

	//#define UNICORE_ENUM_MASK(TEnum) \
	//	static constexpr TEnum operator|(TEnum a, TEnum b) { \
	//		using Type = std::underlying_type_t<TEnum>; \
	//		return  TEnum(Type(a) | Type(b)); } \
	//	static constexpr bool operator==(TEnum color, std::underlying_type_t<TEnum> value) { \
	//		return static_cast<std::underlying_type_t<TEnum>>(color) == value; }

#define UNICORE_ENUM_FLAGS(TFlag, TName) using TName = EnumFlag<TFlag>

	template<typename T>
	extern UNICODE_STRING_BUILDER_FORMAT(const EnumFlag<T>)
	{
		return builder << "Flag(0x" << StringHelper::to_hex(value.value()) << ")";
	}
}