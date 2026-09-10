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

#include "evreghen/entity.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

namespace evreghen {

Entity::Entity(std::string name, char glyph, int x, int y)
	: name_(std::move(name)), glyph_(glyph), x_(x), y_(y)
{
}

int Entity::getX() const noexcept
{
	return x_;
}

int Entity::getY() const noexcept
{
	return y_;
}

void Entity::setPosition(int x, int y) noexcept
{
	x_ = x;
	y_ = y;
}

char Entity::getGlyph() const noexcept
{
	return glyph_;
}

const std::string &Entity::getName() const noexcept
{
	return name_;
}

bool Entity::isNear(int targetX, int targetY, int distance) const noexcept
{
	return std::max(std::abs(x_ - targetX), std::abs(y_ - targetY)) <= distance;
}

bool Entity::isNear(const Entity &other, int distance) const noexcept
{
	return isNear(other.getX(), other.getY(), distance);
}

} // namespace evreghen
