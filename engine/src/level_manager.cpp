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

#include "evreghen/level_manager.hpp"
#include "evreghen/map_loader.hpp"

#include <utility>

namespace evreghen {

void LevelManager::addLevelFromFile(const std::filesystem::path &filePath)
{
	levels_.push_back(LevelEntry{.type = LevelSourceType::File,
								 .source = filePath.string()});
}

void LevelManager::addLevelFromJson(std::string jsonStr)
{
	levels_.push_back(LevelEntry{.type = LevelSourceType::JsonString,
								 .source = std::move(jsonStr)});
}

bool LevelManager::loadLevel(size_t index)
{
	if (index >= levels_.size()) {
		return false;
	}

	std::unique_ptr<Player> existingPlayer = currentMap_.releasePlayer();
	const auto &entry = levels_[index];

	if (entry.type == LevelSourceType::File) {
		currentMap_ = MapLoader::loadFromFile(entry.source, registry_,
											  std::move(existingPlayer));
	} else {
		currentMap_ = MapLoader::loadFromJsonString(entry.source, registry_,
													std::move(existingPlayer));
	}

	currentLevelIndex_ = index;
	return true;
}

bool LevelManager::nextLevel()
{
	if (!hasNextLevel()) {
		return false;
	}
	return loadLevel(currentLevelIndex_ + 1);
}

bool LevelManager::hasNextLevel() const noexcept
{
	return currentLevelIndex_ + 1 < levels_.size();
}

size_t LevelManager::getCurrentLevelIndex() const noexcept
{
	return currentLevelIndex_;
}

size_t LevelManager::getLevelCount() const noexcept
{
	return levels_.size();
}

Map &LevelManager::getCurrentMap() noexcept
{
	return currentMap_;
}

const Map &LevelManager::getCurrentMap() const noexcept
{
	return currentMap_;
}

TileRegistry &LevelManager::getRegistry() noexcept
{
	return registry_;
}

const TileRegistry &LevelManager::getRegistry() const noexcept
{
	return registry_;
}

Player *LevelManager::getPlayer() noexcept
{
	return currentMap_.getPlayer();
}

const Player *LevelManager::getPlayer() const noexcept
{
	return currentMap_.getPlayer();
}

} // namespace evreghen
