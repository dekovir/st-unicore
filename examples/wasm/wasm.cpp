#include "wasm.hpp"
#include "UnicoreMain.hpp"
#include "InitPlugins.hpp"
#include "unicore/system/Unicode.hpp"
#include "unicore/system/TimeSpan.hpp"
#include "unicore/system/Timer.hpp"
#include "unicore/resource/BinaryData.hpp"
#include "unicore/resource/ResourceCache.hpp"
#include "unicore/platform/Time.hpp"
#include "unicore/platform/Input.hpp"
#include "unicore/wasm/WasmEnvironment.hpp"
#include "unicore/wasm/WasmModule.hpp"
#include "unicore/wasm/WasmRuntime.hpp"
#include <locale>

namespace unicore
{
	class State
	{
	public:
		explicit State(WasmRuntime& runtime)
		{
			_init = runtime.find_function("app_init");
			_update = runtime.find_function("app_update");
			_draw = runtime.find_function("app_draw");
		}

		void init()
		{
			if (_init.has_value())
				_init->call();
		}

		void update(const TimeSpan& delta)
		{
			if (_update.has_value())
				_update->call((float)delta.total_seconds());
		}

		void draw()
		{
			if (_draw.has_value())
				_draw->call();
		}

	protected:
		Optional<WasmFunction> _init;
		Optional<WasmFunction> _update;
		Optional<WasmFunction> _draw;
	};

	static MyApp* s_example = nullptr;
	static Shared<State> s_state;
	static TimeSpan s_state_time = TimeSpanConst::Zero;

	static auto ptr_to_str(const void* void_ptr)
	{
		const auto ptr = static_cast<const uint32_t*>(void_ptr);
		const uint32_t lenInPoints = *(ptr - 1) / 2;
		const std::basic_string_view strUtf16(reinterpret_cast<const char16_t*>(ptr), lenInPoints);
		return Unicode::to_utf8(strUtf16);
	}

	static void wa_abort(const void* msg_ptr, const void* file_ptr, int line, int column)
	{
		const auto msg = ptr_to_str(msg_ptr);
		const auto file = ptr_to_str(file_ptr);
		UC_LOG_ERROR(s_example->logger) << file << ":" << line << "." << column << " - " << msg;
	}

	static void api_log(const void* void_ptr)
	{
		const auto text = ptr_to_str(void_ptr);
		s_example->logger.write(LogType::Info, text);
	}

	static int32_t api_random()
	{
		return s_example->_random.next();
	}

	static float api_random_f()
	{
		return s_example->_random.next_float_01();
	}

	static int api_screen_x()
	{
		auto& size = s_example->renderer.screen_size();
		return size.x;
	}

	static int api_screen_y()
	{
		auto& size = s_example->renderer.screen_size();
		return size.y;
	}

	static int api_mouse_x()
	{
		auto& pos = s_example->input.mouse().position();
		return pos.x;
	}

	static int api_mouse_y()
	{
		auto& pos = s_example->input.mouse().position();
		return pos.y;
	}

	static int api_mouse_button()
	{
		return s_example->input.mouse().down() ? 1 : 0;
	}

	static void api_sprite_draw_at(float x, float y)
	{
		s_example->_sprite_batch.draw(s_example->_spr, Vector2f(x, y));
		s_example->_sprite_count++;
	}

	MyApp::MyApp(const CoreSettings& settings)
		: SDLApplication(create_settings(settings, "Wasm"))
		, _console(80, 20)
	{
		UC_LOG_INFO(logger) << "Starting";

		s_example = this;

		init_plugins(*this);
	}

	MyApp::~MyApp()
	{
		s_example = nullptr;
		s_state = nullptr;
	}

