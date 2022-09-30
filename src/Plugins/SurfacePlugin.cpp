#include "unicore/plugins/SurfacePlugin.hpp"
#include "unicore/ResourceCache.hpp"
#include "Resources/SolidSizeLoader.hpp"
#include "Resources/SpriteListTileSetLoader.hpp"

namespace unicore
{
	void SurfacePlugin::register_plugin(const ModuleContext& context)
	{
		Plugin::register_plugin(context);

		if (const auto cache = context.modules.find<ResourceCache>())
		{
			cache->add_loader(std::make_shared<SurfaceSizeSurfaceLoader>());
			cache->add_loader(std::make_shared<DynamicSurfaceSolidSizeLoader>());
			cache->add_loader(std::make_shared<SpriteListTileSetLoader>());
		}
	}
}
