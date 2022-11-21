#include "example08.hpp"
#include "unicore/io/Logger.hpp"
#include "unicore/resource/ResourceCache.hpp"
#include "unicore/imgui/ImGuiContext.hpp"
#include "unicore/ui/components/ListComponent.hpp"

namespace unicore
{
	UC_EXAMPLE_REGISTER(Example08, "UI Components");

	static const Dictionary<int, String32> items = {
			{0, U"Item 1"},
			{1, U"Item 2"},
			{2, U"Item 3"},
			{3, U"Item 4"},
			{4, U"Item 5"},
	};

	class TestListModel : public DataModel1<Shared<ui::Component>>
	{
	public:
		explicit TestListModel(Size count) : _count(count) {}

		UC_NODISCARD Size get_system_memory_use() const override { return sizeof(TestListModel); }
		UC_NODISCARD IndexArg size() const override { return _count; }
		UC_NODISCARD DataType get_at(IndexArg index) const override
		{
			const auto text = StringBuilder::format(U"Item {}", index + 1);
			auto item = ui::ptr(ui::item(text));

			if (index % 5 == 0)
			{
				return ui::ptr(ui::vlayout(
					ui::text(U"--- Header ---"),
					item
				));
			}

			return item;
		}

	protected:
		Size _count;
	};

	class TestTableModel : public TableDataModel<Shared<ui::Component>>
	{
	public:
		explicit TestTableModel(Size size)
			: _size(size)
		{
		}

		UC_NODISCARD Size get_system_memory_use() const override
		{
			return sizeof(TestTableModel);
		}

		UC_NODISCARD IndexArg size() const override { return _size; }

		UC_NODISCARD StringView32 get_header(Size col) const override
		{
			switch (col)
			{
			case 0: return U"Col 1";
			case 1: return U"Col 2";
			case 2: return U"Col 3";
			default: return U"Err";
			}
		}

		UC_NODISCARD std::shared_ptr<ui::Component> get_at(IndexArg index) const override
		{
			auto str = StringBuilder::format(U"Cell {} {}", index.x + 1, index.y + 1);
			if (Math::even(index.x + index.y))
				return std::make_shared<ui::TextComponent>(str);
			return std::make_shared<ui::ItemComponent>(str);
		}

	protected:
		const IndexType _size;
	};

	Example08::Example08(const ExampleContext& context)
		: Example(context)
		, _context(context.imgui)
	{
	}

	void Example08::load(IResourceCache& resources)
	{
		Example::load(resources);

		const auto size = renderer.screen_size();

		_document = std::make_shared<UIDocument>(&logger);
		_view = std::make_shared<UIViewImGui>(_context, logger);
		_view->set_document(_document);
		_view->set_title(U"Component UI");
		_view->set_size(Vector2f(300, 0));
		_view->set_position(Vector2f(size.x / 2 - 150, 50));

		Shared<ui::button> btn_ref;
		Shared<ui::combo_box<int>> combo_ref;
		Shared<ui::list_box> list_ref;
		Shared<ui::button> add_ref;

		const auto combo_model = std::make_shared<ConstDictionaryDataModel<Int, String32>>(items);
		const auto radio_model = std::make_shared<ConstDictionaryDataModel<Int, String32>>(items);
		const auto list_model = std::make_shared<TestListModel>(10);
		const auto table_model = std::make_shared<TestTableModel>(3);

		_root = ui::ptr(ui::vlayout(
			ui::ref(ui::text(U""), _position_node),
			ui::hlayout(
				ui::text(U"Text"),
				ui::input_text(U"Lorem ipsum dolor")
			),
			ui::hlayout(
				ui::text(U"Bool"),
				ui::toggle(true),
				ui::radio_button()
			),
			ui::hlayout(
				ui::text(U"Button"),
				ref(ui::button(U"Label"), btn_ref)
			),
			ui::hlayout(
				ui::text(U"Int16"),
				ui::input_int()
			),
			ui::hlayout(
				ui::text(U"UInt8"),
				ui::input_int_t<UInt8>()
			),
			ui::hlayout(
				ui::text(U"Float"),
				ui::input_float(0, 0.1f)
			),
			ui::hlayout(
				ui::text(U"IntRange"),
				ui::range_int({ 0, 100 }, 50)
			),
			ui::hlayout(
				ui::text(U"FloatRange"),
				ui::range_float({ 0, 1 }, 0.5f)
			),
			ui::hlayout(
				ui::text(U"Items"),
				ui::vlayout(
					ui::item(U"Item 1"),
					ui::item(U"Item 2"),
					ui::item(U"Item 3")
				)
			),
			ui::hlayout(
				ui::text(U"Combo"),
				ui::ref(ui::combo_box<Int>(combo_model, 0), combo_ref)
			),
			ui::hlayout(
				ui::text(U"Radio"),
				ui::radio_group<Int>(radio_model, 0)
			),
			ui::vlayout(
				ui::text(U"Items"),
				ref(ui::list_box(), list_ref),
				ref(ui::button(U"Add"), add_ref)
			),
			ui::vlayout(
				ui::text(U"Items 2"),
				ui::list(list_model)
			),
			ui::vlayout(
				ui::text(U"Table"),
				ui::table(table_model)
			)
		));

		if (btn_ref)
			btn_ref->set_click_action([btn_ref] { btn_ref->set_text(U"Clicked"); });

		if (combo_ref)
			combo_ref->on_changed() +=
			[this](auto value) { UC_LOG_DEBUG(logger) << "Combo value changed to " << value; };

		if (list_ref && add_ref)
		{
			add_ref->set_click_action(
				[this, list_ref] {
					const auto text = StringBuilder::format(U"Item {}", list_ref->size() + 1);
			list_ref->add(ui::item(text));
				});
		}

		_root->mount(*_document, UINode::Empty);
	}

	void Example08::update()
	{
		if (_document && _view && _position_node)
		{
			const auto text = StringBuilder::format(U"Pos {}, Size {}",
				_view->position().cast<Int>(), _view->size().cast<Int>());
			_position_node->set_text(text);
		}

		if (_view)
			_view->render();
	}

	void Example08::draw() const
	{
	}
}