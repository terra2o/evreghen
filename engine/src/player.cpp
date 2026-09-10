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

#include "evreghen/player.hpp"

#include <algorithm>

namespace evreghen {

Player::Player(int x, int y, int health, int attackPower)
	: Entity("Player", '@', x, y), health_(health), maxHealth_(health),
	  attackPower_(attackPower)
{
}

int Player::getHealth() const noexcept
{
	return health_;
}

int Player::getMaxHealth() const noexcept
{
	return maxHealth_;
}

int Player::getAttackPower() const noexcept
{
	return attackPower_;
}

bool Player::isAlive() const noexcept
{
	return health_ > 0;
}

void Player::takeDamage(int amount) noexcept
{
	health_ = std::max(0, health_ - amount);
}

void Player::heal(int amount) noexcept
{
	health_ = std::min(maxHealth_, health_ + amount);
}

std::string Player::interact(Player & /* player */)
{
	return "You check your gear. HP: " + std::to_string(health_) + "/" +
		   std::to_string(maxHealth_);
}

} // namespace evreghen
