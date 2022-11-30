#include "raycast.hpp"
#include "UnicoreMain.hpp"
#include "InitPlugins.hpp"
#include "unicore/platform/Input.hpp"
#include "unicore/platform/Time.hpp"
#include "unicore/renderer/Canvas.hpp"
#include "unicore/renderer/Font.hpp"

namespace unicore
{
	constexpr Vector2i WindowSize = Vector2i(1024, 768);
	constexpr DisplayWindowFlags WindowFlags = DisplayWindowFlag::Resizable;

	class MapModel : public RaycastModel
	{
	public:
		explicit MapModel(const Map& map)
			: _map(map)
		{
		}

		UC_NODISCARD Size get_system_memory_use() const override { return 0; }
		UC_NODISCARD IndexArg size() const override { return _map.size(); }
		UC_NODISCARD DataType get_at(IndexArg index) const override
		{
			Cell cell;
			if (_map.get(index.x, index.y, cell))
			{
				if (cell.type == CellType::Wall)
					return true;
			}

			return false;
		}

	protected:
		Map _map;
	};

	MyCore::MyCore(const CoreSettings& settings)
		: SDLApplication(create_settings(settings, "Raycast", { false, WindowSize, WindowFlags }))
	{
		init_plugins(*this);

		constexpr int side = 20;

		const Cell wall = { CellType::Wall };

		_map = std::make_shared<Map>(side, side);
		Canvas canvas(*_map);
		canvas.fill(Cell{ CellType::Floor });
		canvas.draw_rect({ 0, 0, side, side }, wall);
		canvas.draw_line_h({ 1, 5 }, 5, wall);
		canvas.draw_line_h({ 7, 5 }, 5, wall);
		canvas.draw_line_h({ 13, 5 }, 5, wall);

		_model = std::make_shared<MapModel>(*_map);

		_player = std::make_shared<Player>();
		_player->position = { 3.5f, 3.5f };
		//_player->angle = 45_deg;

		_raycast = std::make_shared<RaycastRenderer>();
	}

	void MyCore::on_init()
	{
		UC_LOG_INFO(logger) << "Starting";

		_font = resources.create<Font>(EmptyResourceOptions{});
	}

	void MyCore::on_update()
	{
		const auto screen_size = renderer.screen_size();

#if !defined(UNICORE_PLATFORM_WEB)
		if (input.keyboard().down(KeyCode::Escape))
			platform.looper.quit();

		if (
			input.keyboard().down_changed(KeyCode::Enter) &&
			input.keyboard().mods(KeyModCombine::Alt))
		{
			auto mode = display.get_mode();
			mode.fullscreen = !mode.fullscreen;
			mode.window_size = WindowSize;
			mode.window_flags = WindowFlags;
			display.set_mode(mode);
		}
#endif

		if (_player)
		{
			constexpr float forward_speed = +2;
			constexpr float backward_speed = -1;
			constexpr auto turn_speed = 180_deg;

			const auto delta = static_cast<float>(time.delta().total_seconds());

			if (input.keyboard().down(KeyCode::W) || input.keyboard().down(KeyCode::ArrowUp))
				_player->move_forward(forward_speed * delta);

			if (input.keyboard().down(KeyCode::S) || input.keyboard().down(KeyCode::ArrowDown))
				_player->move_forward(backward_speed * delta);

			if (input.keyboard().down(KeyCode::A) || input.keyboard().down(KeyCode::ArrowLeft))
				_player->angle -= turn_speed * delta;

			if (input.keyboard().down(KeyCode::D) || input.keyboard().down(KeyCode::ArrowRight))
				_player->angle += turn_speed * delta;
		}

		_graphics.clear();
		_sprite_batch.clear();

		if (_map)
		{
			constexpr float scale = 16;

			_graphics.move({ 0, 30 });
			const auto size = _map->size();
			for (int y = 0; y < size.y; y++)
				for (int x = 0; x < size.x; x++)
				{
					Cell cell;
					if (!_map->get(x, y, cell))
						continue;

					switch (cell.type)
					{
					case CellType::Floor:
						_graphics.set_color(ColorConst4b::Gray);
						break;

					case CellType::Wall:
						_graphics.set_color(ColorConst4b::White);
						break;
					}

					const Rectf rect(scale * x, scale * y, scale, scale);
					_graphics.draw_rect(rect, true);
				}

			if (_player)
			{
				const auto pos = _player->position * scale;

				_graphics.set_color(ColorConst4b::Green);
				_graphics.draw_circle(pos, scale / 4, false, 16);
				_graphics.draw_line(pos, pos + _player->forward() * (scale / 2));

				constexpr int rays_count = 60;
				constexpr Radians fov = 70_deg;
				constexpr Radians step = fov / rays_count;

				for (int i = 0; i < rays_count; i++)
				{
					const Vector2f dir = VectorConst2f::AxisX.rotate(_player->angle - fov /2 + step * i);
					const Ray2 ray{ _player->position, dir };

					RaycastRenderer::HitInfo hit;
					if (RaycastRenderer::raycast(*_model, ray, hit))
					{
						_graphics.set_color(ColorConst4b::Red);
						_graphics.draw_line(pos, hit.point * scale);
					}
				}
			}
		}

		if (_font)
			_sprite_batch.print(_font, { 0, 0 }, U"Raycast example");

		_graphics.flush();
		_sprite_batch.flush();
	}

	void MyCore::on_draw()
	{
		renderer.clear(ColorConst4b::Black);

		_graphics.render(renderer);

		_sprite_batch.render(renderer);
	}

	UNICORE_MAIN_CORE(MyCore);
}