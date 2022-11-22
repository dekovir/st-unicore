#include "unicore/ui/components/ItemComponent.hpp"

namespace unicore::ui
{
	ItemComponent::ItemComponent(StringView32 text)
		: TypedInputComponent(UIInputType::Item)
	{
		set_attribute(UIAttribute::Text, text);
	}

	ItemComponent::ItemComponent(StringView32 text, Bool value)
		: ItemComponent(text)
	{
		set_attribute(UIAttribute::Value, value);
	}

	void ItemComponent::set_text(StringView32 text)
	{
		set_attribute(UIAttribute::Text, text);
	}

	String32 ItemComponent::text() const
	{
		return get_attribute(UIAttribute::Text).get_string32();
	}

	void ItemComponent::set_click_action(const Event_clicked::ActionType& action)
	{
		_event_clicked.add(action);
	}
}