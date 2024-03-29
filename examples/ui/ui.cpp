#include "ui.hpp"
#include "UnicoreMain.hpp"
#include "InitPlugins.hpp"
#include "unicore/io/Logger.hpp"
#include "unicore/math/Random.hpp"
#include "unicore/platform/Input.hpp"
#include "unicore/remoteui/DocumentParseXML.hpp"

namespace unicore
{
	constexpr Vector2i WindowSize = Vector2i(1280, 800);
	constexpr DisplayWindowFlags WindowFlags = DisplayWindowFlag::Resizable;

	MyApp::MyApp(const CoreSettings& settings)
		: SDLApplication(create_settings(settings, "UI", { false, WindowSize, WindowFlags }))
		, _context_logger("[UI] ", logger)
		, _contex_render(renderer, _context_logger)
		, _context(_contex_render, time, input, _context_logger)
	{
		init_plugins(*this);
	}

	void MyApp::on_init()
	{
		_contex_render.init(renderer);

		_database = std::make_shared<ItemDatabase>();

		// GENERATE ITEMS
		_sprites = resources.load<SpriteList>("items.png"_path, TileSetOptions({ 16, 16 }));
		if (_sprites && _sprites->size() == 64)
		{
			_database->add(Item::make_weapon(U"Staff", 50, 1500, { 5, 10 }, _sprites->get(33)));
			_database->add(Item::make_weapon(U"Dagger", 150, 1000, { 2, 5 }, _sprites->get(1)));
			_database->add(Item::make_weapon(U"Sword", 350, 2000, { 7, 14 }, _sprites->get(2)));
			_database->add(Item::make_weapon(U"Crossbow", 500, 4000, { 5, 12 }, _sprites->get(48)));
			_database->add(Item::make_weapon(U"Spear", 150, 5000, { 8,  16 }, _sprites->get(50)));
			_database->add(Item::make_shield(U"Shield", 250, 1500, 5, _sprites->get(43)));
			_database->add(Item::make_consumable(U"Potion of Mana", 75, 150, _sprites->get(3)));
			_database->add(Item::make_consumable(U"Potion of Health", 50, 150, _sprites->get(4)));
			_database->add(Item::make_consumable(U"Potion of Stamina", 25, 150, _sprites->get(5)));
			_database->add(Item::make_armor(U"Cloak", 100, 300, 1, _sprites->get(52)));
			_database->add(Item::make_armor(U"Brass Plate", 500, 15000, 15, _sprites->get(8)));
			_database->add(Item::make_armor(U"Steel Plate", 1000, 13000, 25, _sprites->get(9)));
			_database->add(Item::make_armor(U"Dwarven Plate", 5000, 14000, 40, _sprites->get(10)));
			_database->add(Item::make_accessory(U"Necklace", 100, 300, _sprites->get(14)));
			_database->add(Item::make_accessory(U"Magic orb", 500, 1100, _sprites->get(36)));
			_database->add(Item::make_accessory(U"Ring of Health", 1500, 100, _sprites->get(38)));
			_database->add(Item::make_accessory(U"Ring of Mana", 1500, 100, _sprites->get(39)));
		}
		else
		{
			DefaultRandom random;
			for (unsigned i = 0; i < 5; i++)
			{
				Item item;
				item.item_type = ItemType::Accessory;
				item.price = random.range<UInt16>(100, 900);
				item.weight = random.range<UInt16>(100, 15000);
				item.title = StringBuilder::format(U"Item {}", i + 1);
				_database->add(item);
			}
		}

		_inventory = std::make_shared<Inventory>(*_database);

		// Database UI
#if 1
		_database_doc = resources.load<remoteui::Document>("database.xml"_path, LoggerOption{ _context_logger });
		if (_database_doc)
		{
			_database_view = std::make_shared<remoteui::ViewImGui>(_context, _context_logger);
			_database_view->set_document(_database_doc);
			_database_view->set_title(U"Database");
			_database_view->set_size({ 500, 0 });
			_database_view->set_position({ 10, 10 });

			_database_ui = std::make_shared<ItemDatabaseUI>(_database, _sprites, *_database_doc, &_context_logger);
		}
#endif

		// Inventory UI
#if 1
		_inventory_doc = resources.load<remoteui::Document>("inventory.xml"_path, LoggerOption{ _context_logger });
		if (_inventory_doc)
		{
			_inventory_view = std::make_shared<remoteui::ViewImGui>(_context, _context_logger);
			_inventory_view->set_document(_inventory_doc);
			_inventory_view->set_title(U"Inventory");
			_inventory_view->set_position(Vector2f(10, 300));

			_inventory_ui = std::make_shared<InventoryUI>(
				*_inventory, *_inventory_doc, &_context_logger);

			for (const auto& it : *_database)
			{
				const auto item = _inventory->database().get(it.first);
				if (item->is_stackable())
					_inventory->add_item(it.first, 10);
				else if (item->has_status())
					_inventory->add_item(it.first, 200);
				else _inventory->add_item(it.first);
			}
		}
#endif
	}

	void MyApp::on_update()
	{
#if !defined(UNICORE_PLATFORM_WEB)
		if (input.keyboard().down(KeyCode::Escape))
			platform.looper.quit();

		if (
			input.keyboard().down_changed(KeyCode::Enter) &&
			input.keyboard().mods(KeyModCombine::Alt))
		{
			auto mode = display.get_mode();
			mode.fullscreen = !mode.fullscreen;
			mode.window_size = WindowSize;
			mode.window_flags = WindowFlags;
			display.set_mode(mode);
		}
#endif

		// ImGui //////////////////////////////////////////////////////////////////
		_context.frame_begin();

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		static bool show_demo_window = false;
		ImGui::ShowDemoWindow(&show_demo_window);
#endif

		if (_database_view)
			_database_view->render();

		if (_inventory_view)
			_inventory_view->render();

		_context.frame_end();
	}

	void MyApp::on_draw()
	{
		renderer.clear(ColorConst4b::DarkSlateGray);

		_context.render();
	}

	UNICORE_MAIN_CORE(MyApp);
}