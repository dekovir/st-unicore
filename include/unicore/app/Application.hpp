#pragma once
#include "unicore/platform/Platform.hpp"

namespace unicore
{
	class Plugin;

	struct CoreSettings
	{
		Platform& platform;
	};

	class Application
	{
		ProxyLogger _logger;
	public:
		explicit Application(const CoreSettings& settings);
		virtual ~Application();

		Platform& platform;
		Logger& logger;
		Time& time;
		Input& input;
		FileSystem& file_system;
		ResourceCache& resources;

		virtual void init();
		virtual void update();

		void add_plugin(Unique<Plugin>&& plugin);

		template<typename TPlugin,
			std::enable_if_t<std::is_base_of_v<Plugin, TPlugin>>* = nullptr>
		void create_plugin()
		{
			add_plugin(std::make_unique<TPlugin>());
		}

	protected:
		virtual void on_init() = 0;
		virtual void on_update() = 0;

		virtual void on_drop_file(const Path& path) {}

		ModuleContainer _modules;

	private:
		List<Unique<Plugin>> _plugins;
	};
}