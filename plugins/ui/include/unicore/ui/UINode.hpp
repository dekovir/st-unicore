#pragma once

#include "unicore/system/Index.hpp"
#include "unicore/math/Rect.hpp"

namespace unicore
{
	struct UINodeIndexTag {};
	using UINodeIndex = Index<UInt32, UINodeIndexTag>;
	static constexpr auto UINodeIndexInvalid = UINodeIndex(std::numeric_limits<UInt32>::max());

	using UIAttributeValue = Variant<
		String, String32,
		Int, Int64,
		Float, Double,
		Vector2i, Vector2f, Recti, Rectf>;

	enum class UIAttributeType
	{
		Uid,
		Name,
		//Style,
		Value,
		MinValue,
		MaxValue,
	};

	using UIAttributes = Dictionary<UIAttributeType, UIAttributeValue>;

	using UIAction = std::function<void()>;

	enum class UIActionType
	{
		OnLoad,
		OnClick,
		OnChange,
	};
	using UINodeActions = Dictionary<UIActionType, UIAction>;

	enum class UINodeType
	{
		None,
		Window,
		Group,
		Text,
		Button,
		Input,
		Slider,
	};

	class UIDocument;

	class UINode
	{
	public:
		UINode(UIDocument& document, const UINodeIndex& index);

		UC_NODISCARD UIDocument& document() const { return _document; }
		UC_NODISCARD UINodeIndex index() const { return _index; }

		UC_NODISCARD UINodeType type() const;
		UC_NODISCARD Optional<UINode> parent() const;

		UC_NODISCARD const UIAttributes& attributes() const;
		UC_NODISCARD const UINodeActions& actions() const;

		void set_attribute(UIAttributeType type, const Optional<UIAttributeValue>& value);
		UC_NODISCARD Optional<UIAttributeValue> get_attribute(UIAttributeType type) const;

		void set_action(UIActionType type, const Optional<UIAction>& value);
		UC_NODISCARD Optional<UIAction> get_action(UIActionType type) const;

		size_t get_children(List<UINode>& children) const;
		UC_NODISCARD List<UINode> get_children() const;

		UC_NODISCARD Optional<UINode> get_next_sibling() const;
		UC_NODISCARD Optional<UINode> get_prev_sibling() const;

		UINode create_node(UINodeType type);

		// GET ATTRIBUTE /////////////////////////////////////////////////////////////
		UC_NODISCARD bool try_get_float(UIAttributeType type, Float& value) const;
		UC_NODISCARD Float get_float(UIAttributeType type, Float default_value = 0) const;

		UC_NODISCARD bool try_get_double(UIAttributeType type, Double& value) const;
		UC_NODISCARD Double get_double(UIAttributeType type, Double default_value = 0) const;

		UC_NODISCARD bool try_get_string(UIAttributeType type, String& value) const;
		UC_NODISCARD String get_string(UIAttributeType type, StringView default_value = "") const;

		UC_NODISCARD bool try_get_string32(UIAttributeType type, String32& value) const;
		UC_NODISCARD String32 get_string32(UIAttributeType type, StringView32 default_value = U"") const;

	private:
		UIDocument& _document;
		UINodeIndex _index;
	};

	extern UNICODE_STRING_BUILDER_FORMAT(const UINodeIndex&);
}