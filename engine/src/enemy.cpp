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

#include "evreghen/enemy.hpp"
#include "evreghen/player.hpp"

#include <algorithm>
#include <utility>

namespace evreghen {

Enemy::Enemy(std::string name, int x, int y, int health, int attackPower)
	: Entity(std::move(name), 'E', x, y), health_(health), maxHealth_(health),
	  attackPower_(attackPower)
{
}

int Enemy::getHealth() const noexcept
{
	return health_;
}

int Enemy::getMaxHealth() const noexcept
{
	return maxHealth_;
}

int Enemy::getAttackPower() const noexcept
{
	return attackPower_;
}

bool Enemy::isAlive() const noexcept
{
	return health_ > 0;
}

void Enemy::takeDamage(int amount) noexcept
{
	health_ = std::max(0, health_ - amount);
}

// TODO: make enemies follow player AFTER enemies see the player
std::string Enemy::attack(Player &player)
{
	if (!isAlive()) {
		return name_ + " is defeated.";
	}

	player.takeDamage(attackPower_);
	return name_ + " attacks you for " + std::to_string(attackPower_) +
		   " damage!";
}

std::string Enemy::interact(Player &player)
{
	if (!isAlive()) {
		return name_ + " lies defeated on the ground.";
	}

	// enemy strikes first, player counter-attacks
	player.takeDamage(attackPower_);
	takeDamage(player.getAttackPower());

	if (!isAlive()) {
		return name_ + " struck for " + std::to_string(attackPower_) +
			   " damage, but you defeated it!";
	}

	return name_ + " hit you for " + std::to_string(attackPower_) +
		   " damage! You countered for " +
		   std::to_string(player.getAttackPower()) + " (" +
		   std::to_string(health_) + " HP remaining).";
}

} // namespace evreghen
