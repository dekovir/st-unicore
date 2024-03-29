#pragma once
#include "unicore/platform/Plugin.hpp"

namespace unicore
{
	class XMLPlugin : public Plugin
	{
		UC_OBJECT(XMLPlugin, Plugin)
	public:
		XMLPlugin() = default;

		void register_plugin(const ModuleContext& context) override;
	};
}