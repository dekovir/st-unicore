#include "unicore/Logger.hpp"
#include "Windows/WinLogger.hpp"
#include <stdio.h>

namespace unicore
{
	void PrintLogger::write(LogType type, const StringView text)
	{
		printf("%s %s\n", type_to_str(type), text.data());
	}

	void MultiLogger::write(LogType type, const StringView text)
	{
		for (const auto& logger : _loggers)
			logger->write(type, text);
	}

	Shared<Logger> Logger::create()
	{
#if defined(UNICORE_PLATFORM_WINDOWS)
		return make_shared<WinLogger>();
#else
		return make_shared<PrintLogger>();
#endif
	}

	const char* Logger::type_to_str(LogType type)
	{
		switch (type)
		{
		case LogType::Info: return "i";
		case LogType::Debug: return "d";
		case LogType::Warning: return "w";
		case LogType::Error: return "e";
		}
	}
}
