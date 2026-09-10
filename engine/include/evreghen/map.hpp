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

// 2d tile grid and entity container for a playable game scene

#include "evreghen/enemy.hpp"
#include "evreghen/npc.hpp"
#include "evreghen/player.hpp"
#include "evreghen/tile_registry.hpp"

#include <memory>
#include <vector>

namespace evreghen {

// 2d tile grid and entity container for a playable game scene
class Map {
public:
	Map(int width = 0, int height = 0);

	[[nodiscard]] int getWidth() const noexcept;
	[[nodiscard]] int getHeight() const noexcept;

	void resize(int width, int height, uint32_t defaultTile = 1);

	[[nodiscard]] bool inBounds(int x, int y) const noexcept;
	[[nodiscard]] uint32_t getTile(int x, int y) const;
	void setTile(int x, int y, uint32_t tileId);

	// checks grid bounds, solid tile flag, and living entity obstruction
	[[nodiscard]] bool isWalkable(int x, int y,
								  const TileRegistry &registry) const;
	// checks if coordinate contains a transition tile like stairs
	[[nodiscard]] bool isTransitionAt(int x, int y,
									  const TileRegistry &registry) const;

	void setPlayer(std::unique_ptr<Player> player);
	// relinquishes player ownership to transfer state to the next level
	std::unique_ptr<Player> releasePlayer() noexcept;
	[[nodiscard]] Player *getPlayer() noexcept;
	[[nodiscard]] const Player *getPlayer() const noexcept;

	void addNpc(std::unique_ptr<NPC> npc);
	[[nodiscard]] const std::vector<std::unique_ptr<NPC>> &
	getNpcs() const noexcept;
	[[nodiscard]] std::vector<std::unique_ptr<NPC>> &getNpcs() noexcept;

	void addEnemy(std::unique_ptr<Enemy> enemy);
	[[nodiscard]] const std::vector<std::unique_ptr<Enemy>> &
	getEnemies() const noexcept;
	[[nodiscard]] std::vector<std::unique_ptr<Enemy>> &getEnemies() noexcept;

	[[nodiscard]] Entity *getEntityAt(int x, int y);
	[[nodiscard]] const Entity *getEntityAt(int x, int y) const;

	// finds nearest living npc or enemy within chebyshev range
	[[nodiscard]] Entity *findAdjacentInteractable(int x, int y,
												   int distance = 1);

private:
	int width_{0};
	int height_{0};
	std::vector<uint32_t> tiles_;
	std::unique_ptr<Player> player_;
	std::vector<std::unique_ptr<NPC>> npcs_;
	std::vector<std::unique_ptr<Enemy>> enemies_;
};

} // namespace evreghen
