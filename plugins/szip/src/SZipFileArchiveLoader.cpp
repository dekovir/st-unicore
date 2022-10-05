#include "SZipFileArchiveLoader.hpp"
#include "unicore/ResourceCache.hpp"
#include "unicore/szip/SZipFileProvider.hpp"
#include "SZipUtils.hpp"
#include "SZipError.hpp"
#include <7zCrc.h>

namespace unicore
{
	SZipFileArchiveLoader::SZipFileArchiveLoader()
	{
		CrcGenerateTable();
	}

	Shared<Resource> SZipFileArchiveLoader::load(const Context& context)
	{
		const auto file = context.cache.load<ReadFile>(context.path);
		if (!file)
		{
			UC_LOG_ERROR(context.logger) << "Failed to open file";
			return nullptr;
		}

		auto stream = std::make_shared<SZipStream>(file);
		auto data = std::make_unique<SZipFileData>(stream);

		const auto result = SzArEx_Open(&data->db, &data->look_stream.s,
			SZipUtils::get_alloc_main(), SZipUtils::get_alloc_temp());
		if (result != SZ_OK)
		{
			UC_LOG_ERROR(context.logger) << SZipError(result);
			return nullptr;
		}

		return std::make_shared<SZipFileProvider>(std::move(data), context.logger);
	}
}