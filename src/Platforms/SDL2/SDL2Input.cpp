#include "SDL2Input.hpp"
#if defined(UNICORE_USE_SDL2)
#include "unicore/io/Logger.hpp"
#include <SDL.h>

namespace unicore
{
	// https://wiki.libsdl.org/SDL_Scancode
	const std::map<SDL_Scancode, KeyCode> s_key_bindings =
	{
			{SDL_SCANCODE_SPACE, KeyCode::Space},
			{SDL_SCANCODE_0, KeyCode::Num0},
			{SDL_SCANCODE_1, KeyCode::Num1},
			{SDL_SCANCODE_2, KeyCode::Num2},
			{SDL_SCANCODE_3, KeyCode::Num3},
			{SDL_SCANCODE_4, KeyCode::Num4},
			{SDL_SCANCODE_5, KeyCode::Num5},
			{SDL_SCANCODE_6, KeyCode::Num6},
			{SDL_SCANCODE_7, KeyCode::Num7},
			{SDL_SCANCODE_8, KeyCode::Num8},
			{SDL_SCANCODE_9, KeyCode::Num9},
			{SDL_SCANCODE_SEMICOLON, KeyCode::Semicolon},
			{SDL_SCANCODE_A, KeyCode::A},
			{SDL_SCANCODE_B, KeyCode::B},
			{SDL_SCANCODE_C, KeyCode::C},
			{SDL_SCANCODE_D, KeyCode::D},
			{SDL_SCANCODE_E, KeyCode::E},
			{SDL_SCANCODE_F, KeyCode::F},
			{SDL_SCANCODE_G, KeyCode::G},
			{SDL_SCANCODE_H, KeyCode::H},
			{SDL_SCANCODE_I, KeyCode::I},
			{SDL_SCANCODE_J, KeyCode::K},
			{SDL_SCANCODE_K, KeyCode::K},
			{SDL_SCANCODE_L, KeyCode::L},
			{SDL_SCANCODE_M, KeyCode::M},
			{SDL_SCANCODE_N, KeyCode::N},
			{SDL_SCANCODE_O, KeyCode::O},
			{SDL_SCANCODE_P, KeyCode::P},
			{SDL_SCANCODE_Q, KeyCode::Q},
			{SDL_SCANCODE_R, KeyCode::R},
			{SDL_SCANCODE_S, KeyCode::S},
			{SDL_SCANCODE_T, KeyCode::T},
			{SDL_SCANCODE_U, KeyCode::U},
			{SDL_SCANCODE_V, KeyCode::V},
			{SDL_SCANCODE_W, KeyCode::W},
			{SDL_SCANCODE_X, KeyCode::X},
			{SDL_SCANCODE_Y, KeyCode::Y},
			{SDL_SCANCODE_Z, KeyCode::Z},
			{SDL_SCANCODE_LEFTBRACKET, KeyCode::BracketLeft},
			{SDL_SCANCODE_RIGHTBRACKET, KeyCode::BracketRight},
			{SDL_SCANCODE_ESCAPE, KeyCode::Escape},
			{SDL_SCANCODE_RETURN, KeyCode::Return},
			{SDL_SCANCODE_TAB, KeyCode::Tab},
			{SDL_SCANCODE_BACKSPACE, KeyCode::BackSpace},
			{SDL_SCANCODE_INSERT, KeyCode::Insert},
			{SDL_SCANCODE_DELETE, KeyCode::Delete},
			{SDL_SCANCODE_RIGHT, KeyCode::ArrowRight},
			{SDL_SCANCODE_LEFT, KeyCode::ArrowLeft},
			{SDL_SCANCODE_DOWN, KeyCode::ArrowDown},
			{SDL_SCANCODE_UP, KeyCode::ArrowUp},
			{SDL_SCANCODE_PAGEUP, KeyCode::PageUp},
			{SDL_SCANCODE_PAGEDOWN, KeyCode::PageDown},
			{SDL_SCANCODE_HOME, KeyCode::Home},
			{SDL_SCANCODE_END, KeyCode::End},

			// F KEYS
			{SDL_SCANCODE_F1, KeyCode::F1},
			{SDL_SCANCODE_F2, KeyCode::F2},
			{SDL_SCANCODE_F3, KeyCode::F3},
			{SDL_SCANCODE_F4, KeyCode::F4},
			{SDL_SCANCODE_F5, KeyCode::F5},
			{SDL_SCANCODE_F6, KeyCode::F6},
			{SDL_SCANCODE_F7, KeyCode::F7},
			{SDL_SCANCODE_F8, KeyCode::F8},
			{SDL_SCANCODE_F9, KeyCode::F9},
			{SDL_SCANCODE_F10, KeyCode::F10},
			{SDL_SCANCODE_F11, KeyCode::F11},
			{SDL_SCANCODE_F12, KeyCode::F12},

			// KEYPAD / NUMPAD
			{SDL_SCANCODE_KP_0, KeyCode::NumPad0},
			{SDL_SCANCODE_KP_1, KeyCode::NumPad1},
			{SDL_SCANCODE_KP_2, KeyCode::NumPad2},
			{SDL_SCANCODE_KP_3, KeyCode::NumPad3},
			{SDL_SCANCODE_KP_4, KeyCode::NumPad4},
			{SDL_SCANCODE_KP_5, KeyCode::NumPad5},
			{SDL_SCANCODE_KP_6, KeyCode::NumPad6},
			{SDL_SCANCODE_KP_7, KeyCode::NumPad7},
			{SDL_SCANCODE_KP_8, KeyCode::NumPad8},
			{SDL_SCANCODE_KP_9, KeyCode::NumPad9},
			{SDL_SCANCODE_KP_DECIMAL, KeyCode::NumPadDecimal},
			{SDL_SCANCODE_KP_DIVIDE, KeyCode::NumPadDivide},
			{SDL_SCANCODE_KP_MULTIPLY, KeyCode::NumPadMultiply},
			{SDL_SCANCODE_KP_MINUS, KeyCode::NumPadSubtract},
			{SDL_SCANCODE_KP_PLUS, KeyCode::NumPadAdd},
			{SDL_SCANCODE_KP_ENTER, KeyCode::NumPadEnter},

			// AUDIO
			{ SDL_SCANCODE_AUDIOPLAY, KeyCode::AudioPlay },
			{ SDL_SCANCODE_AUDIOSTOP, KeyCode::AudioStop },
			{ SDL_SCANCODE_AUDIONEXT, KeyCode::AudioNext },
			{ SDL_SCANCODE_AUDIOPREV, KeyCode::AudioPrev },

			// OTHER
			{SDL_SCANCODE_SLASH, KeyCode::Slash},
			{SDL_SCANCODE_BACKSLASH, KeyCode::BackSlash},
			{SDL_SCANCODE_APOSTROPHE, KeyCode::Apostrophe},
			{SDL_SCANCODE_COMMA, KeyCode::Comma},
			{SDL_SCANCODE_PERIOD, KeyCode::Period},
			{SDL_SCANCODE_MINUS, KeyCode::Minus},
			{SDL_SCANCODE_EQUALS, KeyCode::Equal},
			{SDL_SCANCODE_GRAVE, KeyCode::GraveAccent},

			// MODIFIERS
			{SDL_SCANCODE_LSHIFT, KeyCode::ShiftLeft},
			{SDL_SCANCODE_LCTRL, KeyCode::ControlLeft},
			{SDL_SCANCODE_LALT, KeyCode::AltLeft},
			{SDL_SCANCODE_LGUI, KeyCode::SystemLeft},
			{SDL_SCANCODE_RSHIFT, KeyCode::ShiftRight},
			{SDL_SCANCODE_RCTRL, KeyCode::ControlRight},
			{SDL_SCANCODE_RALT, KeyCode::AltRight},
			{SDL_SCANCODE_RGUI, KeyCode::SystemRight},
	};

