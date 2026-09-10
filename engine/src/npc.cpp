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

#include "evreghen/npc.hpp"
#include "evreghen/player.hpp"

#include <utility>

namespace evreghen {

NPC::NPC(std::string name, int x, int y, std::vector<std::string> dialogues)
	: Entity(std::move(name), 'N', x, y), dialogues_(std::move(dialogues))
{
}

void NPC::addDialogue(std::string line)
{
	dialogues_.push_back(std::move(line));
}

void NPC::setDialogues(std::vector<std::string> dialogues)
{
	dialogues_ = std::move(dialogues);
	currentDialogueIdx_ = 0;
}

const std::vector<std::string> &NPC::getDialogues() const noexcept
{
	return dialogues_;
}

std::string NPC::interact(Player & /* player */)
{
	if (dialogues_.empty()) {
		return name_ + " looks at you quietly.";
	}

	const std::string &line = dialogues_[currentDialogueIdx_];
	currentDialogueIdx_ = (currentDialogueIdx_ + 1) % dialogues_.size();
	return name_ + ": \"" + line + "\"";
}

} // namespace evreghen
