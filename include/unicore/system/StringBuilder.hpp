#pragma once
#include "unicore/system/StringHelper.hpp"
#include "unicore/system/Unicode.hpp"

namespace unicore
{
	class StringBuilder
	{
	public:
		String data;

		StringBuilder() noexcept = default;
		StringBuilder(const StringBuilder& other) noexcept = default;
		StringBuilder(StringBuilder&& other) noexcept;
		~StringBuilder() = default;

		void append(char c);
		void append(Char16 c);
		void append(Char32 c);

		void append(StringView text);
		void append(StringView16 text);
		void append(StringView32 text);

		StringBuilder& operator=(const StringBuilder& other) noexcept = default;
		StringBuilder& operator=(StringBuilder&& other) noexcept;

		template<typename ... Args>
		static String format(StringView format, const Args& ... args)
		{
			StringBuilder builder;
			internal_format(builder, format, args...);
			return builder.data;
		}

		template<typename ... Args>
		static StringW format(StringViewW format, const Args& ... args)
		{
			// TODO: Optimize (Rewrite without Unicode methods)
			StringBuilder builder;
			internal_format(builder, Unicode::to_utf8(format), args...);
			return Unicode::to_wcs(builder.data);
		}

		template<typename ... Args>
		static String16 format(StringView16 format, const Args& ... args)
		{
			// TODO: Optimize (Rewrite without Unicode methods)
			StringBuilder builder;
			internal_format(builder, Unicode::to_utf8(format), args...);
			return Unicode::to_utf16(builder.data);
		}

		template<typename ... Args>
		static String32 format(StringView32 format, const Args& ... args)
		{
			// TODO: Optimize (Rewrite without Unicode methods)
			StringBuilder builder;
			internal_format(builder, Unicode::to_utf8(format), args...);
			return Unicode::to_utf32(builder.data);
		}

		static String quoted(StringView text, Char delim = '\"')
		{
			StringBuilder builder;
			builder.append(delim);
			builder.append(text);
			builder.append(delim);
			return builder.data;
		}

		static String16 quoted(StringView16 text, Char16 delim = '\"')
		{
			// TODO: Optimize (Rewrite without Unicode methods)
			StringBuilder builder;
			builder.append(delim);
			builder.append(text);
			builder.append(delim);
			return Unicode::to_utf16(StringView(builder.data));
		}

		static String32 quoted(StringView32 text, Char32 delim = '\"')
		{
			// TODO: Optimize (Rewrite without Unicode methods)
			StringBuilder builder;
			builder.append(delim);
			builder.append(text);
			builder.append(delim);
			return Unicode::to_utf32(StringView(builder.data));
		}

	protected:
		static constexpr StringView Elem = "{}";

		static void internal_format(StringBuilder& builder, StringView value)
		{
			builder.append(value);
		}

		template<typename T>
		static void internal_format(StringBuilder& builder, const T& value)
		{
			builder << value;
		}

		template<typename T, typename ... Args>
		static void internal_format(StringBuilder& builder, StringView format, const T& value, const Args& ... args)
		{
			if (const auto pos = format.find_first_of(Elem); pos != StringView::npos)
			{
				builder.append(format.substr(0, pos));
				builder << value;

				internal_format(builder, format.substr(pos + Elem.size()), args...);
			}
			else builder.append(format);
		}
	};

	extern UNICODE_STRING_BUILDER_FORMAT(const StringBuilder&);

	extern UNICODE_STRING_BUILDER_FORMAT(bool);

#if defined (_HAS_EXCEPTIONS)
	extern UNICODE_STRING_BUILDER_FORMAT(const std::exception&);
#endif

	extern UNICODE_STRING_BUILDER_FORMAT(char);
	extern UNICODE_STRING_BUILDER_FORMAT(Char16);
	extern UNICODE_STRING_BUILDER_FORMAT(Char32);

	extern UNICODE_STRING_BUILDER_FORMAT(const char*);
	extern UNICODE_STRING_BUILDER_FORMAT(const Char16*);
	extern UNICODE_STRING_BUILDER_FORMAT(const Char32*);

	extern UNICODE_STRING_BUILDER_FORMAT(float);
	extern UNICODE_STRING_BUILDER_FORMAT(double);

	extern UNICODE_STRING_BUILDER_FORMAT(const std::type_info&);
	extern UNICODE_STRING_BUILDER_FORMAT(const std::type_index&);

	template<typename TChar,
		std::enable_if_t<sfinae::is_char_v<TChar>>* = nullptr>
	extern StringBuilder& operator << (StringBuilder& builder, const BasicStringView<TChar> value)
	{
		builder.append(value);
		return builder;
	}

	template<typename TChar,
		std::enable_if_t<sfinae::is_char_v<TChar>>* = nullptr>
	extern StringBuilder& operator << (StringBuilder& builder, const BasicString<TChar>& value)
	{
		builder.append(value);
		return builder;
	}

	template<typename T, std::enable_if_t<std::is_enum_v<T>>* = nullptr>
	extern StringBuilder& operator<<(StringBuilder& builder, T value)
	{
		const auto tmp = static_cast<int>(value);
		const auto str = std::to_string(tmp);
		return builder << str.c_str();
	}

	template<typename T, std::enable_if_t<std::is_pointer_v<T>>* = nullptr>
	extern StringBuilder& operator<<(StringBuilder& builder, T value)
	{
		return builder << "0x" << StringHelper::to_hex(reinterpret_cast<intptr_t>(value));
	}

	template<typename T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
	extern StringBuilder& operator<<(StringBuilder& builder, T value)
	{
		const auto str = std::to_string(value);
		return builder << str.c_str();
	}

	template<typename T>
	extern StringBuilder& operator<<(StringBuilder& builder, std::initializer_list<T> list)
	{
		int index = 0;
		for (const auto& value : list)
		{
			if (index > 0)
				builder << ',';
			builder << value;
			index++;
		}
		return builder;
	}

	template<typename T>
	extern StringBuilder& operator<<(StringBuilder& builder, const List<T> list)
	{
		int index = 0;
		for (const auto& value : list)
		{
			if (index > 0)
				builder << ',';
			builder << value;
			index++;
		}
		return builder;
	}
}