	// SDL2MouseDevice ///////////////////////////////////////////////////////////
	Bool SDL2MouseDevice::button(uint8_t button) const
	{
		return button < _buttons.size() ? _buttons[button] : false;
	}

	void SDL2MouseDevice::reset()
	{
		_buttons.reset();
		_position = VectorConst2i::Zero;
		_wheel = VectorConst2i::Zero;
	}

	void SDL2MouseDevice::update()
	{
		const auto mouse_buttons = SDL_GetMouseState(&_position.x, &_position.y);
		for (unsigned i = 0; i < _buttons.size(); i++)
			_buttons[i] = (mouse_buttons & SDL_BUTTON(i + 1)) != 0;
	}

	// SDL2KeyboardDevice ////////////////////////////////////////////////////////
	Bool SDL2KeyboardDevice::key(KeyCode code) const
	{
		const int index = static_cast<int>(code);
		return _state[index];
	}

	KeyModFlags SDL2KeyboardDevice::mods() const
	{
		return _mods;
	}

	void SDL2KeyboardDevice::reset()
	{
		_state.reset();
		_mods = KeyModFlags::Zero;
	}

	void SDL2KeyboardDevice::update()
	{
		// Keyboard State
		_state.reset();
		int keys_num;
		const auto key_state = SDL_GetKeyboardState(&keys_num);
		for (auto i = 0; i < keys_num; i++)
		{
			if (key_state[i] == 1)
			{
				const auto scancode = static_cast<SDL_Scancode>(i);
				const auto it = s_key_bindings.find(scancode);
				if (it != s_key_bindings.end())
					_state[static_cast<int>(it->second)] = true;
				//else
				//{
				//	const auto key_name = SDL_GetScancodeName(scancode);
				//	UC_LOG_WARNING(_logger) << "Unknown scancode " << key_name << ":" << scancode;
				//}
			}
		}

		// Keyboard Modifiers
		const auto key_mod = SDL_GetModState();
		_mods.set(KeyMod::ShiftLeft, key_mod & KMOD_LSHIFT);
		_mods.set(KeyMod::ShiftRight, key_mod & KMOD_LSHIFT);

		_mods.set(KeyMod::AltLeft, key_mod & KMOD_LALT);
		_mods.set(KeyMod::AltRight, key_mod & KMOD_RALT);

		_mods.set(KeyMod::ControlLeft, key_mod & KMOD_LCTRL);
		_mods.set(KeyMod::ControlRight, key_mod & KMOD_RCTRL);

		_mods.set(KeyMod::SystemLeft, key_mod & KMOD_LGUI);
		_mods.set(KeyMod::SystemRight, key_mod & KMOD_RGUI);
	}

	// SDL2Input //////////////////////////////////////////////////////////////////
	SDL2Input::SDL2Input(Logger& logger)
		: _logger(logger)
		, _mouse_state(_mouse)
		, _keyboard_state(_keyboard)
	{
		SDL_InitSubSystem(SDL_INIT_EVENTS);
	}

	const MouseDeviceState& SDL2Input::mouse() const
	{
		return _mouse_state;
	}

	const KeyboardDeviceState& SDL2Input::keyboard() const
	{
		return _keyboard_state;
	}

	void SDL2Input::reset()
	{
		_mouse.reset();
		_keyboard.reset();

		_mouse_state.reset();
		_keyboard_state.reset();
	}

	void SDL2Input::update()
	{
		_mouse.update();
		_keyboard.update();

		_mouse_state.update();
		_keyboard_state.update();
	}
}
#endif