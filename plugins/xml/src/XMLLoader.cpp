#include "XMLLoader.hpp"
#include "unicore/Data.hpp"
#include "unicore/ResourceCache.hpp"

namespace unicore
{
	XMLDataLoader::XMLDataLoader()
		: ResourceLoaderT({ L".xml" })
	{
	}

	Shared<Resource> XMLDataLoader::load(const ResourceLoaderContext& context)
	{
		const auto data = context.cache.load<TextData>(context.path);
		if (!data)
		{
			UC_LOG_ERROR(context.logger) << "Failed to load BinaryData";
			return nullptr;
		}

		const auto xml = make_shared<XMLData>();
		const auto& str = data->data();
		if (xml->doc.Parse(str.data(), str.size()) == tinyxml2::XML_SUCCESS)
			return xml;

		return nullptr;
	}
}