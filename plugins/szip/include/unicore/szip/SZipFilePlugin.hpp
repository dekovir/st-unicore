#pragma once
#include "unicore/Plugin.hpp"

namespace unicore
{
	class SZipFilePlugin : public Plugin
	{
		UC_OBJECT(SZipFilePlugin, Plugin)
	public:
		void register_plugin(const ModuleContext& context) override;
	};
}