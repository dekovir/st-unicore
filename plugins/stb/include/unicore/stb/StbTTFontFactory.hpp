#pragma once
#include "unicore/Font.hpp"
#if defined(UNICORE_USE_STB_TRUETYPE)
#include <stb_truetype.h>

namespace unicore
{
	class BinaryData;

	class StbTTFontFactory : public TTFontFactory
	{
		UC_OBJECT(StbTTFontFactory, TTFontFactory)
	public:
		StbTTFontFactory(Renderer& renderer, const Shared<BinaryData>& data);

		UC_NODISCARD bool valid() const { return _valid; }

		UC_NODISCARD const stbtt_fontinfo* info() const { return &_font_info; }

		UC_NODISCARD size_t get_system_memory_use() const override;

		UC_NODISCARD Shared<TexturedFont> create_options(const Context& context, const TTFontOptions& options) override;

	protected:
		Renderer& _renderer;
		Shared<BinaryData> _data;
		stbtt_fontinfo _font_info{};
		bool _valid;
	};
}
#endif