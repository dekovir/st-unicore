#pragma once
#include "unicore/Render2D.hpp"

namespace unicore
{
	class Graphics2D
	{
	public:
		Graphics2D() = default;

		Vector2f offset = VectorConst2f::Zero;

		void begin();
		void end();

		void clear();
		void render(Render2D& render) const;

		Graphics2D& set_color(const Color4b& color);

		Graphics2D& draw_point(const Vector2i& p);
		Graphics2D& draw_point(const Vector2f& p);

		Graphics2D& draw_line(const Vector2i& p0, const Vector2i& p1);
		Graphics2D& draw_line(const Vector2f& p0, const Vector2f& p1);

		Graphics2D& draw_poly_line(const List<Vector2f>& points, bool closed = false);

		Graphics2D& draw_rect(const Recti& rect, bool filled = false);
		Graphics2D& draw_rect(const Rectf& rect, bool filled = false);

	protected:
		enum class BatchType
		{
			Point,
			Line,
			Rect,
			RectFilled,
		};

		struct Batch
		{
			Color4b color = ColorConst4b::White;
			BatchType type = BatchType::Point;
			size_t start = 0;
			size_t count = 0;
		};

		List<Vector2f> _points;
		List<Batch> _batches;
		Batch _current;

		void set_type(BatchType type);

		void flush();
	};
}