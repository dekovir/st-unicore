#pragma once
#include "unicore/ResourceLoader.hpp"
#include "unicore/Surface.hpp"
#include "unicore/Texture.hpp"

namespace unicore
{
	class TextureLoader : public ResourceLoaderType<Texture, ResourceLoaderPolicy::NotEmptyPath>
	{
	public:
		explicit TextureLoader(Renderer& render);

		UC_NODISCARD Shared<Resource> load(const Context& context) override;

	protected:
		Renderer& _renderer;
	};

	class DynamicTextureLoader : public ResourceLoaderType<DynamicTexture, ResourceLoaderPolicy::NotEmptyPath>
	{
	public:
		explicit DynamicTextureLoader(Renderer& render);

		UC_NODISCARD Shared<Resource> load(const Context& context) override;

	protected:
		Renderer& _renderer;
	};
}