#include "unicore/stb/StbTTFontFactory.hpp"
#if defined(UNICORE_USE_STB_TRUETYPE)
#include "unicore/BinaryData.hpp"
#include "unicore/Logger.hpp"
#include "unicore/Canvas.hpp"
#include "unicore/Surface.hpp"
#include "unicore/Renderer.hpp"
#include "unicore/stb/StbRectPack.hpp"
#include "unicore/stb/StbTTFont.hpp"

namespace unicore
{
	StbTTFontFactory::StbTTFontFactory(const Shared<BinaryData>& data)
		: _data(data)
	{
		_valid = stbtt_InitFont(&_font_info,
			static_cast<const unsigned char*>(_data->data()), 0);
	}

	size_t StbTTFontFactory::get_system_memory_use() const
	{
		return sizeof(StbTTFontFactory) + _data->size();
	}

	Shared<TexturedFont> StbTTFontFactory::create(
		Renderer& renderer, const TTFontOptions& options, Logger* logger)
	{
		if (!valid()) return nullptr;

		const auto scale = stbtt_ScaleForPixelHeight(
			&_font_info, options.height);

		StbTTFont::ConstructionParams params;

		{
			int advance;
			stbtt_GetCodepointHMetrics(&_font_info, L' ', &advance, nullptr);
			params.space_width = scale * static_cast<float>(advance);
		}

		int ascent, descent, line_gap;
		stbtt_GetFontVMetrics(&_font_info, &ascent, &descent, &line_gap);
		//const auto baseline = Math::round_to_int(scale * static_cast<float>(ascent));
		params.height = options.height;

		// GENERATE CHAR BITMAPS
		const size_t char_count = options.chars.size();
		List<unsigned char*> item_bm(char_count);
		List<Vector2i> item_size(char_count);
		List<Vector2i> item_off(char_count);

		for (unsigned i = 0; i < char_count; i++)
		{
			const auto c = options.chars[i];

			int w, h, xoff, yoff;
			auto bm = stbtt_GetCodepointBitmap(
				&_font_info, 0, scale, c, &w, &h, &xoff, &yoff);

			item_bm[i] = bm;
			item_size[i] = { w + 2, h + 2 };
			item_off[i] = { xoff, yoff };
		}

		// PACK CHARS
		List<Recti> item_packed(char_count);

		StbRectPack packer;
		Vector2i surface_size;

		const auto start_size = packer.calc_start_size(item_size);
		if (!packer.pack_optimize(item_size, item_packed, surface_size, { start_size, 16 }))
		{
			UC_LOG_ERROR(logger) << "Failed to pack";
			return nullptr;
		}

		// COPY CHARS FONT TO SURFACE
		DynamicSurface font_surface(surface_size);
		Canvas canvas(font_surface);
		canvas.fill(ColorConst4b::Clear);

		for (unsigned i = 0; i < char_count; i++)
		{
			const auto& r = item_packed[i];
			const Recti packed = { r.x + 1, r.y + 1, r.w - 2, r.h - 2 };

			// TODO: Optimize
			for (int y = 0; y < packed.h; y++)
				for (int x = 0; x < packed.w; x++)
				{
					const auto a = item_bm[i][x + y * packed.w];
					const Color4b color(255, 255, 255, a);
					canvas.draw_point({ packed.x + x, packed.y + y }, color);
				}
			stbtt_FreeBitmap(item_bm[i], nullptr);

			stbtt_bakedchar info;
			info.xoff = static_cast<float>(item_off[i].x);
			info.yoff = static_cast<float>(item_off[i].y);

			int advance;
			stbtt_GetCodepointHMetrics(&_font_info, options.chars[i], &advance, nullptr);
			info.xadvance = scale * static_cast<Float>(advance);
			info.x0 = static_cast<UInt16>(packed.min_x());
			info.y0 = static_cast<UInt16>(packed.min_y());
			info.x1 = static_cast<UInt16>(packed.max_x());
			info.y1 = static_cast<UInt16>(packed.max_y());

			params.infos[options.chars[i]] = info;
		}

		params.texture = renderer.create_texture(font_surface);

		return std::make_shared<StbTTFont>(params);
	}
}
#endif