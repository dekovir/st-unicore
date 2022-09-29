#pragma once
#include "unicore/Buffer2.hpp"

namespace unicore
{
	template<typename T>
	class Canvas
	{
	public:
		using CallbackFunction = Function<bool, Canvas<T>&, const Vector2i&>;

		using ValueType = const T&;
		using CallbackType = const CallbackFunction&;

		explicit Canvas(IBuffer2<T>& buffer)
			: _buffer(buffer)
		{
		}

		UC_NODISCARD IBuffer2<T>& buffer() { return _buffer; }
		UC_NODISCARD const IBuffer2<T>& buffer() const { return _buffer; }
		UC_NODISCARD const Vector2i& size() const { return _buffer.size(); }

		// POINT /////////////////////////////////////////////////////////////////////
		void draw_point(const Vector2i& pos, ValueType value)
		{
			_buffer.set(pos.x, pos.y, value);
		}

		// STRAIGHT LINE /////////////////////////////////////////////////////////////
		void draw_line_h(const Vector2i& pos, unsigned length, ValueType value)
		{
			// TODO: Optimize with linear memory access
			for (unsigned i = 0; i < length; i++)
				_buffer.set(pos.x + i, pos.y, value);
		}

		bool draw_line_h(const Vector2i& pos, unsigned length, CallbackType func)
		{
			// TODO: Optimize with linear memory access
			for (unsigned i = 0; i < length; i++)
			{
				if (!func(*this, Vector2i(pos.x + i, pos.y)))
					return false;
			}

			return true;
		}

		void draw_line_v(const Vector2i& pos, unsigned length, ValueType value)
		{
			for (unsigned i = 0; i < length; i++)
				_buffer.set(pos.x, pos.y + i, value);
		}

		bool draw_line_v(const Vector2i& pos, unsigned length, CallbackType func)
		{
			for (unsigned i = 0; i < length; i++)
			{
				if (!func(*this, Vector2i(pos.x, pos.y + i)))
					return false;
			}

			return true;
		}

		// RECT //////////////////////////////////////////////////////////////////////
		void draw_rect(const Recti& rect, ValueType value)
		{
			draw_line_h(rect.x, rect.y, rect.w, value);
			draw_line_h(rect.x, rect.y + rect.h, rect.w, value);

			draw_line_v(rect.x, rect.y + 1, rect.h - 2, value);
			draw_line_v(rect.x + rect.w, rect.y + 1, rect.h - 2, value);
		}

		bool draw_rect(const Recti& rect, CallbackType func)
		{
			return
				draw_line_h(rect.x, rect.y, rect.w, func) &&
				draw_line_h(rect.x, rect.y + rect.h, rect.w, func) &&
				draw_line_v(rect.x, rect.y + 1, rect.h - 2, func) &&
				draw_line_v(rect.x + rect.w, rect.y + 1, rect.h - 2, func);
		}

		void fill_rect(const Recti& rect, ValueType value)
		{
			for (unsigned i = 0; i < rect.h; i++)
				draw_line_h(Vector2i(rect.x, rect.y + i), static_cast<unsigned>(rect.w), value);
		}

		bool fill_rect(const Recti& rect, CallbackType func)
		{
			for (unsigned i = 0; i < rect.h; i++)
			{
				if (!draw_line_h(Vector2i(rect.x, rect.y + i), static_cast<unsigned>(rect.w), func))
					return false;
			}

			return true;
		}

		void fill(ValueType value)
		{
			fill_rect(_buffer.get_rect(), value);
		}

		bool fill(CallbackType func)
		{
			return fill_rect(_buffer.get_rect(), func);
		}

