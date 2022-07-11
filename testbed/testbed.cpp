#include "testbed.hpp"
#include "UnicoreMain.h"
#include "unicore/Input.hpp"
#include "unicore/Surface.hpp"
#include "unicore/Font.hpp"

namespace unicore
{
	MyCore::MyCore(const CoreSettings& settings)
		: Core(settings)
	{
		UC_LOG_INFO(logger) << "Starting";

		_font = resources.load<Font>(L"assets/font_004.fnt"_path);

		set_example(0);
	}

	void MyCore::on_update()
	{
#if !defined(UNICORE_PLATFORM_WEB)
		if (input.keyboard().down(KeyCode::Escape))
			platform.quit();
#endif

		if (input.keyboard().down_change(KeyCode::Space))
			set_example((_example_index + 1) % ExampleCatalog::get_all().size());

		if (_example)
			_example->update();

		// SPRITE BATCH ////////////////////////////////////////////////////////////
		const String title_str = "Example: " + ExampleCatalog::get_all()[_example_index].title;
		const String fps_str = "FPS: " + std::to_string(fps());
		const String draw_str = "Draw: " + std::to_string(_draw_calls);

		_sprite_batch.begin();

		_sprite_batch.print(_font, { 0, 0 }, fps_str);
		_sprite_batch.print(_font, { 200, 0 }, title_str);
		_sprite_batch.print(_font, { 0, 20 }, draw_str);

		_sprite_batch.end();
	}

	void MyCore::on_draw()
	{
		render.clear(ColorConst4b::Black);

		if (_example)
			_example->draw();

		_sprite_batch.render(render);

		_draw_calls = render.draw_calls();
	}

	void MyCore::set_example(int index)
	{
		if (_example_index == index) return;

		auto& info = ExampleCatalog::get_all()[index];

		const auto example = info.factory({ logger, _random, time, input, render });
		if (!example->load(resources))
			return;

		_example_index = index;
		resources.unload_unused();
		_example = example;
	}

	UNICORE_MAIN_CORE(MyCore);
}