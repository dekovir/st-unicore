#pragma once
#include "unicore/system/Object.hpp"
#include "unicore/system/EnumFlag.hpp"
#include "unicore/math/Vector2.hpp"

namespace unicore
{
	class Logger;

	//enum class DisplayState : uint8_t
	//{
	//	Visible = 1 << 0,
	//	Focused = 1 << 1,
	//};
	//UNICORE_ENUM_FLAGS(DisplayState, DisplayStateFlags);

	enum class DisplayWindowFlag : uint8_t
	{
		Resizable = 1 << 0,
		Borderless = 1 << 1,
	};
	UNICORE_ENUM_FLAGS(DisplayWindowFlag, DisplayWindowFlags);

	struct DisplayMode
	{
		bool fullscreen;
		Vector2i window_size = VectorConst2i::Zero;
		DisplayWindowFlags window_flags = DisplayWindowFlags::Zero;
	};

	struct DisplaySettings
	{
		Logger& logger;
		StringView title;
		DisplayMode mode;
	};

	class Display : public Object
	{
		UC_OBJECT(Display, Object)
	public:
		UC_NODISCARD virtual const DisplayMode& get_mode() const = 0;
		virtual void set_mode(const DisplayMode& mode) = 0;

		UC_NODISCARD virtual Vector2i get_maximum_size() const = 0;
		UC_NODISCARD virtual Vector2i get_minimum_size() const = 0;

		UC_NODISCARD virtual void* native_handle() const = 0;

		UC_NODISCARD const Vector2i& size() const { return get_mode().window_size; }
	};
}