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

// hostile combatant entity with attack and counter-attack logic

#include "evreghen/entity.hpp"

namespace evreghen {

// hostile entity that damages the player when adjacent or bumped into
class Enemy : public Entity {
public:
	Enemy(std::string name, int x, int y, int health = 30,
		  int attackPower = 10);

	[[nodiscard]] int getHealth() const noexcept;
	[[nodiscard]] int getMaxHealth() const noexcept;
	[[nodiscard]] int getAttackPower() const noexcept;
	[[nodiscard]] bool isAlive() const noexcept;

	void takeDamage(int amount) noexcept;
	// deals damage to player on opportunity attack
	std::string attack(Player &player);

	// executes two-way turn where enemy strikes and player counter-attacks
	std::string interact(Player &player) override;

private:
	int health_{30};
	int maxHealth_{30};
	int attackPower_{10};
};

} // namespace evreghen
