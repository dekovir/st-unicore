#pragma once
#include "unicore/io/Logger.hpp"
#include "unicore/io/File.hpp"

namespace unicore
{
	class WriteFile;

	class FileLogger : public Logger
	{
		UC_OBJECT(FileLogger, Logger)
	public:
		explicit FileLogger(const Shared<WriteFile>& file);

		void write(LogType type, const StringView text) override;

	protected:
		Shared<WriteFile> _file;
		FileWriter _writer;
	};
}