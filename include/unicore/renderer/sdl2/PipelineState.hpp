#pragma once
#include "unicore/math/Rect.hpp"
#include "unicore/renderer/Color4.hpp"

namespace unicore::sdl2
{
	class PipelineState
	{
	public:
		virtual ~PipelineState() = default;

		virtual void set_viewport(const Optional<Recti>& rect) = 0;
		UC_NODISCARD virtual const Optional<Recti>& get_viewport() const = 0;

		virtual void set_scale(const Vector2f& value) = 0;
		UC_NODISCARD virtual const Vector2f& get_scale() const = 0;

		virtual void set_logical_size(const Vector2i& size) = 0;
		UC_NODISCARD virtual const Vector2i& get_logical_size() const = 0;

		virtual void set_clip(const Optional<Recti>& rect) = 0;
		UC_NODISCARD virtual const Optional<Recti>& get_clip() const = 0;
	};
}