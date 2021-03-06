#pragma once
#include "example.hpp"
#include "unicore/SpriteBatch.hpp"
#include "unicore/TimeSpan.hpp"

namespace unicore
{
	struct Entity
	{
		Vector2f center;
		Vector2f velocity = Vector2f(300, 300);

		float radius;
		Vector2f scale;

		Radians angle;
		Radians angle_speed;

		Color4b color;

		void update(const Vector2i& size, float delta);
	};

	class Example02 : public Example
	{
	public:
		explicit Example02(const ExampleContext& context);

		bool load(ResourceCache& resources) override;

		void update() override;
		void draw() const override;

	protected:
		Shared<Texture> _tex;
		Shared<Font> _font;

		SpriteBatch _sprite_batch;
		List<Entity> _entites;

		TimeSpan _add_time = TimeSpanConst::Zero;
		bool _add_active = false;

		void spawn_entity(const Vector2f& position, const Vector2i& size);
		void spawn_entities(unsigned count);
	};
}
