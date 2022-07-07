#pragma once
#include "unicore/Plugin.hpp"

namespace unicore
{
	class FNTPlugin : public Plugin
	{
	public:
		void register_module(Context& context) override;
		void unregister_module(Context& context) override;
	};
}