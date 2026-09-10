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

// tile archetype registry and tiled gid arithmetic

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>

namespace evreghen {

inline constexpr int templateImageWidth = 256;
inline constexpr int templateImageHeight = 256;
inline constexpr int templateTileWidth = 32;
inline constexpr int templateTileHeight = 32;
inline constexpr int templateColumns = templateImageWidth / templateTileWidth;

// metadata and physical properties for a single tile archetype
struct TileDefinition {
	uint32_t id{0};
	std::string name;
	char glyph{' '};
	bool solid{false};
	bool isEntity{false};
	bool isTransition{false};
};

// maps global tile ids to definitions and handles grid coordinate math
class TileRegistry {
public:
	TileRegistry();

	// calculates 1-based tile gid from tileset grid coordinates
	static constexpr uint32_t coordsToId(int tx, int ty,
										 int columns = templateColumns,
										 uint32_t firstGid = 1) noexcept
	{
		return static_cast<uint32_t>(ty * columns + tx) + firstGid;
	}

	// converts 1-based tile gid back to 0-indexed column and row coordinates
	static constexpr std::pair<int, int>
	idToCoords(uint32_t id, int columns = templateColumns,
			   uint32_t firstGid = 1) noexcept
	{
		int index = static_cast<int>(id - firstGid);
		return {index % columns, index / columns};
	}

	void registerTile(const TileDefinition &def);
	void registerTile(int tx, int ty, const std::string &name, char glyph,
					  bool solid = false, bool isEntity = false,
					  bool isTransition = false);

	void unregisterTile(uint32_t id);
	void unregisterTile(int tx, int ty);

	[[nodiscard]] const TileDefinition *getTile(uint32_t id) const;
	[[nodiscard]] bool hasTile(uint32_t id) const;

	void clear();
	void registerDefaults();

private:
	std::unordered_map<uint32_t, TileDefinition> tiles_;
};

} // namespace evreghen
