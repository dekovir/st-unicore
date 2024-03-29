#pragma once
#include "unicore/resource/Resource.hpp"
#include "unicore/xml/XMLNode.hpp"

namespace unicore
{
	// TODO: Implement as const
	class XMLData : public Resource
	{
		UC_OBJECT(XMLData, Resource)
	public:
		UC_NODISCARD size_t get_system_memory_use() const override { return sizeof(XMLData); }
		tinyxml2::XMLDocument doc;

		Bool parse(StringView xml, Logger* logger = nullptr);

		XMLValue<tinyxml2::XMLElement> get_root();
		XMLValue<const tinyxml2::XMLElement> get_root() const;
	};
}