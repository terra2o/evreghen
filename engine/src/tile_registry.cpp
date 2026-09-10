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

#include "evreghen/tile_registry.hpp"

namespace evreghen {

TileRegistry::TileRegistry()
{
	registerDefaults();
}

void TileRegistry::registerTile(const TileDefinition &def)
{
	tiles_[def.id] = def;
}

void TileRegistry::registerTile(int tx, int ty, const std::string &name,
								char glyph, bool solid, bool isEntity,
								bool isTransition)
{
	uint32_t id = coordsToId(tx, ty);
	tiles_[id] = TileDefinition{.id = id,
								.name = name,
								.glyph = glyph,
								.solid = solid,
								.isEntity = isEntity,
								.isTransition = isTransition};
}

void TileRegistry::unregisterTile(uint32_t id)
{
	tiles_.erase(id);
}

void TileRegistry::unregisterTile(int tx, int ty)
{
	tiles_.erase(coordsToId(tx, ty));
}

const TileDefinition *TileRegistry::getTile(uint32_t id) const
{
	auto it = tiles_.find(id);
	if (it != tiles_.end()) {
		return &it->second;
	}
	return nullptr;
}

bool TileRegistry::hasTile(uint32_t id) const
{
	return tiles_.contains(id);
}

void TileRegistry::clear()
{
	tiles_.clear();
}

void TileRegistry::registerDefaults()
{
	// standard 256x256 tileset layout (32x32 tiles, 8 per row)
	registerTile(0, 0, "space", ' ', false, false, false);
	registerTile(1, 0, "ground", '.', false, false, false);
	registerTile(2, 0, "path", '=', false, false, false);
	registerTile(3, 0, "stairs_down", '>', false, false, true);
	registerTile(4, 0, "stairs_up", '<', false, false, true);
	registerTile(0, 1, "water", '~', true, false, false);
	registerTile(1, 1, "wall", '#', true, false, false);
	registerTile(0, 2, "player", '@', true, true, false);
	registerTile(1, 2, "enemy", 'E', true, true, false);
	registerTile(2, 2, "npc", 'N', true, true, false);
}

} // namespace evreghen
