#pragma once
#include "unicore/Font.hpp"

namespace unicore
{
	struct BitmapFontGlyph
	{
		uint8_t page;
		Recti rect;
		int xoffset;
		int yoffset;
		int xadvance;
	};

	class BitmapFont : public Font
	{
		UC_OBJECT(BitmapFont, Font)
	public:
		UC_NODISCARD size_t system_memory_use() const override;

		List<Shared<Texture>> pages;
		Dictionary<uint32_t, BitmapFontGlyph> glyphs;

		Shared<Texture> get_char_print_info(uint32_t code,
			Vector2f& pos, Rectf* rect, Rectf* uv_rect) const override;

		Shared<Texture> print_char(uint32_t code, Vector2f& pos,
			VertexTexColor2& v0, VertexTexColor2& v1,
			VertexTexColor2& v2, VertexTexColor2& v3,
			const Color4b& color = ColorConst4b::White) const override;
	};
}