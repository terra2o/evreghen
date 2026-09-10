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

#include "evreghen/resource.hpp"

#include <fstream>
#include <sstream>

namespace evreghen {

bool Resource::saveToFile(const std::filesystem::path &filePath) const
{
	std::ofstream file(filePath);
	if (!file.is_open()) {
		return false;
	}
	file << toJson().dump(4) << '\n';
	return true;
}

bool Resource::loadFromFile(const std::filesystem::path &filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open()) {
		return false;
	}

	nlohmann::json j;
	file >> j;
	fromJson(j);
	return true;
}

std::unique_ptr<Resource>
Resource::loadResource(const std::filesystem::path &filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open()) {
		return nullptr;
	}

	nlohmann::json j;
	file >> j;

	std::string type = j.value("type", "");
	std::unique_ptr<Resource> res;
	if (type == "npc") {
		res = std::make_unique<NPCResource>();
	} else if (type == "enemy") {
		res = std::make_unique<EnemyResource>();
	} else {
		return nullptr;
	}

	res->fromJson(j);
	return res;
}

NPCResource::NPCResource(std::string name, std::vector<std::string> dialogues,
                         char glyph)
	: dialogues_(std::move(dialogues)), glyph_(glyph)
{
	name_ = std::move(name);
}

nlohmann::json NPCResource::toJson() const
{
	return nlohmann::json{
			{"type", "npc"},
			{"name", name_},
			{"glyph", std::string(1, glyph_)},
			{"dialogues", dialogues_},
	};
}

void NPCResource::fromJson(const nlohmann::json &j)
{
	name_ = j.value("name", "NPC");
	std::string glyphStr = j.value("glyph", "@");
	if (!glyphStr.empty()) {
		glyph_ = glyphStr[0];
	}
	dialogues_ = j.value("dialogues", std::vector<std::string>{});
}

EnemyResource::EnemyResource(std::string name, int maxHealth, int attackPower,
                             char glyph)
	: maxHealth_(maxHealth), attackPower_(attackPower), glyph_(glyph)
{
	name_ = std::move(name);
}

nlohmann::json EnemyResource::toJson() const
{
	return nlohmann::json{
			{"type", "enemy"},
			{"name", name_},
			{"glyph", std::string(1, glyph_)},
			{"maxHealth", maxHealth_},
			{"attackPower", attackPower_},
	};
}

void EnemyResource::fromJson(const nlohmann::json &j)
{
	name_ = j.value("name", "Enemy");
	std::string glyphStr = j.value("glyph", "E");
	if (!glyphStr.empty()) {
		glyph_ = glyphStr[0];
	}
	maxHealth_ = j.value("maxHealth", 30);
	attackPower_ = j.value("attackPower", 10);
}

} // namespace evreghen