	void MyApp::on_init()
	{
		_font = resources.load<Font>("font_004.fnt"_path);
		if (auto tex = resources.load<Texture>("zazaka.png"_path))
			_spr = std::make_shared<Sprite>(tex);

		ConsoleLogger wasm_logger(_console);
		_env = WasmEnvironment::create(wasm_logger);

		UC_LOG_INFO(wasm_logger) << "Loading release.wasm";
		if (const auto data = resources.load<BinaryData>("logic.wasm"_path))
		{
			_runtime = _env->new_runtime();
			if (_runtime)
			{
				_module = _env->parse_module(data);
				if (_module)
				{
					_module->load_to(*_runtime);
					_module->link_module("env", "abort", &wa_abort);
					_module->link("log", &api_log);

					_module->link("random", &api_random, true);
					_module->link("random_f", &api_random_f, true);

					_module->link("screen_x", &api_screen_x);
					_module->link("screen_y", &api_screen_y);

					_module->link("mouse_x", &api_mouse_x);
					_module->link("mouse_y", &api_mouse_y);
					_module->link("mouse_button", &api_mouse_button);

					_module->link("sprite_draw_at", &api_sprite_draw_at);

					UC_LOG_INFO(wasm_logger) << "Runtime functions:";
					unsigned index = 0;
					_runtime->enum_functions([&wasm_logger, &index](WasmFunction func)
						{
							UC_LOG_INFO(wasm_logger) << index << ": " << func;
							index++;
						});

					s_state = std::make_shared<State>(*_runtime);
				}
			}
		}

		s_state->init();

		resources.unload_unused();
	}

	void MyApp::on_update()
	{
#if !defined(UNICORE_PLATFORM_WEB)
		if (input.keyboard().down(KeyCode::Escape))
			platform.looper.quit();
#endif

		_sprite_batch.clear();

		// UPDATE STATE
		s_state_time += time.delta();

		{
			AutoTimer timer(_update_time);
			static constexpr auto fps_lock = TimeSpan::from_seconds(1. / 60.);
			while (s_state_time > fps_lock)
			{
				s_state->update(fps_lock);
				s_state_time -= fps_lock;
			}
		}

		// DRAW STATE
		_sprite_count = 0;
		{
			AutoTimer timer(_draw_time);
			s_state->draw();
		}

		// DRAW CONSOLE
		const auto size = _console.size();
		for (int y = 0; y < size.y; y++)
			for (int x = 0; x < size.x; x++)
			{
				DefaultConsoleChar c;
				if (!_console.get(x, y, c))
					continue;

				if (c.c == 0 || c.color == 0)
					continue;

				const Vector2f pos(x * 10, y * 20 + 100);
				Char buf[2] = { c.c, 0 };

				const StringView str(buf);
				const Color4b color(
					c.color.has(ConsoleColor8::ForegroundRed) ? 255 : 0,
					c.color.has(ConsoleColor8::ForegroundGreen) ? 255 : 0,
					c.color.has(ConsoleColor8::ForegroundBlue) ? 255 : 0
				);
				_sprite_batch.print(_font, pos, Unicode::to_utf32(str), color);
			}

		// DRAW TEXT
		const auto fps_str = StringBuilder::format(U"FPS: {}", fps());
		const auto sprites_str = StringBuilder::format(U"Sprites: {}", _sprite_count);
		const auto update_str = StringBuilder::format(U"Update time: {}ms", _update_time.total_milliseconds());
		const auto draw_str = StringBuilder::format(U"Draw time: {}ms", _draw_time.total_milliseconds());

		_sprite_batch.print(_font, { 0, 0 }, fps_str);
		_sprite_batch.print(_font, { 0, 20 }, sprites_str);
		_sprite_batch.print(_font, { 0, 40 }, update_str);
		_sprite_batch.print(_font, { 0, 60 }, draw_str);

		_sprite_batch.flush();
	}

	void MyApp::on_draw()
	{
		renderer.clear(ColorConst4b::Black);

		_sprite_batch.render(renderer);
	}

	UNICORE_MAIN_CORE(MyApp);
}