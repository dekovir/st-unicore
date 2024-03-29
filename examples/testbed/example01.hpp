#pragma once
#include "example.hpp"
#include "unicore/renderer/PrimitiveBatch.hpp"

namespace unicore
{
	class Example01 : public Example
	{
	public:
		explicit Example01(const ExampleContext& context)
			: Example(context)
		{
		}

		void load(IResourceCache& resources) override;
		void update() override;
		void draw() const override;

	protected:
		Shared<GeometryFont> _font;
		PrimitiveBatch _graphics;
		Radians _angle;
	};
}