#include "unicore/system/DateTime.hpp"
#include "unicore/io/Logger.hpp"

namespace unicore
{
	DateTime::DateTime(time_t data) noexcept
		: _data(ClockType::from_time_t(data))
	{}

	time_t DateTime::to_time_t() const noexcept
	{
		return ClockType::to_time_t(_data);
	}

	DateTime DateTime::now() noexcept
	{
		return DateTime(ClockType::now());
	}

	UNICODE_STRING_BUILDER_FORMAT(const DateTime&)
	{
		const auto time = value.to_time_t();
		return builder << ctime(&time);
	}
}