		// LINE //////////////////////////////////////////////////////////////////////
		// TODO: Rewrite line render
		void draw_line(const Vector2i& p1, const Vector2i& p2, ValueType value)
		{
			int x, y;

			const auto x1 = p1.x;
			const auto y1 = p1.y;

			const auto x2 = p2.x;
			const auto y2 = p2.y;

			const int dx = x2 - x1;
			const int dy = y2 - y1;
			const int dx1 = Math::abs(dx);
			const int dy1 = Math::abs(dy);
			int px = 2 * dy1 - dx1;
			int py = 2 * dx1 - dy1;
			if (dy1 <= dx1)
			{
				int xe;
				if (dx >= 0)
				{
					x = x1;
					y = y1;
					xe = x2;
				}
				else
				{
					x = x2;
					y = y2;
					xe = x1;
				}
				_buffer.set(x, y, value);
				for (int i = 0; x < xe; i++)
				{
					x = x + 1;
					if (px < 0)
					{
						px = px + 2 * dy1;
					}
					else
					{
						if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						{
							y = y + 1;
						}
						else
						{
							y = y - 1;
						}
						px = px + 2 * (dy1 - dx1);
					}
					_buffer.set(x, y, value);
				}
			}
			else
			{
				int ye;
				if (dy >= 0)
				{
					x = x1;
					y = y1;
					ye = y2;
				}
				else
				{
					x = x2;
					y = y2;
					ye = y1;
				}
				_buffer.set(x, y, value);
				for (int i = 0; y < ye; i++)
				{
					y = y + 1;
					if (py <= 0)
					{
						py = py + 2 * dx1;
					}
					else
					{
						if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						{
							x = x + 1;
						}
						else
						{
							x = x - 1;
						}
						py = py + 2 * (dx1 - dy1);
					}
					_buffer.set(x, y, value);
				}
			}
		}

		// TODO: Replace with span
		void draw_line_poly(const Vector2i* points, unsigned num_points, ValueType value)
		{
			for (unsigned i = 1; i < num_points; i++)
				draw_line(points[i - 1], points[i], value);
		}

		// TODO: Replace with span
		bool draw_line_poly(const Vector2i* points, unsigned num_points, CallbackType func)
		{
			for (unsigned i = 1; i < num_points; i++)
			{
				if (!draw_line(points[i - 1], points[i], func))
					return false;
			}

			return true;
		}

		// CIRCE /////////////////////////////////////////////////////////////////////
		void draw_circle(const Vector2i& center, int radius, ValueType value)
		{
			int x = 0, y = radius;
			int d = 3 - 2 * radius;
			draw_circle_points(center.x, center.y, x, y, value);
			while (y >= x)
			{
				x++;
				if (d > 0)
				{
					y--;
					d = d + 4 * (x - y) + 10;
				}
				else
					d = d + 4 * x + 6;
				draw_circle_points(center.x, center.y, x, y, value);
			}
		}

		void fill_circle(const Vector2i& center, int radius, ValueType value)
		{
			// TODO: Optimize with draw_line_h
			const int N = 2 * radius + 1;
			for (int i = 0; i < N; i++)
			{
				for (int j = 0; j < N; j++)
				{
					const int x = i - radius;
					const int y = j - radius;

					if (x * x + y * y <= radius * radius + 1)
						_buffer.set(center.x + x, center.y + y, value);
				}
			}
		}

		// TODO: Implement triangle rasterization
		// https://github.com/ssloy/tinyrenderer/wiki/Lesson-2:-Triangle-rasterization-and-back-face-culling

		// COPY //////////////////////////////////////////////////////////////////////
		void draw_buffer(const Vector2i& pos, const IBuffer2<T>& src)
		{
			const auto& src_size = src.size();

			for (int y = 0; y < src_size.y; y++)
				for (int x = 0; x < src_size.x; x++)
				{
					T value;
					if (src.get(x, y, value))
						draw_point({ pos.x + x, pos.y + y }, value);
				}
		}

	protected:
		IBuffer2<T>& _buffer;

		void draw_circle_points(int xc, int yc, int x, int y, ValueType value)
		{
			_buffer.set(xc + x, yc + y, value);
			_buffer.set(xc - x, yc + y, value);
			_buffer.set(xc + x, yc - y, value);
			_buffer.set(xc - x, yc - y, value);
			_buffer.set(xc + y, yc + x, value);
			_buffer.set(xc - y, yc + x, value);
			_buffer.set(xc + y, yc - x, value);
			_buffer.set(xc - y, yc - x, value);
		}
	};
}