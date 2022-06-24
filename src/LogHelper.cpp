#include "unicore/LogHelper.hpp"
#include "unicore/Strings.hpp"

namespace unicore
{
	LogHelper::LogHelper(Logger& logger, LogType type)
		: _logger(logger), _type(type)
	{
	}

	LogHelper::~LogHelper()
	{
		_logger.write(_type, _text);
	}

	LogHelper& LogHelper::operator<<(bool value)
	{
		append(value ? L"true" : L"false");
		return *this;
	}

#if defined (_HAS_EXCEPTIONS)
	LogHelper& LogHelper::operator<<(const std::exception& ex)
	{
		append(ex.what());
		return *this;
	}
#endif

	LogHelper& LogHelper::operator<<(const DebugSource& source)
	{
		// TODO: Replace with constexpr
#if defined (UNICORE_PLATFORM_WINDOWS) || defined(UNICORE_PLATFORM_ANDROID)
		const auto* pos = strrchr(source.file.data(), '\\');
#else
		const auto* pos = strrchr(source.file.data(), '/');
#endif
		if (pos != nullptr)
			append(pos + 1);
		else append(source.file);
		append(L":");
		this->operator<<(source.line);

		return *this;
	}

	LogHelper& LogHelper::operator<<(const DebugFunction& source)
	{
		this->operator<<(source._func);
		this->operator<<(L" - ");
		return *this;
	}

	void LogHelper::append(const char c)
	{
		const char str[] = { c, 0 };
		append(str);
	}

	void LogHelper::append(const wchar_t c)
	{
		const wchar_t str[] = { c, 0 };
		append(str);
	}

	void LogHelper::append(const StringView text)
	{
		_text.append(text.data());
	}

	void LogHelper::append(const WStringView text)
	{
		_text.append(Strings::to_utf8(text.data()));
	}

	LogHelper& operator<<(LogHelper& helper, const TypeInfo& type)
	{
		return helper << type.name();
	}

	LogHelper& operator<<(LogHelper& helper, const TypeIndex& type)
	{
		return helper << type.name();
	}
}