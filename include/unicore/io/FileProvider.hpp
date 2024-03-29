#pragma once
#include "unicore/resource/Resource.hpp"
#include "unicore/system/EnumFlag.hpp"
#include "unicore/system/Containers.hpp"
#include "unicore/system/DateTime.hpp"
#include "unicore/io/Path.hpp"

namespace unicore
{
	class MemoryChunk;

	class ReadFile;
	class WriteFile;

	enum class FileType
	{
		File,
		Directory,
		Unknown,
	};

	struct FileStats
	{
		int64_t size = 0;
		DateTime mod_time;
		DateTime create_time;
		DateTime access_time;
		FileType type;
	};

	enum class EnumerateFlag
	{
		Files = 1 << 0,
		Directories = 1 << 1,
		All = Files | Directories
	};
	UNICORE_ENUM_FLAGS(EnumerateFlag, EnumerateFlags);

	struct EnumerateOptions
	{
		EnumerateFlags flags = EnumerateFlag::All;
	};

	// FileProvider ///////////////////////////////////////////////////////////////
	class FileProvider : public Resource
	{
		UC_OBJECT(FileProvider, Resource)
	public:
		UC_NODISCARD size_t get_system_memory_use() const override;

		UC_NODISCARD virtual Optional<FileStats> stats(const Path& path) const = 0;
		UC_NODISCARD virtual bool exists(const Path& path) const;

		UC_NODISCARD bool is_file(const Path& path) const;
		UC_NODISCARD bool is_directory(const Path& path) const;

		virtual uint16_t enumerate_entries(const Path& path, StringView search_pattern,
			List<String>& name_list, const EnumerateOptions& options = {}) const = 0;

		uint16_t enumerate_entries(const Path& path, List<String>& name_list,
			const EnumerateOptions& options = {}) const;

		uint16_t enumerate_files(const Path& path,
			StringView search_pattern, List<String>& name_list) const;
		uint16_t enumerate_files(const Path& path, List<String>& name_list) const;

		uint16_t enumerate_dirs(const Path& path,
			StringView search_pattern, List<String>& name_list) const;
		uint16_t enumerate_dirs(const Path& path, List<String>& name_list) const;

		UC_NODISCARD virtual List<String> get_files(
			const Path& path, StringView search_pattern) const;
		UC_NODISCARD virtual List<String> get_files(const Path& path) const;

		UC_NODISCARD virtual List<String> get_dirs(
			const Path& path, StringView search_pattern) const;
		UC_NODISCARD virtual List<String> get_dirs(const Path& path) const;
	};

	// ReadFileProvider ///////////////////////////////////////////////////////////
	class ReadFileProvider : public FileProvider
	{
		UC_OBJECT(ReadFileProvider, FileProvider)
	public:
		UC_NODISCARD virtual Shared<ReadFile> open_read(const Path& path) = 0;
		UC_NODISCARD virtual Shared<MemoryChunk> read_chunk(const Path& path);

	protected:
		static bool enumerate_test_flags(FileType type, EnumerateFlags flags);
		static bool enumerate_test_options(FileType type, const EnumerateOptions& options);
	};

	// WriteFileProvider /////////////////////////////////////////////////////////
	class WriteFileProvider : public ReadFileProvider
	{
		UC_OBJECT(WriteFileProvider, ReadFileProvider)
	public:
		virtual bool create_directory(const Path& path) = 0;
		virtual bool delete_directory(const Path& path, bool recursive = false) = 0;

		virtual Shared<WriteFile> create_new(const Path& path) = 0;

		virtual bool delete_file(const Path& path) = 0;

		virtual bool write_chunk(const Path& path, const MemoryChunk& chunk);
	};

	// DirectoryFileProvider //////////////////////////////////////////////////////
	class DirectoryFileProvider : public ReadFileProvider
	{
		UC_OBJECT(DirectoryFileProvider, ReadFileProvider)
	public:
		explicit DirectoryFileProvider(ReadFileProvider& provider, const Path& base)
			: _provider(provider), _base(base) {}

		UC_NODISCARD size_t get_system_memory_use() const override;

		UC_NODISCARD Optional<FileStats> stats(const Path& path) const override;
		UC_NODISCARD bool exists(const Path& path) const override;

		uint16_t enumerate_entries(const Path& path, StringView search_pattern,
			List<String>& name_list, const EnumerateOptions& options) const override;

		Shared<ReadFile> open_read(const Path& path) override;

	protected:
		ReadFileProvider& _provider;
		Path _base;

		UC_NODISCARD Path make_path(const Path& path) const { return _base / path; }
	};

	// CachedFileProvider /////////////////////////////////////////////////////////
	class CachedFileProvider : public ReadFileProvider, protected CachedPathData<intptr_t>
	{
		UC_OBJECT(CachedFileProvider, ReadFileProvider)
	public:
		UC_NODISCARD size_t get_system_memory_use() const override;

		UC_NODISCARD bool exists(const Path& path) const override;
		UC_NODISCARD Optional<FileStats> stats(const Path& path) const override;

		uint16_t enumerate_entries(const Path& path, StringView search_pattern,
			List<String>& name_list, const EnumerateOptions& options) const override;

		Shared<ReadFile> open_read(const Path& path) override;

	protected:
		UC_NODISCARD virtual Optional<FileStats> stats_index(intptr_t index) const = 0;
		UC_NODISCARD virtual Shared<ReadFile> open_read_index(intptr_t index) = 0;

		UC_NODISCARD virtual bool enumerate_index(intptr_t index, const EnumerateOptions& options) const;
	};
}