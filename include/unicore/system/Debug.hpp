#pragma once
#include "unicore/Defs.hpp"

namespace unicore
{
	template<typename T>
	struct Error
	{
		constexpr explicit Error(T code)
			: _code(code) {}

		UC_NODISCARD constexpr T code() const { return _code; }

	protected:
		const T _code;
	};

	struct DebugSource
	{
		constexpr DebugSource(const StringView file_, const int line_)
			: file(file_), line(line_)
		{
		}

		const StringView file;
		const int line;
	};

	struct DebugFunction
	{
		explicit constexpr DebugFunction(const StringView name_)
			: name(name_) {}

		StringView name;
	};

	extern UNICODE_STRING_BUILDER_FORMAT(const DebugSource&);
	extern UNICODE_STRING_BUILDER_FORMAT(const DebugFunction&);

#define UC_DEBUG_SOURCE() DebugSource(__FILE__, __LINE__)
#define UC_DEBUG_FUNCTION() DebugFunction(__FUNCTION__)
}