#include "unicore/plugins/RendererPlugin.hpp"
#include "unicore/ResourceCache.hpp"
#include "unicore/Renderer.hpp"
#include "Resources/TextureConverter.hpp"

namespace unicore
{
	void RendererPlugin::register_plugin(const ModuleContext& context)
	{
		Plugin::register_plugin(context);

		const auto cache = context.modules.find<ResourceCache>();
		const auto renderer = context.modules.find<Renderer>();

		if (cache && renderer)
		{
			cache->add_converter(std::make_shared<TextureConverter>(*renderer));
			cache->add_converter(std::make_shared<DynamicTextureConverter>(*renderer));
		}
	}
}