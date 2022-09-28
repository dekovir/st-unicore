#include "unicore/ResourceCache.hpp"
#include "unicore/Math.hpp"
#include "unicore/Memory.hpp"
#include "unicore/Logger.hpp"
#include "unicore/FileProvider.hpp"
#include "unicore/RendererResource.hpp"
#include "unicore/ResourceLoader.hpp"
#include "unicore/ResourceCreator.hpp"
#include "unicore/ResourceConverter.hpp"

namespace unicore
{
	static auto& s_resource_type = get_type<Resource>();

	namespace linq
	{
		template<typename T, typename TIterator>
		class IteratorRange
		{
		public:
			constexpr IteratorRange(TIterator begin, TIterator end)
				: _begin(begin), _end(end)
			{}

			UC_NODISCARD bool any(const Predicate<T>& predicate) const
			{
				return std::any_of(_begin, _end, predicate);
			}

			UC_NODISCARD bool all(const Predicate<T>& predicate) const
			{
				return std::all_of(_begin, _end, predicate);
			}

			UC_NODISCARD bool none(const Predicate<T>& predicate) const
			{
				return std::none_of(_begin, _end, predicate);
			}

			UC_NODISCARD TIterator find(const Predicate<T>& predicate) const
			{
				return std::find_if(_begin, _end, predicate);
			}

		protected:
			TIterator _begin, _end;
		};

		template<typename T>
		static auto from(const List<T>& list)
		{
			return IteratorRange<T, typename List<T>::const_iterator>(list.begin(), list.end());
		}

		template<typename T>
		static auto from_reverse(const List<T>& list)
		{
			return IteratorRange<T, typename List<T>::const_iterator>(list.rbegin(), list.rend());
		}
	}

	ResourceCache::ResourceCache(Logger& logger, ReadFileProvider& provider)
		: _logger(logger), _provider(provider)
	{
	}

	void ResourceCache::unload_all()
	{
		_cached.clear();
		_resources.clear();
	}

	void ResourceCache::unload_unused()
	{
		for (auto it = _cached.begin(); it != _cached.end();)
		{
			auto& info = it->second;
			if (info.resource.use_count() == 1)
			{
				UC_LOG_DEBUG(_logger) << "Unload resource "
					<< info.resource->type() << " from " << info.path;
				it = _cached.erase(it);
			}
			else ++it;
		}

		// Remove expired weak pointers
		for (auto it = _resources.begin(); it != _resources.end();)
		{
			if (it->use_count() > 0) ++it;
			else it = _resources.erase(it);
		}
	}

	Shared<Resource> ResourceCache::find(const Path& path,
		TypeConstRef type, const ResourceOptions* options) const
	{
		const size_t hash = make_hash(path, options);
		const auto range = _cached.equal_range(hash);
		for (auto it = range.first; it != range.second; ++it)
		{
			auto& info = it->second;
			auto res_type = info.resource->type();
			if (res_type.is_derived_from(type))
				return info.resource;
		}

		return nullptr;
	}

	Optional<Path> ResourceCache::find_path(const Resource& resource) const
	{
		for (const auto& it : _cached)
		{
			auto& info = it.second;
			if (info.resource.get() == &resource)
				return info.path;
		}

		return std::nullopt;
	}

	Shared<Resource> ResourceCache::create(TypeConstRef type, const Any& value)
	{
		if (value.type() == typeid(Path))
		{
			const auto path = std::any_cast<Path>(value);
			return load(path, type, nullptr, ResourceCacheFlags::Zero);
		}

		const auto it = _creators.find(&type);
		if (it == _creators.end())
		{
			UC_LOG_WARNING(_logger) << "No creator added for " << type;
			return nullptr;
		}

		const ResourceCreator::Options context{ *this, &_logger };

		for (const auto& creator : it->second)
		{
			if (!creator->can_create(value))
				continue;

			if (auto resource = creator->create(context, value); resource != nullptr)
			{
				UC_LOG_DEBUG(_logger) << "Created " << type << " from " << value.type().name()
					<< " " << MemorySize{ resource->get_system_memory_use() };
				_resources.push_back(resource);
				return resource;
			}

			UC_LOG_ERROR(_logger) << "Failed to load " << type
				<< " with creator " << creator->type()
				<< " with value" << value.type();
		}

		UC_LOG_WARNING(_logger) << "No " << type << " creator found for '"
			<< value.type().name() << "' value type";

		return nullptr;
	}

	Shared<Resource> ResourceCache::load(const Path& path,
		TypeConstRef type, const ResourceOptions* options, ResourceCacheFlags flags)
	{
		if (auto resource_find = find(path, type, options))
			return resource_find;

		const auto logger = !flags.has(ResourceCacheFlag::Quiet) ? &_logger : nullptr;

		if (auto resource = load_resource(path, type, options, flags, logger))
			return resource;

		if (const auto it = _converters.find(&type); it != _converters.end())
		{
			for (const auto& converter : it->second)
			{
				// TODO: Handle cyclic loading
				if (auto raw = load(path, converter->raw_type(), options, flags))
				{
					if (auto resource = converter->convert(*raw, { *this, logger }))
					{
						// TODO: Optimize (get path directly from load function)
						const auto converted_path = find_path(*raw);
						_resources.push_back(resource);
						add_resource(resource, converted_path.value_or(path), options);
						return resource;
					}
				}
			}
		}

		UC_LOG_WARNING(logger) << "Failed to load " << type << " from " << path;
		return nullptr;
	}

