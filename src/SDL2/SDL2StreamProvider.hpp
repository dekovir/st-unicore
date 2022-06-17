#pragma once
#include "unicore/StreamProvider.hpp"
#if defined(UNICORE_USE_SDL2)
#include "SDL2Stream.hpp"

namespace unicore
{
	class SDL2StreamProvider : public StreamProvider
	{
	public:
		bool exists(const Path& path) override;
		Optional<FileStats> stats(const Path& path) override;

		uint16_t enumerate(const Path& path, List<Path>& name_list, FileFlags flags) override;

		Shared<ReadStream> open_read(const Path& path) override;
		Shared<WriteStream> create_new(const Path& path) override;
	};
}

#endif