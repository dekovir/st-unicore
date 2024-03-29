#pragma once
#include "unicore/system/Object.hpp"
#include "unicore/renderer/sdl2/Pipeline.hpp"
#include "unicore/imgui/ImGuiDefs.hpp"

namespace unicore
{
	class Logger;
	class Texture;
	class Renderer;

	class ImGuiRender : public Object
	{
		UC_OBJECT(ImGuiRender, Object)
	public:
		explicit ImGuiRender(Logger& logger);

		void init(Renderer& render);

		virtual void begin_frame(ImGui::IO& io) = 0;
		virtual void render(const ImDrawData* data) = 0;

	protected:
		Logger& _logger;
		Shared<Texture> _font_texture;
	};

	class ImGuiRender2D : public ImGuiRender
	{
		UC_OBJECT(ImGuiRender2D, Object)
	public:
		ImGuiRender2D(sdl2::Pipeline& render, Logger& logger);

		void begin_frame(ImGui::IO& io) override;

		void render(const ImDrawData* data) override;

	protected:
		sdl2::Pipeline& _render;

		void setup_render_state();
	};
}