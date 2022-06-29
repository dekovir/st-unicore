#include "main.h"
#include "UnicoreMain.h"
#include "unicore/Input.hpp"
#include "unicore/Memory.hpp"
#include "unicore/Graphics.hpp"
#include "unicore/SpriteBatch.hpp"

namespace unicore
{
	void Entity::update(const Vector2i& size, float delta)
	{
		center += velocity * delta;

		if (
			(velocity.x < 0 && center.x < radius) ||
			(velocity.x > 0 && center.x > size.x - radius))
		{
			velocity.x *= -1;
		}
		if (
			(velocity.y < 0 && center.y < radius) ||
			(velocity.y > 0 && center.y > size.y - radius))
		{
			velocity.y *= -1;
		}

		angle += angle_speed * delta;
	}

	MyCore::MyCore(const CoreSettings& settings)
		: Core(settings)
	{
		UC_LOG_INFO(logger) << "Starting";

		//List<WString> files;
		//file_system.enumerate(L"assets"_path, files, FileFlag::File);

#if 0
		{
			BitmapSurface surface(64, 64);
			surface.fill(ColorConst4b::Cyan);
			_tex = render.create_texture(surface);
		}
#else
		_tex = resources.load<Texture>(L"assets/zazaka.png"_path);
#endif
		_font = resources.load<BitmapFont>(L"assets/font_004.fnt"_path);

		size_t sys_mem;
		resources.calc_memory_use(&sys_mem, nullptr);
		UC_LOG_DEBUG(logger) << "Resource used system memory " << MemorySize{ sys_mem };

		resources.unload_unused();
		resources.dump_used();
		}

	void MyCore::on_update()
	{
#if !defined(UNICORE_PLATFORM_WEB)
		if (input.key_code(KeyCode::Escape))
			platform.quit();
#endif

		if (_tex && input.mouse_button(MouseButton::Left))
			spawn_entity(input.mouse_position().cast<float>(), _tex->size());

		auto& size = render.screen_size();

		for (auto& entity : _entites)
			entity.update(size, time.delta().total_seconds());
	}

	void MyCore::on_draw()
	{
		auto& size = render.screen_size();

		render.clear(ColorConst4b::Black);
		Graphics graphics;
		graphics.begin();
		graphics
			.draw_tri(
				Vertex2::from_pos({ 100, 100 }, ColorConst4b::Yellow),
				Vertex2::from_pos({ 200, 100 }, ColorConst4b::Cyan),
				Vertex2::from_pos({ 100, 200 }, ColorConst4b::Magenta))
			.set_color(ColorConst4b::Red)
			.draw_line(Vector2iConst::Zero, size)
			.draw_line(Vector2i(0, size.y), Vector2i(size.x, 0))
			.set_color(ColorConst4b::White)
			.set_color(0x20FF00FF_rgba)
			.draw_point(Vector2f(400, 400))
			;

		graphics.end();
		graphics.to_render(render);

		const String fps_str = "FPS: " + std::to_string(fps());
		const String count_str = "Count: " + std::to_string(_entites.size());

		SpriteBatch batch;
		batch.begin();
		batch.draw(_tex, {32, 32});

		for (const auto& entity : _entites)
			batch.draw(_tex, entity.center, entity.angle, entity.scale, entity.color);

		batch.print(_font, { 0, 0 }, fps_str);
		batch.print(_font, { 0, 20 }, count_str);

		batch.end();
		batch.to_render(render);
	}

	void MyCore::spawn_entity(const Vector2f& position, const Vector2i& size)
	{
		Entity entity;
		entity.center = position;
		entity.velocity = Vector2f(
			_random.range(100.f, 500.f) * (_random.boolean() ? +1.f : -1.f),
			_random.range(100.f, 500.f) * (_random.boolean() ? +1.f : -1.f)
		);
		entity.color = Color4b::create_random(_random);

		entity.radius = _random.range(25.f, 50.f);
		entity.scale = 2.f * entity.radius / static_cast<float>(size.x);

		entity.angle = Degrees(_random.range(0.f, 359.f));
		entity.angle_speed = Degrees(_random.range(45.f, 300.f));

		_entites.push_back(entity);
	}

	UNICORE_MAIN_CORE(MyCore);
	}