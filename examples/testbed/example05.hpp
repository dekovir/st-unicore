#pragma once
#include "example.hpp"
#if defined(UNICORE_USE_GRID)
#include "unicore/math/Transform2.hpp"
#include "unicore/renderer/Sprite.hpp"
#include "unicore/renderer/SpriteBatch.hpp"
#include "unicore/renderer/PrimitiveBatch.hpp"
#include "unicore/grid/GridMap.hpp"

namespace unicore
{
	enum class CellType : uint16_t
	{
		Empty = 0,
		Grass = 1,
		Solid = 7,
	};

	class Example05 : public Example
	{
	public:
		explicit Example05(const ExampleContext& context);

		void load(IResourceCache& resources) override;
		void update() override;
		void draw() const override;

	protected:
		Shared<SpriteList> _tiles;
		Shared<Font> _font;

		RectangleTopology _topology;
		GridMap<CellType> _map;
		Transform2f _tr;

		PrimitiveBatch _graphics;
		SpriteBatch _sprite_batch;
	};
}
#endif