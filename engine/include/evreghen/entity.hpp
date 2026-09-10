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

// runtime game entity base class for terminal characters

#include <string>

namespace evreghen {

class Player;

// base interactive map object positioned on discrete grid coordinates
class Entity {
public:
	Entity(std::string name, char glyph, int x, int y);
	virtual ~Entity() = default;

	[[nodiscard]] int getX() const noexcept;
	[[nodiscard]] int getY() const noexcept;
	void setPosition(int x, int y) noexcept;

	[[nodiscard]] char getGlyph() const noexcept;
	[[nodiscard]] const std::string &getName() const noexcept;

	// chebyshev distance check (includes diagonals)
	[[nodiscard]] bool isNear(int targetX, int targetY,
							  int distance = 1) const noexcept;
	[[nodiscard]] bool isNear(const Entity &other,
							  int distance = 1) const noexcept;

	// triggered when the player bumps into or interacts with this entity
	virtual std::string interact(Player &player) = 0;

protected:
	std::string name_;
	char glyph_{'?'};
	int x_{0};
	int y_{0};
};

} // namespace evreghen
