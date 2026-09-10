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

// raii curses terminal renderer and input handling

#include "evreghen/map.hpp"
#include "evreghen/tile_registry.hpp"

#include <string>

namespace evreghen {

// raii wrapper around curses window management and map drawing
class CursesRenderer {
public:
	CursesRenderer();
	~CursesRenderer();

	CursesRenderer(const CursesRenderer &) = delete;
	CursesRenderer &operator=(const CursesRenderer &) = delete;

	CursesRenderer(CursesRenderer &&other) noexcept;
	CursesRenderer &operator=(CursesRenderer &&other) noexcept;

	// draws tile layer, characters, and bottom status bar in a single frame
	void render(const Map &map, const TileRegistry &registry,
				const std::string &statusMsg = "");

	// blocking read for keypress via getch()
	[[nodiscard]] int getInput() const;

private:
	bool initialized_{false};
};

} // namespace evreghen
