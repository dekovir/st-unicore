#pragma once
#include "unicore/math/Rect.hpp"
#include "unicore/io/File.hpp"
#if defined(UNICORE_USE_SDL2)
#include <SDL.h>

namespace unicore
{
	template<typename T>
	class SDLValue
	{
	public:
		virtual ~SDLValue() = default;

		T* handle() const { return _handle;  }
	protected:
		T* _handle;

		explicit SDLValue(T* handle) : _handle(handle) {}
	};

	class SDL2EventListener
	{
	public:
		virtual ~SDL2EventListener() = default;
		virtual bool on_event(const SDL_Event& evt) = 0;
	};

	namespace SDL2Utils
	{
		extern bool get_desktop_native_mode(SDL_DisplayMode& mode);

		extern SDL_RWops* from_stream(ReadFile& stream);
		extern SDL_RWops* from_stream(const Shared<ReadFile>& stream);

		extern SDL_RWops* from_stream(const Shared<WriteFile>& stream);

		static SDL_Rect& convert(const Recti& src, SDL_Rect& dst)
		{
			dst = { src.pos.x, src.pos.y, src.size.x, src.size.y };
			return dst;
		}

		static Recti& convert(const SDL_Rect& src, Recti& dst)
		{
			dst = { src.x, src.y, src.w, src.h };
			return dst;
		}

		static SDL_FRect& convert(const Rectf& src, SDL_FRect& dst)
		{
			dst = { src.pos.x, src.pos.y, src.size.x, src.size.y };
			return dst;
		}

		static SDL_Point& convert(const Vector2i& src, SDL_Point& dst)
		{
			dst = { src.x, src.y };
			return dst;
		}

		static SDL_FPoint& convert(const Vector2f& src, SDL_FPoint& dst)
		{
			dst = { src.x, src.y };
			return dst;
		}

		static void convert(const Vector2i* src, size_t count, List<SDL_Point>& dst)
		{
			dst.resize(count);
			for (size_t i = 0; i < count; i++)
				convert(src[i], dst[i]);
		}

		static void convert(const Vector2f* src, size_t count, List<SDL_FPoint>& dst)
		{
			dst.resize(count);
			for (size_t i = 0; i < count; i++)
				convert(src[i], dst[i]);
		}

		static void convert(const Recti* src, size_t count, List<SDL_Rect>& dst)
		{
			dst.resize(count);
			for (size_t i = 0; i < count; i++)
				convert(src[i], dst[i]);
		}

		static void convert(const Rectf* src, size_t count, List<SDL_FRect>& dst)
		{
			dst.resize(count);
			for (size_t i = 0; i < count; i++)
				convert(src[i], dst[i]);
		}
	}

	extern UNICODE_STRING_BUILDER_FORMAT(const SDL_version&);
	extern UNICODE_STRING_BUILDER_FORMAT(const SDL_DisplayOrientation&);

	extern UNICODE_STRING_BUILDER_FORMAT(const SDL_Point&);
	extern UNICODE_STRING_BUILDER_FORMAT(const SDL_FPoint&);

	extern UNICODE_STRING_BUILDER_FORMAT(const SDL_Rect&);
	extern UNICODE_STRING_BUILDER_FORMAT(const SDL_FRect&);
}

#endif