#pragma once
#include "unicore/platform/Module.hpp"
#include "unicore/platform/MouseDevice.hpp"
#include "unicore/platform/KeyboardDevice.hpp"

namespace unicore
{
	class Input : public Module
	{
		UC_OBJECT(Input, Module)
	public:
		UC_NODISCARD virtual const MouseDeviceState& mouse() const = 0;
		UC_NODISCARD virtual const KeyboardDeviceState& keyboard() const = 0;
	};
}