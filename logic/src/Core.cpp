#include "unicore/app/Core.hpp"
#include "unicore/Time.hpp"
#include "unicore/Input.hpp"
#include "unicore/TimeSpan.hpp"

#if defined(UNICORE_USE_XML)
#	include "unicore/xml/XMLPlugin.hpp"
#endif

#if defined(UNICORE_USE_FNT)
#	include "unicore/fnt/FNTPlugin.hpp"
#endif

namespace unicore
{
	Core::Core(const CoreSettings& settings)
		: platform(settings.platform)
		, logger(platform.logger)
		, time(platform.time)
		, input(platform.input)
		, resources(platform.resources)
	{
		platform.register_module(context);
		input.register_module(context);
		resources.register_module(context);

#if defined(UNICORE_USE_XML)
		_plugins.push_back(std::make_unique<XMLPlugin>());
#endif

#if defined(UNICORE_USE_FNT)
		_plugins.push_back(std::make_unique<FNTPlugin>());
#endif

		for (const auto& plugin : _plugins)
			plugin->register_module(context);
	}

	Core::~Core()
	{
		for (const auto& plugin : _plugins)
			plugin->unregister_module(context);

		resources.unload_all();
		resources.clear();

		resources.unregister_module(context);
		platform.unregister_module(context);
		input.unregister_module(context);
	}

	void Core::update()
	{
		platform.poll_events();

		if (time.delta() > TimeSpanConst::Zero)
			on_update();
	}
}