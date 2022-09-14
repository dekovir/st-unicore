#include "EasyFont.hpp"
#if defined(UNICORE_USE_STB_EASY_FONT)
#include "unicore/Vector3.hpp"
#include <stb_easy_font.h>

namespace unicore
{
	struct Vertex
	{
		Vector2f pos;
		float z;
		uint32_t color;
	};

	// TODO: Optimize
	constexpr auto BUFFER_SIZE = 1024 * 2;
	static Vertex s_quads[BUFFER_SIZE];

	EasyFont::EasyFont()
		: _height(static_cast<float>(stb_easy_font_height("W")))
	{
	}

	size_t EasyFont::get_system_memory_use() const
	{
		return sizeof(EasyFont);
	}

	size_t EasyFont::print_quads(const Vector2f& position, StringView text, List<VertexColor2>& quads) const
	{
		const auto num_quads = stb_easy_font_print(
			position.x, position.y, (char*)text.data(),
			nullptr, s_quads, BUFFER_SIZE * sizeof(Vertex));

		constexpr auto color = ColorConst4b::White;

		for (auto i = 0; i < num_quads; i++)
		{
			quads.emplace_back(s_quads[i * 4 + 0].pos, color);
			quads.emplace_back(s_quads[i * 4 + 1].pos, color);
			quads.emplace_back(s_quads[i * 4 + 2].pos, color);
			quads.emplace_back(s_quads[i * 4 + 3].pos, color);
		}

		return num_quads;
	}
}
#endif