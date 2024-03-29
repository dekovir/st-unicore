#include "unicore/xml/XMLPlugin.hpp"
#include "unicore/resource/ResourceCache.hpp"
#include "unicore/xml/XMLDataLoader.hpp"

namespace unicore
{
	static XMLDataLoader xml_loader;

	void XMLPlugin::register_plugin(const ModuleContext& context)
	{
		Plugin::register_plugin(context);

		if (const auto cache = context.modules.find<ResourceCache>())
			cache->add_loader(std::make_shared<XMLDataLoader>());
	}
}