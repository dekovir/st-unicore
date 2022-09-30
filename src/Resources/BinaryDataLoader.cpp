#include "BinaryDataLoader.hpp"
#include "unicore/File.hpp"
#include "unicore/ResourceCache.hpp"

namespace unicore
{
	Shared<Resource> BinaryDataLoader::load(const Context& context)
	{
		// TODO: Log open_read failed
		const auto file = context.cache.load<ReadFile>(context.path);
		if (!file) return nullptr;

		file->seek(0);
		const auto size = file->size();

		MemoryChunk chunk(size);
		if (file->read(chunk.data(), size))
			return std::make_shared<BinaryData>(chunk);

		UC_LOG_ERROR(context.logger) << "Read failed";
		return nullptr;
	}
}
