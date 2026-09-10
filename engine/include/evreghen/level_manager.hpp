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

// multi-level campaign sequencing with player state carryover

#include "evreghen/map.hpp"
#include "evreghen/tile_registry.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace evreghen {

// multi-level campaign manager that transfers player state across scenes
class LevelManager {
public:
	enum class LevelSourceType { File, JsonString };

	struct LevelEntry {
		LevelSourceType type{LevelSourceType::File};
		std::string source;
	};

	LevelManager() = default;

	void addLevelFromFile(const std::filesystem::path &filePath);
	void addLevelFromJson(std::string jsonStr);

	// releases existing player instance and injects it into newly loaded map
	bool loadLevel(size_t index);
	// advances to subsequent floor if available
	bool nextLevel();
	[[nodiscard]] bool hasNextLevel() const noexcept;

	[[nodiscard]] size_t getCurrentLevelIndex() const noexcept;
	[[nodiscard]] size_t getLevelCount() const noexcept;

	[[nodiscard]] Map &getCurrentMap() noexcept;
	[[nodiscard]] const Map &getCurrentMap() const noexcept;

	[[nodiscard]] TileRegistry &getRegistry() noexcept;
	[[nodiscard]] const TileRegistry &getRegistry() const noexcept;

	[[nodiscard]] Player *getPlayer() noexcept;
	[[nodiscard]] const Player *getPlayer() const noexcept;

private:
	std::vector<LevelEntry> levels_;
	size_t currentLevelIndex_{0};
	TileRegistry registry_;
	Map currentMap_;
};

} // namespace evreghen
