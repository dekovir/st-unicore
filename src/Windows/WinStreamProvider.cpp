#include "WinStreamProvider.hpp"
#if defined(UNICORE_PLATFORM_WINDOWS)
#include <Shlwapi.h>
#include "WinError.hpp"
#include "WinStream.hpp"

namespace unicore
{
	WinStreamProvider::WinStreamProvider(Logger& logger)
		: _logger(logger)
	{
	}

	bool WinStreamProvider::exists(const Path& path) const
	{
		const auto native_path = path.native_path();
		const auto flags = GetFileAttributesW(native_path.c_str());
		return flags != INVALID_FILE_ATTRIBUTES ? true : false;
	}

	Optional<StreamStats> WinStreamProvider::stats(const Path& path) const
	{
		const auto native_path = path.native_path();

		// TODO: Test for directory
		WIN32_FILE_ATTRIBUTE_DATA fileAttr;
		if (GetFileAttributesExW(native_path.c_str(),
			GetFileExInfoStandard, &fileAttr) != 0)
		{
			SYSTEMTIME st;
			FileTimeToSystemTime(&fileAttr.ftCreationTime, &st);

			StreamStats stats;
			stats.accestime = filetime_to_timet(fileAttr.ftLastAccessTime);
			stats.createtime = filetime_to_timet(fileAttr.ftCreationTime);
			stats.modtime = filetime_to_timet(fileAttr.ftLastWriteTime);
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				stats.flag = StreamFlag::Directory;
			else
				stats.flag = StreamFlag::File;

			return stats;
		}

		if (const auto error = WinError::get_last(); !error.is_not_found())
			UC_LOG_ERROR(_logger) << error << L" for '" << path << L"'";

		return std::nullopt;
	}

	uint16_t WinStreamProvider::enumerate(const Path& path,
		WStringView search_pattern, List<Path>& name_list, FileFlags flags) const
	{
		const auto native_path = (path / search_pattern).native_path();

		uint16_t count = 0;

		// TODO: Test for directories
		WIN32_FIND_DATAW data;
		const auto find = FindFirstFileW(native_path.c_str(), &data);
		if (find != INVALID_HANDLE_VALUE)
		{
			do
			{
				const auto fileName = std::wstring(data.cFileName);
				if (fileName == L"." || fileName == L"..")
					continue;

				const StreamFlag fileType = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0
					? StreamFlag::Directory : StreamFlag::File;
				if (!flags.has(fileType))
					continue;

				name_list.push_back(path / fileName);
				count++;
			} while (FindNextFileW(find, &data));

			FindClose(find);
		}

		return count;
	}

	bool WinStreamProvider::create_directory(const Path& path)
	{
		const auto native_path = path.native_path();
		if (CreateDirectoryW(native_path.data(), nullptr) != 0)
			return true;

		if (const auto error = WinError::get_last(); error.is_error())
			UC_LOG_ERROR(_logger) << error << L" for '" << path << L"'";
		return false;
	}

	bool WinStreamProvider::delete_directory(const Path& path, bool recursive)
	{
		const auto native_path = path.native_path();
		if (RemoveDirectoryW(native_path.data()) != 0)
			return true;

		if (const auto error = WinError::get_last(); error.is_error())
			UC_LOG_ERROR(_logger) << error << L" for '" << path << L"'";
		return false;
	}

	Shared<ReadStream> WinStreamProvider::open_read(const Path& path)
	{
		const auto native_path = path.native_path();

		auto handle = CreateFileW(native_path.c_str(),
			GENERIC_READ, FILE_SHARE_READ, nullptr,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (handle != INVALID_HANDLE_VALUE)
			return std::make_shared<WinStream>(handle);

		if (const auto error = WinError::get_last(); !error.is_not_found())
			UC_LOG_ERROR(_logger) << error << L" for '" << path << L"'";

		return nullptr;
	}

	Shared<WriteStream> WinStreamProvider::create_new(const Path& path)
	{
		const auto native_path = path.native_path();

		auto handle = CreateFileW(native_path.c_str(),
			GENERIC_WRITE, 0, nullptr,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (handle != INVALID_HANDLE_VALUE)
			return std::make_shared<WinStream>(handle);

		if (const auto error = WinError::get_last(); !error.is_not_found())
			UC_LOG_ERROR(_logger) << error << L" for '" << path << L"'";

		return nullptr;
	}

	bool WinStreamProvider::delete_file(const Path& path)
	{
		const auto native_path = path.native_path();
		if (DeleteFileW(native_path.data()) != 0)
			return true;

		if (const auto error = WinError::get_last(); error.is_error())
			UC_LOG_ERROR(_logger) << error << L" for '" << path << L"'";
		return false;
	}

	time_t WinStreamProvider::filetime_to_timet(FILETIME const& ft)
	{
		ULARGE_INTEGER ull;
		ull.LowPart = ft.dwLowDateTime;
		ull.HighPart = ft.dwHighDateTime;
		return ull.QuadPart / 10000000ULL - 11644473600ULL;
	}
}
#endif