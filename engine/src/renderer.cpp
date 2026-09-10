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

#include "evreghen/renderer.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include <curses.h>
#else
#include <ncurses.h>
#endif

#include <utility>

namespace evreghen {

CursesRenderer::CursesRenderer()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	initialized_ = true;
}

CursesRenderer::~CursesRenderer()
{
	if (initialized_) {
		endwin();
		initialized_ = false;
	}
}

CursesRenderer::CursesRenderer(CursesRenderer &&other) noexcept
	: initialized_(other.initialized_)
{
	other.initialized_ = false;
}

CursesRenderer &CursesRenderer::operator=(CursesRenderer &&other) noexcept
{
	if (this != &other) {
		if (initialized_) {
			endwin();
		}
		initialized_ = other.initialized_;
		other.initialized_ = false;
	}
	return *this;
}

// renders tiles in row-major order, layers alive entities, and draws hud beneath
void CursesRenderer::render(const Map &map, const TileRegistry &registry,
							const std::string &statusMsg)
{
	erase();

	for (int y = 0; y < map.getHeight(); ++y) {
		for (int x = 0; x < map.getWidth(); ++x) {
			uint32_t tileId = map.getTile(x, y);
			const TileDefinition *def = registry.getTile(tileId);
			char glyph = (def != nullptr) ? def->glyph : ' ';
			mvaddch(y, x, glyph);
		}
	}

	for (const auto &npc : map.getNpcs()) {
		if (npc) {
			mvaddch(npc->getY(), npc->getX(), npc->getGlyph());
		}
	}

	for (const auto &enemy : map.getEnemies()) {
		if (enemy && enemy->isAlive()) {
			mvaddch(enemy->getY(), enemy->getX(), enemy->getGlyph());
		}
	}

	const Player *player = map.getPlayer();
	if (player && player->isAlive()) {
		mvaddch(player->getY(), player->getX(), player->getGlyph());
	}

	int hudY = map.getHeight() + 1;
	if (player != nullptr) {
		mvprintw(hudY, 0, "HP: %d/%d | Pos: (%d, %d)", player->getHealth(),
				 player->getMaxHealth(), player->getX(), player->getY());
	}

	mvprintw(
			hudY + 1, 0,
			"Controls: WASD / Arrows = Move | Space / E = Interact | Q = Quit");

	if (!statusMsg.empty()) {
		mvprintw(hudY + 3, 0, "> %s", statusMsg.c_str());
	}

	refresh();
}

// blocks on getch to await user input
int CursesRenderer::getInput() const
{
	return getch();
}

} // namespace evreghen
