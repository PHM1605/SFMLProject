#pragma once
#include <map>
#include <memory>
#include <cassert>
#include <SFML/Graphics.hpp>
#include <iostream>

template <typename Resource, typename Identifier>
class ResourceHolder {
private:
	std::map<Identifier, std::unique_ptr<Resource>> mResourceMap;
private:
	void insertResource(Identifier id, std::unique_ptr<Resource> resource) {
		auto inserted = mResourceMap.insert(std::make_pair(id, std::move(resource)));
		assert(inserted.second);
	}
public:
	void load(Identifier id, const std::string& filename) {
		std::unique_ptr<Resource> resource(new Resource());
		if (!resource->loadFromFile(filename)) {
			throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);
		}
		insertResource(id, std::move(resource));
	}

	template <typename Parameter>
	void load(Identifier id, const std::string& filename, const Parameter& secondParam) {
		std::unique_ptr<Resource> resource(new Resource());
		if (!resource->loadFromFile(filename, secondParam)) {
			throw std::runtime_error("ResourceHolder::load - Failed to load second params for " + filename);
		}
		insertResource(id, std::move(resource));
	}

	Resource& get(Identifier id) {
		auto found = mResourceMap.find(id);
		assert(found != mResourceMap.end());
		return *found->second;
	}

	const Resource& get(Identifier id) const {
		auto found = mResourceMap.find(id);
		assert(found != mResourceMap.end());
		return *found->second;
	}
};
