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

// disk-persisted data templates for entities and scenes

#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>

namespace evreghen {

// base asset interface with json serialization and polymorphic factory loader
class Resource {
public:
	virtual ~Resource() = default;

	[[nodiscard]] const std::string &getName() const noexcept { return name_; }
	void setName(std::string name) { name_ = std::move(name); }

	[[nodiscard]] virtual std::string getResourceType() const = 0;

	virtual nlohmann::json toJson() const = 0;
	virtual void fromJson(const nlohmann::json &j) = 0;

	bool saveToFile(const std::filesystem::path &filePath) const;
	bool loadFromFile(const std::filesystem::path &filePath);

	// inspects "type" field and instantiates the matching resource subclass
	static std::unique_ptr<Resource>
	loadResource(const std::filesystem::path &filePath);

protected:
	std::string name_;
};

// serialized npc archetype defining dialogues and display glyph
class NPCResource : public Resource {
public:
	NPCResource(std::string name = "NPC",
	            std::vector<std::string> dialogues = {},
	            char glyph = '@');

	[[nodiscard]] std::string getResourceType() const override { return "npc"; }

	nlohmann::json toJson() const override;
	void fromJson(const nlohmann::json &j) override;

	[[nodiscard]] const std::vector<std::string> &getDialogues() const noexcept {
		return dialogues_;
	}
	void setDialogues(std::vector<std::string> dialogues) {
		dialogues_ = std::move(dialogues);
	}
	void addDialogue(std::string line) {
		dialogues_.push_back(std::move(line));
	}

	[[nodiscard]] char getGlyph() const noexcept { return glyph_; }
	void setGlyph(char glyph) noexcept { glyph_ = glyph; }

private:
	std::vector<std::string> dialogues_;
	char glyph_{'@'};
};

// serialized enemy archetype defining health, attack power, and display glyph
class EnemyResource : public Resource {
public:
	EnemyResource(std::string name = "Enemy", int maxHealth = 30,
	              int attackPower = 10, char glyph = 'E');

	[[nodiscard]] std::string getResourceType() const override {
		return "enemy";
	}

	nlohmann::json toJson() const override;
	void fromJson(const nlohmann::json &j) override;

	[[nodiscard]] int getMaxHealth() const noexcept { return maxHealth_; }
	void setMaxHealth(int hp) noexcept { maxHealth_ = hp; }

	[[nodiscard]] int getAttackPower() const noexcept { return attackPower_; }
	void setAttackPower(int atk) noexcept { attackPower_ = atk; }

	[[nodiscard]] char getGlyph() const noexcept { return glyph_; }
	void setGlyph(char glyph) noexcept { glyph_ = glyph; }

private:
	int maxHealth_{30};
	int attackPower_{10};
	char glyph_{'E'};
};

} // namespace evreghen