	void ResourceCache::dump_used()
	{
		unsigned index = 0;
		MemorySize sys_mem{ 0 };

		UC_LOG_INFO(_logger) << "Used resource dump";
		UC_LOG_INFO(_logger) << "----------------------------------";

		for (const auto& weak : _resources)
		{
			if (const auto resource = weak.lock())
			{
				const auto memory_use = MemorySize{ resource->get_system_memory_use() };
				sys_mem += memory_use;

				auto path = find_path(*resource);

				UC_LOG_INFO(_logger) << index << ": "
					<< (path.has_value() ? path.value() : Path::Empty)
					<< " " << resource->type()
					<< " [" << resource.use_count()
					<< ", " << memory_use << "]";
				index++;
			}
		}

		UC_LOG_INFO(_logger) << "----------------------------------";
		UC_LOG_INFO(_logger) << "Used system memory: " << sys_mem;
	}

	void ResourceCache::calc_memory_use(size_t* system, size_t* video) const
	{
		if (system == nullptr && video == nullptr) return;

		if (system != nullptr)
			*system = 0;

		if (video != nullptr)
			*video = 0;

		for (const auto& it : _cached)
		{
			const auto& info = it.second;
			if (system != nullptr)
				*system += info.resource->get_system_memory_use();
			if (video != nullptr)
			{
				if (const auto render_resource = std::dynamic_pointer_cast<RendererResource>(info.resource))
					*video += render_resource->get_video_memory_use();
			}
		}
	}

	void ResourceCache::add_loader(const Shared<ResourceLoader>& loader)
	{
		for (auto type = &loader->resource_type(); type != nullptr; type = type->parent)
		{
			_loaders[type].insert(loader);
			if (type == &s_resource_type)
				break;
		}
	}

	void ResourceCache::add_converter(const Shared<ResourceConverter>& converter)
	{
		_converters[&converter->resource_type()].insert(converter);
	}

	void ResourceCache::add_creator(const Shared<ResourceCreator>& creator)
	{
		for (auto type = &creator->resource_type(); type != nullptr && type != &s_resource_type; type = type->parent)
			_creators[type].insert(creator);
	}

	void ResourceCache::unregister_module(const ModuleContext& context)
	{
		Module::unregister_module(context);

		_loaders.clear();
		_converters.clear();
		_creators.clear();

		unload_all();
	}

	// ============================================================================
	bool ResourceCache::LoaderSort::operator()(const Shared<ResourceLoader>& lhs, const Shared<ResourceLoader>& rhs) const
	{
		if (lhs->priority() == rhs->priority())
			return lhs < rhs;
		return lhs->priority() < rhs->priority();
	}

	bool ResourceCache::CreatorSort::operator()(const Shared<ResourceCreator>& lhs, const Shared<ResourceCreator>& rhs) const
	{
		if (lhs->priority() == rhs->priority())
			return lhs < rhs;
		return lhs->priority() < rhs->priority();
	}

	Shared<Resource> ResourceCache::load_resource(const Path& path, TypeConstRef type,
		const ResourceOptions* options, ResourceCacheFlags flags, Logger* logger)
	{
		const auto it = _loaders.find(&type);
		if (it == _loaders.end()) return nullptr;

		//const TypeInfo* data_type = options ? &typeid(*options) : nullptr;

		const auto& loaders = it->second;
		// TODO: Implement loading stack for prevent recursive loading
		if (path.has_extension(Path::WildcardExt))
		{
			for (const auto& loader : loaders)
			{
				for (const auto& extension : loader->extension())
				{
					Path new_path(path);
					new_path.replace_extension(extension);

					const auto stream = _provider.open_read(new_path);
					if (!stream)
						continue;

					if (auto resource = load_resource(*loader, new_path, *stream, options, logger))
						return resource;
				}
			}
		}
		else
		{
			const auto stream = _provider.open_read(path);
			if (!stream)
			{
				UC_LOG_WARNING(logger) << "Failed to open " << path;
				return nullptr;
			}

			const auto extension = path.extension();
			for (const auto& loader : loaders)
			{
				if (!flags.has(ResourceCacheFlag::IgnoreExtension) && !linq::from(loader->extension())
					.any([&extension](const WStringView ext) -> bool { return extension == ext; }))
					continue;

				//const auto& data_types = loader->data_types();
				//if (!data_types.empty() && !linq::from(data_types)
				//	.any([&data_type](const TypeIndex type) { return data_type == type; }))
				//	continue;

				if (auto resource = load_resource(*loader, path, *stream, options, logger))
					return resource;
			}
		}

		return nullptr;
	}

	Shared<Resource> ResourceCache::load_resource(ResourceLoader& loader,
		const Path& path, ReadFile& file, const ResourceOptions* options, Logger* logger)
	{
		if (auto resource = loader.load({ path, *this, file, options, logger }))
		{
			_resources.push_back(resource);
			add_resource(resource, path, options);

			return resource;
		}

		return nullptr;
	}

	bool ResourceCache::add_resource(const Shared<Resource>& resource,
		const Path& path, const ResourceOptions* options)
	{
		if (resource->cache_policy() == ResourceCachePolicy::CanCache)
		{
			const auto hash = make_hash(path, options);
			const auto& type = resource->type();

			CachedInfo info{ resource, path };
			_cached.emplace(hash, info);
			UC_LOG_DEBUG(_logger) << "Added " << type << " from " << path
				<< " " << MemorySize{ resource->get_system_memory_use() };
			return true;
		}

		return false;
	}

	size_t ResourceCache::make_hash(const Path& path, const ResourceOptions* options)
	{
		return options
			? Math::hash(path, options->hash())
			: Math::hash(path);
	}
}