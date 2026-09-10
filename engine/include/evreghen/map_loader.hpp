/* evreghen - A Cross-Platform Game Engine and Build System for Terminal Games
Copyright (C)  2026 Emir Baha Yıldırım <jayshozie@gmail.com>
Copyright (C)  2026 terra2o <terra2o@protonmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#pragma once

// tiled-compatible json serializer and parser for maps and entities

#include "evreghen/map.hpp"
#include "evreghen/tile_registry.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace evreghen {

// tiled-compatible json serializer and parser for maps and entities
class MapLoader {
public:
	// parses tiled layers, spawns entities, and adopts optional carried player
	static Map
	loadFromJsonString(const std::string &jsonStr, const TileRegistry &registry,
	                   std::unique_ptr<Player> existingPlayer = nullptr,
	                   uint32_t defaultGroundTile = 2);

	static Map loadFromFile(const std::filesystem::path &filePath,
	                        const TileRegistry &registry,
	                        std::unique_ptr<Player> existingPlayer = nullptr,
	                        uint32_t defaultGroundTile = 2);

	// writes tilelayer and objectgroup matching tiled 1.10 specifications
	static std::string saveToJsonString(const Map &map);
	static bool saveToFile(const std::filesystem::path &filePath,
	                       const Map &map);
};

} // namespace evreghen
