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

// player character entity with persistent health and combat stats

#include "evreghen/entity.hpp"

namespace evreghen {

// player character tracking persistent health, attack, and inventory status
class Player : public Entity {
public:
	Player(int x = 0, int y = 0, int health = 100, int attackPower = 15);

	[[nodiscard]] int getHealth() const noexcept;
	[[nodiscard]] int getMaxHealth() const noexcept;
	[[nodiscard]] int getAttackPower() const noexcept;
	[[nodiscard]] bool isAlive() const noexcept;

	void takeDamage(int amount) noexcept;
	// increases hp clamped to max health
	void heal(int amount) noexcept;

	// returns gear and health summary
	std::string interact(Player &player) override;

private:
	int health_{100};
	int maxHealth_{100};
	int attackPower_{15};
};

} // namespace evreghen
