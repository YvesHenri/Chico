#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include <string>
#include <functional>
#include <unordered_map>
#include <filesystem> // C++17
#include <typeindex>

#include "../Log.h"

template <typename Resource>
class ResourceStore final
{
public:
	static void load(const std::string& resourceFolder) {
		load(resourceFolder, [](const std::string&) {
			return Resource();
		});
	}

	static void load(const std::string& resourceFolder, const std::function<Resource(const std::string&)>& allocator) {
		if (resources.empty()) {
			for (const auto& entry : std::filesystem::recursive_directory_iterator(resourceFolder)) {
				if (entry.is_regular_file()) {
					auto path = entry.path().string();
					auto name = entry.path().filename().string();
					
					if (resources.emplace(name, allocator(path)).second) {
						TRACE("Stored resource %s", path.c_str());
					}
				}
			}
		}
	}

	static Resource& get(const std::string& resourceName) {
		return resources[resourceName];
	}

private:
	static std::unordered_map<std::string, Resource> resources;
};

template <typename Resource>
std::unordered_map<std::string, Resource> ResourceStore<Resource>::resources;

using FontStore = ResourceStore<sf::Font>;
using TextureStore = ResourceStore<sf::Texture>;