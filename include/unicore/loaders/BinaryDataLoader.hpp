#pragma once
#include "unicore/BinaryData.hpp"
#include "unicore/ResourceLoader.hpp"

namespace unicore
{
	struct BinaryDataLoadPolicy : ResourceLoaderPathPolicy::Extension
	{
		BinaryDataLoadPolicy()
			: Extension({ ".dat" })
		{
		}
	};

	class BinaryDataLoader : public ResourceLoaderTyped<
		ResourceLoaderTypePolicy::Multiple<BinaryData>, BinaryDataLoadPolicy>
	{
		UC_OBJECT(BinaryDataLoader, ResourceLoaderTyped)
	public:
		UC_NODISCARD Shared<Resource> load(const Context& context) override;
	};
}