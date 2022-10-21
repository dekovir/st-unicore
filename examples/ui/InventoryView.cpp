#include "InventoryView.hpp"
#include "unicore/system/StringHelper.hpp"
#include "unicore/io/Logger.hpp"
#include <imgui_stdlib.h>

namespace unicore
{
	UIViewImGui::UIViewImGui(ImGuiContext& context, Logger& logger)
		: _logger(logger)
		, _context(context)
	{
	}

	void UIViewImGui::update()
	{
		if (!_document) return;

		_update_events.clear();

		for (auto& node : _document->get_root_nodes())
			render_node(node);

		for (const auto& evt : _update_events)
			_document->send_event(evt);
	}

	void UIViewImGui::on_create_node(const UINode& node)
	{
		UC_LOG_DEBUG(_logger) << "create_node " << node.index();

		//if (node.type() == UINodeType::Input)
		//	_input_buffers[node.index()] = "";
	}

	void UIViewImGui::on_set_attribute(const UINode& node,
		UIAttributeType type, const Optional<UIAttributeValue>& value)
	{
		UC_LOG_DEBUG(_logger) << "set_attribute " << node.index();

		//if (node.type() == UINodeType::Input && type == UIAttributeType::Value)
		//{
		//	_input_buffers[node.index()] =
		//		value.has_value() ? std::get<String>(value.value()) : "";
		//}
	}

	void UIViewImGui::on_set_action(const UINode& node,
		UIActionType type, const Optional<UIAction>& value)
	{
		UC_LOG_DEBUG(_logger) << "set_action " << node.index();
	}

	void UIViewImGui::render_node(const UINode& node)
	{
		const auto type = node.type();

		List<UINode> children;
		node.get_children(children);

		const auto id = make_id(node.index());
		String str;
		String32 str32;

		switch (type)
		{
		case UINodeType::None: return;

		case UINodeType::Window:
			str = node.get_string(UIAttributeType::Value, "Window");
			if (ImGui::Begin(str.c_str()))
			{
				for (const auto& child : children)
					render_node(child);
				ImGui::End();
			}
			break;

		case UINodeType::Group:

			if (ImGui::BeginChild(id.c_str()))
			{
				for (const auto& child : children)
					render_node(child);

				ImGui::EndChild();
			}
			break;

		case UINodeType::Text:
			if (node.try_get_string(UIAttributeType::Value, str))
			{
				ImGui::Text("%s", str.c_str());
			}
			else ImGui::Text("No text");
			break;

		case UINodeType::Button:
			str = node.get_string(UIAttributeType::Value);
			if (ImGui::Button(str.c_str()))
				_update_events.push_back({ node, UIEventType::ActionCall, UIActionType::OnClick });
			break;

		case UINodeType::Input:
		{
			str = node.get_string(UIAttributeType::Value);
			if (ImGui::InputText(id.c_str(), &str))
				_update_events.push_back({ node, UIEventType::ValueChanged, str });
		}
		break;

		case UINodeType::Slider:
		{
			const auto value_min = node.get_float(UIAttributeType::MinValue, 0);
			const auto value_max = node.get_float(UIAttributeType::MaxValue, 1);

			auto value = node.get_float(UIAttributeType::Value);

			if (ImGui::SliderFloat(id.c_str(), &value, value_min, value_max))
				_update_events.push_back({ node, UIEventType::ValueChanged, value });
		}
		break;
		}
	}

	String UIViewImGui::make_id(UINodeIndex index)
	{
		return StringBuilder::format("##{}", StringHelper::to_hex(index.value));
	}
}