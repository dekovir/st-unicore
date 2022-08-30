#pragma once
#include "unicore/Utility.hpp"
#include "unicore/Path.hpp"

namespace unicore
{
	template<typename T>
	class DataView
	{
	public:
		constexpr DataView()
			: _data(nullptr), _size(0) {}

		constexpr DataView(T* data, size_t size) noexcept
			: _data(data), _size(size) {}

		UC_NODISCARD constexpr size_t size() const { return _size; }
		UC_NODISCARD constexpr size_t size_bytes() const { return _size * sizeof(T); }
		UC_NODISCARD constexpr bool empty() const { return _size == 0; }

		UC_NODISCARD constexpr T* data() const { return _data; }

	protected:
		T* _data;
		size_t _size;
	};

	// ARRAY //////////////////////////////////////////////////////////////////////
	namespace ArrayHelper
	{
		template<typename... Ts>
		constexpr auto make(Ts&&... ts) noexcept
		{
			using CommonType = std::common_type_t<Ts...>;
			return std::array<CommonType, sizeof...(Ts)>{std::forward<CommonType>(ts)...};
		}

		template <typename T, std::size_t N1, std::size_t N2>
		constexpr std::array<T, N1 + N2> concat(std::array<T, N1> lhs, std::array<T, N2> rhs)
		{
			std::array<T, N1 + N2> result{};
			std::size_t index = 0;

			for (auto& el : lhs) {
				result[index] = std::move(el);
				++index;
			}
			for (auto& el : rhs) {
				result[index] = std::move(el);
				++index;
			}

			return result;
		}
	}

	template <typename T, size_t N, size_t M>
	static constexpr auto operator+(const Array<T, N>& a, const Array<T, M>& b) noexcept
	{
		return ArrayHelper::concat(a, b);
	}

	// TUPLE //////////////////////////////////////////////////////////////////////
	namespace TupleHelper
	{
		namespace internal
		{
			template <int Start, typename... T, std::size_t... I>
			static constexpr auto sub(const std::tuple<T...>& t, std::index_sequence<I...>) {
				return std::make_tuple(std::get<Start + I>(t)...);
			}
		}

		template<size_t ... Ids, typename... T>
		static constexpr auto sub_sequence(const std::tuple<T...>& tuple)
		{
			return std::make_tuple(std::get<Ids>(tuple)...);
		}

		template <int Count, int Start = 0, typename... T>
		static constexpr auto sub(const std::tuple<T...>& t) {
			return internal::sub<Start>(t, std::make_index_sequence<Count>());
		}

		template<typename ... Ts, typename Tuple>
		constexpr auto extract(Tuple tuple)
		{
			auto get_element = [](auto el) {
				if constexpr ((std::is_base_of_v<decltype(el), Ts> || ...)) {
					return std::make_tuple(std::move(el));
				}
				else {
					return std::make_tuple();
				}
			};
			return std::apply([&](auto ... args) {
				return std::tuple_cat(get_element(std::move(args)) ...); }, std::move(tuple));
		}
	}

	//
	template<typename DataType>
	class CachedPathData
	{
	public:
		//using DataType = intptr_t;
		//virtual ~CachedPathData() = default;

		struct EntryData
		{
			Dictionary<WString, DataType> files;
			Dictionary<WString, EntryData> subfolders;
		};

		EntryData root;

		UC_NODISCARD const EntryData* find_entry(const Path& path) const
		{
			_elements.clear();
			path.explode(_elements);

			auto data = &root;
			for (const auto& _element : _elements)
			{
				const auto it = data->subfolders.find(_element);
				if (it != data->subfolders.end())
					data = &it->second;

				return nullptr;
			}

			return nullptr;
		}

		UC_NODISCARD const DataType* find_data(const Path& path) const
		{
			path.explode(_parent, _file);

			const auto entry = find_entry(_parent);
			if (entry != nullptr)
			{
				const auto it = entry->files.find(_file);
				if (it != entry->files.end())
					return &it->second;
			}

			return nullptr;
		}

		EntryData* make_entry(const Path& path)
		{
			_elements.clear();
			path.explode(_elements);

			EntryData* data = &root;
			for (auto& _element : _elements)
			{
				auto it = data->subfolders.find(_element);
				if (it != data->subfolders.end())
					data = &it->second;

				data = &data->subfolders[_element];
			}

			return data;
		}

		void add_entry(const Path& path, const DataType& data)
		{
			path.explode(_parent, _file);

			const auto entry = make_entry(_parent);
			entry->files.emplace(_file, data);
		}

	protected:
		static List<WString> _elements;
		static Path _parent;
		static WString _file;
	};

	template<typename DataType>
	List<WString> CachedPathData<DataType>::_elements;

	template<typename DataType>
	Path CachedPathData<DataType>::_parent;

	template<typename DataType>
	WString CachedPathData<DataType>::_file;
}