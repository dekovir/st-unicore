#pragma once
#include "unicore/system/Object.hpp"

namespace unicore
{
	enum class ResourceCachePolicy : uint8_t
	{
		CanCache,
		NoCache,
	};

	class Resource : public virtual Object
	{
		UC_OBJECT(Resource, Object)
	public:
		UC_NODISCARD virtual Size get_system_memory_use() const = 0;
		UC_NODISCARD virtual Size get_used_resources(Set<Shared<Resource>>& resources) { return 0; }

		// TODO: Replace with const value
		UC_NODISCARD virtual ResourceCachePolicy cache_policy() const { return ResourceCachePolicy::CanCache; }
	};

	class ResourceOptions
	{
	public:
		virtual ~ResourceOptions() = default;
		UC_NODISCARD virtual Size hash() const = 0;
	};

	template<Size Hash>
	class ResourceOptionsHash : public ResourceOptions
	{
	public:
		UC_NODISCARD Size hash() const override { return Hash; }
	};

	class ResourceOptionsTag : public ResourceOptions
	{
	public:
		const StringView text;
		const Size hash_value;

		explicit ResourceOptionsTag(StringView text);

		UC_NODISCARD Size hash() const override { return hash_value; }
	};

	class LoggerOption : public ResourceOptions
	{
	public:
		Logger* logger = nullptr;

		LoggerOption() = default;

		explicit LoggerOption(Logger& logger)
			: logger(&logger)
		{
		}

		UC_NODISCARD Size hash() const override;
	};

#define UNICORE_RESOURCE_OPTIONS_TAG(name) \
	static const auto name = ResourceOptionsTag(#name)

	class EmptyResourceOptions : public ResourceOptions
	{
	public:
		UC_NODISCARD Size hash() const override { return 0; }
	};

	template<typename T,
		std::enable_if_t<std::is_base_of_v<Resource, T>>* = nullptr>
	class ResourceList : public Resource
	{
		UC_OBJECT(ResourceList, Resource)
	public:
		using DataType = List<Shared<T>>;

		explicit ResourceList(DataType&& data)
			: _data(std::move(data))
		{}

		auto get(Size index) { return _data[index]; }
		auto get(Size index) const { return _data[index]; }

		auto operator[](Size index) { return get(index); }
		auto operator[](Size index) const { return get(index); }

		UC_NODISCARD const DataType& data() const { return _data; }
		UC_NODISCARD Size size() const { return _data.size(); }
		UC_NODISCARD Bool empty() const { return _data.empty(); }

		UC_NODISCARD Size get_system_memory_use() const override { return sizeof(ResourceList); }
		UC_NODISCARD Size get_used_resources(Set<Shared<Resource>>& resources) override
		{
			resources.insert(_data.begin(), _data.end());
			return _data.size();
		}

	protected:
		const DataType _data;
	};

	template<typename T,
		std::enable_if_t<std::is_base_of_v<Resource, T>>* = nullptr>
	class ResourceDictionary : public Resource
	{
		UC_OBJECT(ResourceDictionary, Resource)
	public:
		using ResourceList = List<Shared<T>>;
		using NamesDictionary = Dictionary<String, UInt16>;

		ResourceDictionary(ResourceList&& list, NamesDictionary&& names)
			:_list(std::move(list)), _names(std::move(names))
		{}

		UC_NODISCARD const ResourceList& list() const { return _list; }
		UC_NODISCARD const NamesDictionary& names() const { return _names; }

		UC_NODISCARD Size get_system_memory_use() const override { return sizeof(ResourceDictionary); }
		UC_NODISCARD Size get_used_resources(Set<Shared<Resource>>& resources) override
		{
			resources.insert(_list.begin(), _list.end());
			return _list.size();
		}

		Shared<T> find(StringView name) const
		{
			const auto it = _names.find(name.data());
			return it != _names.end() ? _list[it->second] : nullptr;
		}

	protected:
		const ResourceList _list;
		const NamesDictionary _names;
	};
}