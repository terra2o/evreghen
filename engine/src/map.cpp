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

#include "evreghen/map.hpp"

#include <utility>

namespace evreghen {

Map::Map(int width, int height)
	: width_(width), height_(height),
	  tiles_(static_cast<size_t>(width * height), 1)
{
}

int Map::getWidth() const noexcept
{
	return width_;
}

int Map::getHeight() const noexcept
{
	return height_;
}

void Map::resize(int width, int height, uint32_t defaultTile)
{
	width_ = width;
	height_ = height;
	tiles_.assign(static_cast<size_t>(width * height), defaultTile);
}

bool Map::inBounds(int x, int y) const noexcept
{
	return x >= 0 && x < width_ && y >= 0 && y < height_;
}

uint32_t Map::getTile(int x, int y) const
{
	if (!inBounds(x, y)) {
		return 0;
	}
	return tiles_[static_cast<size_t>(y * width_ + x)];
}

void Map::setTile(int x, int y, uint32_t tileId)
{
	if (inBounds(x, y)) {
		tiles_[static_cast<size_t>(y * width_ + x)] = tileId;
	}
}

bool Map::isWalkable(int x, int y, const TileRegistry &registry) const
{
	if (!inBounds(x, y)) {
		return false;
	}

	uint32_t id = getTile(x, y);
	const TileDefinition *def = registry.getTile(id);
	if (def != nullptr && def->solid) {
		return false;
	}

	for (const auto &npc : npcs_) {
		if (npc && npc->getX() == x && npc->getY() == y) {
			return false;
		}
	}

	for (const auto &enemy : enemies_) {
		if (enemy && enemy->isAlive() && enemy->getX() == x &&
			enemy->getY() == y) {
			return false;
		}
	}

	return true;
}

bool Map::isTransitionAt(int x, int y, const TileRegistry &registry) const
{
	if (!inBounds(x, y)) {
		return false;
	}
	uint32_t id = getTile(x, y);
	const TileDefinition *def = registry.getTile(id);
	return def != nullptr && def->isTransition;
}

void Map::setPlayer(std::unique_ptr<Player> player)
{
	player_ = std::move(player);
}

std::unique_ptr<Player> Map::releasePlayer() noexcept
{
	return std::move(player_);
}

Player *Map::getPlayer() noexcept
{
	return player_.get();
}

const Player *Map::getPlayer() const noexcept
{
	return player_.get();
}

void Map::addNpc(std::unique_ptr<NPC> npc)
{
	npcs_.push_back(std::move(npc));
}

const std::vector<std::unique_ptr<NPC>> &Map::getNpcs() const noexcept
{
	return npcs_;
}

std::vector<std::unique_ptr<NPC>> &Map::getNpcs() noexcept
{
	return npcs_;
}

void Map::addEnemy(std::unique_ptr<Enemy> enemy)
{
	enemies_.push_back(std::move(enemy));
}

const std::vector<std::unique_ptr<Enemy>> &Map::getEnemies() const noexcept
{
	return enemies_;
}

std::vector<std::unique_ptr<Enemy>> &Map::getEnemies() noexcept
{
	return enemies_;
}

Entity *Map::getEntityAt(int x, int y)
{
	if (player_ && player_->getX() == x && player_->getY() == y) {
		return player_.get();
	}
	for (auto &npc : npcs_) {
		if (npc && npc->getX() == x && npc->getY() == y) {
			return npc.get();
		}
	}
	for (auto &enemy : enemies_) {
		if (enemy && enemy->getX() == x && enemy->getY() == y &&
			enemy->isAlive()) {
			return enemy.get();
		}
	}
	return nullptr;
}

const Entity *Map::getEntityAt(int x, int y) const
{
	if (player_ && player_->getX() == x && player_->getY() == y) {
		return player_.get();
	}
	for (const auto &npc : npcs_) {
		if (npc && npc->getX() == x && npc->getY() == y) {
			return npc.get();
		}
	}
	for (const auto &enemy : enemies_) {
		if (enemy && enemy->getX() == x && enemy->getY() == y &&
			enemy->isAlive()) {
			return enemy.get();
		}
	}
	return nullptr;
}

Entity *Map::findAdjacentInteractable(int x, int y, int distance)
{
	for (auto &npc : npcs_) {
		if (npc && npc->isNear(x, y, distance)) {
			return npc.get();
		}
	}
	for (auto &enemy : enemies_) {
		if (enemy && enemy->isAlive() && enemy->isNear(x, y, distance)) {
			return enemy.get();
		}
	}
	return nullptr;
}

} // namespace evreghen
