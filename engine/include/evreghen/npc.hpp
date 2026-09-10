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

// non-player character with multi-line dialogue support

#include "evreghen/entity.hpp"

#include <vector>

namespace evreghen {

// non-player character with interactive dialogue lines that cycle on talk
class NPC : public Entity {
public:
	NPC(std::string name, int x, int y,
		std::vector<std::string> dialogues = {});

	void addDialogue(std::string line);
	void setDialogues(std::vector<std::string> dialogues);
	[[nodiscard]] const std::vector<std::string> &getDialogues() const noexcept;

	// advances to next line and returns formatted dialogue string
	std::string interact(Player &player) override;

private:
	std::vector<std::string> dialogues_;
	size_t currentDialogueIdx_{0};
};

} // namespace evreghen
